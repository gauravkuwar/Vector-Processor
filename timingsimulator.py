"""
VMIPS Vector Processor Timing Simulator
Authors: Gaurav Kuwar, Ritvik Nair
"""

import os
import argparse

# Constants
REG_COUNT = 8
REG_BITS = 32
MAX_VECTOR_LEN = 64

allVecComputeInstrs = {'ADDVV', 'SUBVV', 'MULVV', 'DIVVV', 'UNPACKHI', 'UNPACKLO', 'PACKLO', 'PACKHI',
                       'ADDVS', 'SUBVS', 'MULVS', 'DIVVS',
                       'SEQVV', 'SNEVV', 'SGTVV', 'SLTVV', 'SGEVV', 'SLEVV',
                       'SEQVS', 'SNEVS', 'SGTVS', 'SLTVS', 'SGEVS', 'SLEVS'}

vectorMaskRegOps = {'SEQVV', 'SNEVV', 'SGTVV', 'SLTVV', 'SGEVV', 'SLEVV',
                    'SEQVS', 'SNEVS', 'SGTVS', 'SLTVS', 'SGEVS', 'SLEVS'}

# HELPERS
def ceil(x):
    # custom ceil func, since we can't use libraries
    return -int((-x) // 1)
    
def padding(a, N=5): # just a helper function to split instructions
    return (a + N * [None])[:N]
    
def regIdx(reg):
    # ex. VR1 -> 1
    # ex. SR1 -> 9
    
    offset = REG_COUNT if reg[0] == "S" else 0
    return int(reg[2:]) + offset

def isReg(op): # checks if the operand is a reg val
    return type(op) == str and (op.startswith("SR") or op.startswith("VR")) and op[2:].isdigit()

def instr2Func(instr):
    if (instr.name in {'ADDVV', 'ADDVS', 'SUBVV', 'SUBVS'} or 
        instr.name in vectorMaskRegOps):
        return "ADD"
    if instr.name in {"MULVV", "MULVS"}:
        return "MUL"
    if instr.name in {"DIVVV", "DIVVS"}:
        return "DIV"
    if instr.name in {'PACKLO', 'PACKHI', 'UNPACKLO', 'UNPACKHI'}:
        return "SHF"
    
class Queue:
    """Queue Abstraction - for convience and is not efficient"""
    def __init__(self, size):
        self.size = size
        self.q = []

    def pop(self):
        return self.q.pop(-1) if not self.empty() else None

    def push(self, val):
        if self.full(): return 1
        self.q.append(val)
        return 0

    def head(self):
        return self.q[0] if not self.empty() else None

    def empty(self):
        return len(self.q) == 0

    def full(self):
        return len(self.q) == self.size

class Instr:
    """Intruction Object"""
    def __init__(self, instrStr):
        self.instrStr = instrStr
        self.name = instrStr.split()[0]
        
        if self.isVecMem() or self.isScalarMem():
            _, self.op1, addrs = instrStr.split(maxsplit=2)
            self.op3, self.vlen = None, None
            
            if self.isVecMem():
                addrs, self.vlen = addrs.rsplit(maxsplit=1)
            
            self.op2 = [int(a) for a in addrs.strip()[1:-1].split(sep=',') if len(a) > 0] # convert addrs from str to list
        else:
            _, self.op1, self.op2, self.op3, self.vlen = padding(instrStr.split())

        if self.vlen:
            self.vlen = int(self.vlen)

    def isVecMem(self):
        return self.name.startswith('LV') or self.name.startswith('SV')

    def isVecCompute(self):
        return self.name in allVecComputeInstrs

    def isScalarMem(self):
        return self.name in {"LS", "SS"}

    def __repr__(self):
        return f"{(self.name, self.op1, self.op2, self.op3, self.vlen)}"

class BusyBoard:
    """
    Busy board object to keep track of which registers are in use. Idx to reg map:
        [0:REG_COUNT] = vector registers,
        [REG_COUNT:2*REG_COUNT] = scalar
        [2*REG_COUNT] = length register (second last)
        [2*REG_COUNT+1] = vector mask register (last)
    """
    
    def __init__(self):
        self.bb = [0] * (REG_COUNT * 2 + 2)

    def regBusy(self, instr):
        "check if the regs in the instr are busy"
        for op in (instr.op1, instr.op2, instr.op3):
            if isReg(op):
                if self.bb[regIdx(op)] == 1:
                    return True
        return False
        
    def add(self, instr):
        "add the regs in instr to busy board"
        # for vector mask
        if instr.name in {"CVM", "POP"} or instr.name in vectorMaskRegOps:
            self.bb[-2] = 1
        
        # for vector len
        if instr.name in {"MTCL", "MFCL"}:
            self.bb[-1] = 1
        
        for op in (instr.op1, instr.op2, instr.op3):
            if isReg(op):
                self.bb[regIdx(op)] = 1
                
    def clear(self, instr):
        "clear regs in instr from busy board"
        # for vector mask
        if instr.name in {"CVM", "POP"} or instr.name in vectorMaskRegOps:
            self.bb[-2] = 0
        
        # for vector len
        if instr.name in {"MTCL", "MFCL"}:
            self.bb[-1] = 0
            
        for op in (instr.op1, instr.op2, instr.op3):
            if isReg(op):
                self.bb[regIdx(op)] = 0

    def __repr__(self):
        return f"{self.bb}"
    
# ---- Compute and Data Unit Classes ----

def shift(lane): 
    # helper func
    prev = lane[0]
    for i in range(1, len(lane)):
        prev, lane[i] = lane[i], prev

    lane[0] = None    
    return prev

class VectorComputeUnit:
    "Goes cycle by cycle per pipeline stage in each lane"
    def __init__(self, pipelineDepth, numOfLanes):
        self.lanes = [[None]*pipelineDepth for _ in range(numOfLanes)] # each row is stages in the lanes
        self.vector = []
        self.i = 0 # vector index
        self.instr = None

    def inputVec(self, vlen):
        self.i = 0
        self.vector = list(range(vlen)) # could just be the same val

    def busy(self):
        for lane in self.lanes:
            for stage in lane:
                if stage is not None:
                    return True
        return False

    def update(self):
        if self.busy() or len(self.vector) > self.i:
            for laneIdx in range(len(self.lanes)):
                shift(self.lanes[laneIdx])

                # add to new to start of lane
                if self.i < len(self.vector):
                    self.lanes[laneIdx][0] = self.vector[self.i]
                    
                self.i += 1
                
            # print(self.lanes)

class VectorDataUnit:
    """
    Goes cycle by cycle per pipeline stage in each lane, and then banks are decoupled
    So any lane can access any bank
    """
    def __init__(self, pipelineDepth, numOfLanes, numOfBanks, bankBusyTime):
        self.lanes = [[None]*pipelineDepth for _ in range(numOfLanes)] # each row is stages in the lanes
        self.addrs = []
        self.i = 0 # vector index
        self.instr = None

        # VDM is total 512000 bytes split across numOfBanks
        self.banks = [0] * numOfBanks
        self.numOfBanks = numOfBanks
        self.bankBusyTime = bankBusyTime

    def inputVec(self, addrs):
        self.i = 0
        self.addrs = addrs

    def busy(self):
        for lane in self.lanes:
            for stage in lane:
                if stage is not None:
                    return True
        return False

    def update(self):
        if self.busy() or len(self.addrs) > self.i:
            for laneIdx in range(len(self.lanes)):
                stalled = False
                if self.lanes[laneIdx][-1] is not None:
                    # this formula allows us to map the specific addr to its bank
                    addr = self.lanes[laneIdx][-1]
                    bankIdx = addr % self.numOfBanks
        
                    # check if the bank is free
                    if self.banks[bankIdx] == 0: 
                        self.banks[bankIdx] = self.bankBusyTime 
                        
                        # we stalled if the bank wasnt free
                        stalled = False
                    else:
                        stalled = True

                if not stalled:
                    popped = shift(self.lanes[laneIdx])
                    # add to new to start of lane 
                    if self.i < len(self.addrs):
                        self.lanes[laneIdx][0] = self.addrs[self.i]
                        
                self.i += 1
    
            # print(self.lanes)
            # print(self.banks)
            
            # update bank state
            for bankIdx in range(self.numOfBanks):
                if self.banks[bankIdx] > 0:
                    self.banks[bankIdx] -= 1

class Config(object):
    def __init__(self, iodir):
        self.filepath = os.path.abspath(os.path.join(iodir, "Config.txt"))
        self.parameters = {} # dictionary of parameter name: value as strings.

        try:
            with open(self.filepath, 'r') as conf:
                self.parameters = {line.split('=')[0].strip(): line.split('=')[1].split('#')[0].strip() for line in conf.readlines() if not (line.startswith('#') or line.strip() == '')}
            print("Config - Parameters loaded from file:", self.filepath)
            print("Config parameters:", self.parameters)
        except:
            print("Config - ERROR: Couldn't open file in path:", self.filepath)
            raise

# main timing sim class
class TimingSim:
    def __init__(self, tracefp, config):

        with open(tracefp) as f:
            trace = f.read().splitlines()

        self.trace  = iter(trace)
        self.stallFetch = False
        self.stallDecode = True
        self.instrBuf = None # stores/buffers output of fetch (i guess this would affect hardware)

        self.pipelineDepth = {'ADD': int(config.parameters["pipelineDepthAdd"]), 
                              'MUL': int(config.parameters["pipelineDepthMul"]),
                              'DIV': int(config.parameters["pipelineDepthDiv"]), 
                              'SHF': int(config.parameters["pipelineDepthShuffle"])}
        
        self.busyboard = BusyBoard()
        
        self.vectorComputeQ = Queue(int(config.parameters["computeQueueDepth"])) # compute ops like addvv, mulvv, etc.
        self.vectorDataQ    = Queue(int(config.parameters["dataQueueDepth"]))    # vector load store ops
        self.scalarQ        = Queue(1)                 # all scalar ops (mem/ex)

        self.units = {func: VectorComputeUnit(self.pipelineDepth[func], int(config.parameters["numLanes"])) for func in ('ADD', 'MUL', 'DIV', 'SHF')} 
        self.vdata = VectorDataUnit(int(config.parameters["vlsPipelineDepth"]), 
                                    int(config.parameters["numLanes"]), 
                                    int(config.parameters["vdmNumBanks"]), 
                                    int(config.parameters["vdmBankBusyTime"]))
        
        # cur scalar states
        self.s_remaining = 0
        self.s_instr = None
    
    # Frontend Funcs
    def fetch(self):
        "returns the next instr line"
        return next(self.trace)

    def decode(self, instrStr):
        instr = Instr(instrStr)

        # check if any registers are busy, and stall if they are
        if self.busyboard.regBusy(instr): # checks data hazards
            self.stallFetch = True
            return

        # set regs high on busy board
        self.busyboard.add(instr)

        # checks if queue is full and stall if it is
        if instr.isVecMem():                
            if self.vectorDataQ.push(instr) == 1:
                self.stallFetch = True
                # stall frontend

        elif instr.isVecCompute():
            # Note: S__VV and S__VS are diff (op3) ??
            if self.vectorComputeQ.push(instr) == 1:
                self.stallFetch = True
                # stall frontend
        
        else: # scalar ops, CVM, POP, MTCL, MFCL
            if self.scalarQ.push(instr) == 1:
                self.stallFetch = True
                # stall frontend

    
    # Backend Funcs
    # --------------
    # We have different func/mem unit classes which simulate
    # the pipelined operation of the units cycle by cycle
    # we also have a busy() method for these classes which allows us to 
    # check if the units are busy, hence avoiding structural hazards
    # By using the busyboard to keep track of registers
    # and stalling instructions when the required registers are busy
    # we resolve data hazards.
    
    def handleVectorMem(self):
        # pop from queue if unit is not busy
        if self.vectorDataQ.empty() or self.vdata.busy(): return
            
        instr = self.vectorDataQ.pop()
        self.vdata.inputVec(instr.op2)
        self.vdata.instr = instr

    def handleFuncUnits(self):
        # changes state of func unit if it is free to run an instr
        # pop from queue if unit is not busy and queue is not empty
        if self.vectorComputeQ.empty(): return

        # check if unit is busy
        funcUnit = instr2Func(self.vectorComputeQ.head())
        if self.units[funcUnit].busy(): return
            
        instr = self.vectorComputeQ.pop()
        self.units[funcUnit].inputVec(instr.vlen)        
        self.units[funcUnit].instr = instr

    def handleScalar(self):
        # changes state of scalar unit if it is free to run an instr
        # pop from queue if unit is not busy and queue is not empty
        if self.scalarQ.empty() or self.s_remaining > 0: return

        instr = self.scalarQ.pop()
        # print("EX SCALAR: ", instr.instrStr)
        self.s_remaining = 1
        self.s_instr = instr
    
    def stop(self):
        "stop simulator when no pipelines running and nothing in queue"
        if not (self.instrBuf == "HALT" and
                self.scalarQ.empty() and
                self.vectorComputeQ.empty() and 
                self.vectorDataQ.empty() and
                self.s_remaining == 0 and
                not self.vdata.busy()):
            return False
        
        for func in self.units:
            if self.units[func].busy():
                return False
                
        return True

    def run(self):
        self.cycle = 0
        
        while not self.stop():
            self.cycle += 1
            # print("CYCLE:", self.cycle)
            
            # backend
            self.handleVectorMem()
            self.handleFuncUnits()
            self.handleScalar()

            # update states here
            self.s_remaining = max(self.s_remaining - 1, 0)
            if self.s_instr and self.s_remaining == 0:
                self.busyboard.clear(self.s_instr)
                self.s_instr = None
        
            self.vdata.update()
            if self.vdata.instr and not self.vdata.busy():
                self.busyboard.clear(self.vdata.instr)
                self.vdata.instr = None
            
            for func in self.units:
                self.units[func].update()
                
                if self.units[func].instr and not self.units[func].busy():
                    self.busyboard.clear(self.units[func].instr)
                    self.units[func].instr = None
            
            # Frontend
            # --------------
            # decode stage
            if not self.stallDecode:
                # print("DS:", self.instrBuf) # DEBUG         
                self.decode(self.instrBuf)
                
                if self.instrBuf == "HALT":
                    self.stallDecode = True

            # fetch stage
            if not self.stallFetch:
                self.instrBuf = self.fetch()
                
                # print("IF:", self.instrBuf) # DEBUG
                
                if self.instrBuf == "HALT":
                    # stall fetch in the next cycle
                    self.stallFetch = True
                else:
                    # decode the instr in next cycle
                    # this is for when fetching first instr
                    self.stallDecode = False
                    
            elif self.instrBuf != "HALT":
                # if stalled in cur cycle, don't stall next cycle, but only if "HALT" isn't already reached
                # this allows us to stall fetch, until decode stage doesn't stall it
                self.stallFetch = False

            # print("Scalar Q:", self.scalarQ.q) # DEBUG
            # print("VecCom Q:", self.vectorComputeQ.q) # DEBUG
            # print("VecDat Q:", self.vectorDataQ.q) # DEBUG
            # print("BusyBoard", self.busyboard) # DEBUG
            # print() # DEBUG
            
        return self.cycle

    def cyclesTaken(self):
        return self.cycle

if __name__ == "__main__":
     #parse arguments for input file location
    parser = argparse.ArgumentParser(description='Vector Processor Timing Simulator')
    parser.add_argument('--iodir', default="", type=str, help='Path to the folder containing the input files - instructions and data.')
    args = parser.parse_args()

    iodir = os.path.abspath(args.iodir)
    print("IO Directory:", iodir)

    # Parse Config
    config = Config(iodir)
    
    print("Running func simulator...")
    os.system(f'python3 funcsimulator.py -t --iodir "{iodir}" > out.txt')
    print("Saved output of func simulator in out.txt")

    tracefp = os.path.abspath(os.path.join(iodir, "trace.asm"))
    ts = TimingSim(tracefp, config)

    print("Running timing simulator...")
    ts.run()
    print(f"Cycles: {ts.cyclesTaken()}")
