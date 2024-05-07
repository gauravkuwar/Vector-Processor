"""
VMIPS Vector Processor Simulator
Authors: Gaurav Kuwar, Ritvik Nair
"""

import os
import argparse
from verify_program import verifyCode, rmComments, allVecInstrs

REG_COUNT = 8
REG_BITS = 32
MAX_VECTOR_LEN = 64

# hashmap for operation functions
opFunc = {
    "ADD": lambda x, y: x + y,
    "SUB": lambda x, y: x - y,
    "DIV": lambda x, y: x // y,
    "MUL": lambda x, y: x * y,
    "AND": lambda x, y: x & y,
    "OR":  lambda x, y: x | y,
    "XOR": lambda x, y: x ^ y,
    "SLL": lambda x, y: x << y, # Logical Left Shift
    "SRL": lambda x, y: x >> y, # Logical Right Shift
    "SRA": lambda x, y: x >> y, # FIXED: Arithmetic Right Shift
    "EQ": lambda x, y: int(x == y),
    "NE": lambda x, y: int(x != y),
    "GT": lambda x, y: int(x > y),
    "LT": lambda x, y: int(x < y),
    "GE": lambda x, y: int(x >= y),
    "LE": lambda x, y: int(x <= y)
}

boolIntrs = {'EQ', 'NE', 'GT', 'LT', 'GE', 'LE'}
def padding(a, N=4): # just a helper function to split instructions
    return (a + N * [None])[:N]

class IMEM(object):
    def __init__(self, iodir):
        self.size = pow(2, 16) # Can hold a maximum of 2^16 instructions.
        self.filepath = os.path.abspath(os.path.join(iodir, "Code.asm"))
        self.instructions = []

        try:
            with open(self.filepath, 'r') as insf:
                self.instructions = [ins.strip() for ins in insf.readlines()]
            print("IMEM - Instructions loaded from file:", self.filepath)
            # print("IMEM - Instructions:", self.instructions)
        except:
            print("IMEM - ERROR: Couldn't open file in path:", self.filepath)

        # Remove comments
        self.instructions = rmComments(self.instructions)

    def Read(self, idx): # Use this to read from IMEM.
        if idx < self.size:
            return self.instructions[idx]
        else:
            print("IMEM - ERROR: Invalid memory access at index: ", idx, " with memory size: ", self.size)

class DMEM(object):
    # Word addressible - each address contains 32 bits.
    def __init__(self, name, iodir, addressLen):
        self.name = name
        self.size = pow(2, addressLen)
        self.min_value  = -pow(2, 31)
        self.max_value  = pow(2, 31) - 1
        self.ipfilepath = os.path.abspath(os.path.join(iodir, name + ".txt"))
        self.opfilepath = os.path.abspath(os.path.join(iodir, name + "OP.txt"))
        self.data = []

        try:
            with open(self.ipfilepath, 'r') as ipf:
                self.data = [int(line.strip()) for line in ipf.readlines()]
            print(self.name, "- Data loaded from file:", self.ipfilepath)
            # print(self.name, "- Data:", self.data)
            self.data.extend([0x0 for i in range(self.size - len(self.data))])
        except:
            print(self.name, "- ERROR: Couldn't open input file in path:", self.ipfilepath)

    def Read(self, idx): # Use this to read from DMEM.
        return self.data[idx]

    def Write(self, idx, val): # Use this to write into DMEM.
        self.data[idx] = val

    def dump(self):
        try:
            with open(self.opfilepath, 'w') as opf:
                lines = [str(data) + '\n' for data in self.data]
                opf.writelines(lines)
            print(self.name, "- Dumped data into output file in path:", self.opfilepath)
        except:
            print(self.name, "- ERROR: Couldn't open output file in path:", self.opfilepath)

class RegisterFile(object):
    def __init__(self, name, count, length = 1, size = 32):
        self.name       = name
        self.reg_count  = count
        self.vec_length = length # Number of 32 bit words in a register.
        self.reg_bits   = size
        self.min_value  = -pow(2, self.reg_bits-1)
        self.max_value  = pow(2, self.reg_bits-1) - 1
        self.registers  = [[0x0 for e in range(self.vec_length)] for r in range(self.reg_count)] # list of lists of integers

    def Read(self, idx):
        idx = int(idx[2:])
        return self.registers[idx] if self.vec_length > 1 else self.registers[idx][0]

    def Write(self, idx, val):
        idx = int(idx[2:])
        if idx == 0: return
        
        # this lets us easily write scalar values
        if type(val) != list:
            self.registers[idx][0] = val
        else: 
            # this can be used for both scalar values and vectors
            # and it takes into account the vector length
            # but assumes len(val) < MVL
            for i, elem in enumerate(val):
                # If the value is None that means vector mask for that idx
                # was 0, so that value is not being changed
                if elem != None:
                    self.registers[idx][i] = elem

    def dump(self, iodir):
        opfilepath = os.path.abspath(os.path.join(iodir, self.name + ".txt"))
        try:
            with open(opfilepath, 'w') as opf:
                row_format = "{:<13}"*self.vec_length
                lines = [row_format.format(*[str(i) for i in range(self.vec_length)]) + "\n", '-'*(self.vec_length*13) + "\n"]
                lines += [row_format.format(*[str(val) for val in data]) + "\n" for data in self.registers]
                opf.writelines(lines)
            print(self.name, "- Dumped data into output file in path:", opfilepath)
        except:
            print(self.name, "- ERROR: Couldn't open output file in path:", opfilepath)

class VectorMaskRegister():
    def __init__(self, mvl):
        self.mvl = 0
        self.mask = [1] * mvl

    def apply(self, vec): # edits a vec based on mask
        # None makes the values in the actual register which are not
        # affected by the mask are not changed in the vector register
        res = [None] * len(vec) 
        for i in range(len(vec)):
            if self.mask[i]:
                res[i] = vec[i]
        return res

    def write(self, vec): # writes to the mask itself
        for i in range(len(vec)):
            self.mask[i] = vec[i]

    def clear(self): # for CVM instr
        self.mask = [1] * len(self.mask)

    def count1s(self): # for POP instr
        return sum(self.mask)
    
class Trace:
    # get dynamic flow trace
    def __init__(self):
        self.lines = []

    def append(self, line, vlen=None):
        if vlen: line += f" {vlen}"
        self.lines.append(line)

    def update(self, line, vlen=None):
        if not self.lines: raise Exception("TraceError: no lines to update")
        if vlen: line += f" {vlen}"
        self.lines[-1] = line

    def save(self, outfile):
        with open(outfile, 'w') as f:
            f.write('\n'.join(self.lines))

        print("Saved dynamic flow trace")

class Core():
    def __init__(self, imem, sdmem, vdmem):
        self.IMEM = imem
        self.SDMEM = sdmem
        self.VDMEM = vdmem

        self.RFs = {"SRF": RegisterFile("SRF", REG_COUNT),
                    "VRF": RegisterFile("VRF", REG_COUNT, MAX_VECTOR_LEN)}
        
        # Your code here.
        self.vLen = MAX_VECTOR_LEN
        self.vMask = VectorMaskRegister(MAX_VECTOR_LEN) # vector mask register
        self.trace = Trace() # trace code

    def aluOp(self, operator, op1, op2, op3):
        if op2.startswith("SR") and op3.startswith("SR"):
            res = opFunc[operator](self.RFs['SRF'].Read(op2), self.RFs['SRF'].Read(op3))
            self.RFs['SRF'].Write(op1, res)
        else:
            op2Val = self.RFs['VRF'].Read(op2)
            op3Val = self.RFs['VRF'].Read(op3) if op3.startswith("VR") else [self.RFs['SRF'].Read(op3)] * MAX_VECTOR_LEN
            res = [0] * self.vLen
            
            for i in range(self.vLen):
                res[i] = opFunc[operator](op2Val[i], op3Val[i])
            
            if operator in boolIntrs: # for S__VV and S__VS instructions
                self.vMask.write(res)
            else:
                self.RFs['VRF'].Write(op1, self.vMask.apply(res))

    def unpack(self, VR1, VR2, VR3, N, isLo=False):
        offset = (N // 2) if isLo else 0 
        vr1Tmp = [0] * len(VR1)
        for i in range(0, N, 2):
            j = offset + i // 2
            vr1Tmp[i], vr1Tmp[i+1] = VR2[j], VR3[j]

        for i in range(len(VR1)):
            VR1[i] = vr1Tmp[i] # make sure we change the actual register values
            
    def pack(self, VR1, VR2, VR3, N, isOdd=False):
        vr1Tmp = [0] * len(VR1)
        for j, i in enumerate(range(0, N, 2)):
            vr1Tmp[j], vr1Tmp[j + N//2] = VR2[i + isOdd], VR3[i + isOdd]

        for i in range(len(VR1)):
            VR1[i] = vr1Tmp[i] # make sure we change the actual register values

    def lv(self, op1, op2, op3): # for LV and LVWS
        address = self.RFs['SRF'].Read(op2)
        stride = self.RFs['SRF'].Read(op3) if op3 is not None else 1
        res = []

        for i in range(self.vLen):
            res.append(self.VDMEM.Read(address + (stride * i)))

        # FIXED: applied mask
        self.RFs['VRF'].Write(op1, self.vMask.apply(res))

        instrType = "LVWS" if op3 else "LV"
        self.trace.update(f"{instrType} {op1} {tuple(address + (stride * i) for i in range(self.vLen))}", self.vLen) # trace

    def sv(self, op1, op2, op3): # for SV and SVWS
        address = self.RFs['SRF'].Read(op2)
        stride = self.RFs['SRF'].Read(op3) if op3 is not None else 1
        vec = self.vMask.apply(self.RFs['VRF'].Read(op1)) # FIXED: applied mask
        
        for i in range(self.vLen):
            self.VDMEM.Write(address + (stride * i), vec[i])
        
        instrType = "SVWS" if op3 else "SV"
        self.trace.update(f"{instrType} {op1} {tuple(address + (stride * i) for i in range(self.vLen))}", self.vLen) # trace

    def run(self):
        PC = 0 # program counter

        while self.IMEM.Read(PC) != "HALT":
            instr = self.IMEM.Read(PC)
            print(PC, ":", instr)
            instrType, op1, op2, op3 = padding(instr.split())

            if instrType in allVecInstrs:
                self.trace.append(instr, self.vLen) # add vector len to trace
            else:
                self.trace.append(instr)
            
            if instrType == "CVM": # clear vector mask register - set to all 1's
                self.vMask.clear()

            if instrType == "POP": # store # of 1s in vector mask register
                self.RFs['SRF'].Write(op1, self.vMask.count1s())

            if instrType == "MTCL": # Move the contents of the Scalar Register SR1 into the Vector Length Register
                self.vLen = self.RFs['SRF'].Read(op1)

            if instrType == "MFCL": # Move the contents of the Vector Length Register into the Scalar Register SR1
                self.RFs['SRF'].Write(op1, self.vLen)
            
            # ALU operations
            for operator in ("ADD", "SUB", "DIV", "MUL", 
                             "AND", "OR", "XOR", 
                             "SLL", "SLA", "SRA"):
                if operator in instrType:
                    self.aluOp(operator, op1, op2, op3)

            # S__VV and S__VS instructions
            if instrType.startswith("S") and instrType[1:3] in boolIntrs:
                self.aluOp(instrType[1:3], op3, op1, op2)


            # Load and Store instructions
            if instrType in {"LV", "LVWS"}:
                self.lv(op1, op2, op3)
            
            if instrType in {"SV", "SVWS"}:
                self.sv(op1, op2, op3)

            # Load/Store Scatter-Gather 
            if instrType == "LVI":
                address = self.RFs['SRF'].Read(op2)
                vec2 = self.RFs['VRF'].Read(op3)
                res = []

                for i in range(self.vLen):
                    res.append(self.VDMEM.Read(address + vec2[i]))

                self.RFs['VRF'].Write(op1, res)
                self.trace.update(f"LVI {op1} {tuple(address + vec2[i] for i in range(self.vLen))}", self.vLen) # trace

            if instrType == "SVI":
                address = self.RFs['SRF'].Read(op2)
                vec = self.RFs['VRF'].Read(op1)
                vec2 = self.RFs['VRF'].Read(op3)

                for i in range(self.vLen):
                    self.VDMEM.Write(address + vec2[i], vec[i])

                self.trace.update(f"SVI {op1} {tuple(address + vec2[i] for i in range(self.vLen))}", self.vLen) # trace

            # Load/Store Scalar
            if instrType == "LS":
                # load value in memory location Reg[op2] + op3 into register op1
                self.RFs['SRF'].Write(op1, self.SDMEM.Read(self.RFs['SRF'].Read(op2) + int(op3)))
                self.trace.update(f"LS {op1} ({self.RFs['SRF'].Read(op2) + int(op3)})") # trace
                
            if instrType == "SS":
                # store in memory location Reg[op2] + op3 the value Reg[op1] 
                self.SDMEM.Write(self.RFs['SRF'].Read(op2) + int(op3), self.RFs['SRF'].Read(op1))
                self.trace.update(f"SS {op1} ({self.RFs['SRF'].Read(op2) + int(op3)})") # trace


            # Register-Register Shuffle
            # 2 functions for pack and unpack
            if "PACK" in instrType:
                VR1, VR2, VR3 = self.RFs['VRF'].Read(op1), self.RFs['VRF'].Read(op2), self.RFs['VRF'].Read(op3)
                if instrType == "UNPACKLO":
                    self.unpack(VR1, VR2, VR3, self.vLen)
                
                if instrType == "UNPACKHI":
                    self.unpack(VR1, VR2, VR3, self.vLen, isLo=True)

                if instrType == "PACKLO":
                    self.pack(VR1, VR2, VR3, self.vLen)

                if instrType == "PACKHI":
                    self.pack(VR1, VR2, VR3, self.vLen, isOdd=True)  
            

            # Branch operations (Scalar)
            # Note: when incrementing PC, blank lines or lines with only comments are ignored 

            branch_taken = instrType.startswith("B") and opFunc[instrType[1:]](self.RFs['SRF'].Read(op1), self.RFs['SRF'].Read(op2))
            branch_not_taken = instrType.startswith("B") and not opFunc[instrType[1:]](self.RFs['SRF'].Read(op1), self.RFs['SRF'].Read(op2))

            if branch_taken:
                self.trace.update(f"B ({PC + int(op3)})") # trace
                PC += int(op3)
            else:
                if branch_not_taken:
                    self.trace.update(f"B ({PC + 1})") # trace
                PC += 1

            print("SRF:", ', '.join(f'SR{i}: {x[0]}' for i, x in enumerate(self.RFs['SRF'].registers)))
            print("VRF:")
            for i, v in enumerate(self.RFs['VRF'].registers):
                print(f'VR{i}:', v)
            print("V Mask:\n", self.vMask.mask)
            print('-' * 10, '\n')

        self.trace.append("HALT")
        print()


    def dumpregs(self, iodir):
        for rf in self.RFs.values():
            rf.dump(iodir)


if __name__ == "__main__":
    # parse arguments for input file location
    parser = argparse.ArgumentParser(description='Vector Core Performance Model')
    parser.add_argument('--iodir', default="", type=str, help='Path to the folder containing the input files - instructions and data.')
    parser.add_argument('-t', '--trace', default=False, action='store_true', help="save trace of dynamic flow for timing simulator")
    args = parser.parse_args()

    iodir = os.path.abspath(args.iodir)
    print("IO Directory:", iodir)

    # verify Code.asm
    # idea is to check syntax before running code (similar to Python)
    verifyCode(os.path.abspath(os.path.join(iodir, "Code.asm")))

    # Parse IMEM
    imem = IMEM(iodir)  
    # Parse SMEM
    sdmem = DMEM("SDMEM", iodir, 13) # 32 KB is 2^15 bytes = 2^13 K 32-bit words.
    # Parse VMEM
    vdmem = DMEM("VDMEM", iodir, 17) # 512 KB is 2^19 bytes = 2^17 K 32-bit words. 

    # Create Vector Core
    vcore = Core(imem, sdmem, vdmem)

    # Run Core
    vcore.run()   
    vcore.dumpregs(iodir)

    sdmem.dump()
    vdmem.dump()

    if args.trace:
        vcore.trace.save(os.path.abspath(os.path.join(iodir, "trace.asm"))) # save trace
    # THE END