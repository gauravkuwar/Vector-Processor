"""
This script checks if mostly if the syntax of the code is correct.
A lot similar to how python checks syntax.
More specifically it checks:

- if HALT exists
- Correct num of operands
- Instruction in set of all instructions
- vector number {0, 7}, same with scalar
- if vector operand is in the correct spot etc.
- check Imm is int

"""
import argparse
import os

TOTAL_VEC_REGS = 8
TOTAL_SCALAR_REGS = 8

# set of all instructions
allIntrs = {'ADDVS', 'ADD', 'SEQVV', 'SUBVV', 'AND', 'ADDVV', 'SLTVS', 'SVI', 
            'BEQ', 'OR', 'BLT', 'MTCL', 'BGT', 'SLL', 'SRL', 'LV', 'SLEVS', 'LS', 
            'CVM', 'SLEVV', 'PACKLO', 'SNEVS', 'UNPACKHI', 'SNEVV', 'POP', 'DIVVV', 
            'LVWS', 'SS', 'PACKHI', 'SLTVV', 'LVI', 'BNE', 'UNPACKLO', 'SUBVS', 
            'SRA', 'DIVVS', 'MULVV', 'SUB', 'BGE', 'XOR', 'SV', 'MULVS', 'SGTVV', 
            'SGEVS', 'SGTVS', 'SGEVV', 'BLE', 'SVWS', 'SEQVS', 'MFCL', 'HALT'}

# number of operands each instruction has
numOfOp = {
            0: {'CVM', 'HALT'}, 
            1: {'POP', 'MTCL', 'MFCL'},
            2: {'SEQVV', 'SNEVV', 'SGTVV', 'SLTVV', 'SGEVV', 'SLEVV', 
                'SEQVS', 'SNEVS', 'SGTVS', 'SLTVS', 'SGEVS', 'SLEVS',
                'LV', 'SV'},
            # 3 is rest
}

opCombos = {
    ('v', 'v', 'v') : {'ADDVV', 'SUBVV', 'MULVV', 'DIVVV', 'UNPACKHI', 'UNPACKLO', 'PACKLO', 'PACKHI'},
    ('v', 'v', 's') : {'ADDVS', 'SUBVS', 'MULVS', 'DIVVS'},
    ('v', 'v', 'n') : {'SEQVV', 'SNEVV', 'SGTVV', 'SLTVV', 'SGEVV', 'SLEVV'},
    ('v', 's', 'n') : {'SEQVS', 'SNEVS', 'SGTVS', 'SLTVS', 'SGEVS', 'SLEVS', 'LV', 'SV'},
    ('s', 'n', 'n') : {'POP', 'MTCL', 'MFCL'},
    ('v', 's', 's') : {'LVWS', 'SVWS'},
    ('v', 's', 'v') : {'LVI', 'SVI'},
    ('s', 's', 'i') : {'LS', 'SS', 'BEQ', 'BNE', 'BGT', 'BLT', 'BGE', 'BLE'},
    ('s', 's', 's') : {'ADD', 'SUB', 'MUL', 'DIV', 'AND', 'OR', 'XOR', 'SLL', 'SRL', 'SRA'},
    ('n', 'n', 'n') : {'CVM', 'HALT'}
}

allVecInstrs = set()
for k in opCombos:
    if 'v' in k:
        allVecInstrs ^= opCombos[k]

def padding(a, N=4): # just a helper function to split instructions
    return (a + N * [None])[:N]

# Function to remove comments from Code.asm file
def rmComments(allInstrs, withLineNum=False):
    res = []
    for i, instr in enumerate(allInstrs):
        instr = instr.strip()
        newinstr = instr.split("#")[0].strip() # remove inline comments
        if newinstr != '':
            # remove blank lines or comment only lines
            if withLineNum: res.append((i+1, newinstr))
            else: res.append(newinstr)
    return res

def getOpType(op, lineNum):
    # get is the operand is a vector register, scalar register or imm
    # s is scalar, v is vector, i is imm, and n is None
    
    if op is None: return "n"
    if op.startswith("SR") and op[2:].isdigit(): 
        if not (0 <= int(op[2:]) < TOTAL_SCALAR_REGS):
            raise Exception(f"Line {lineNum}: Register number out of bound")
        return "s"
    if op.startswith("VR") and op[2:].isdigit(): 
        if not (0 <= int(op[2:]) < TOTAL_VEC_REGS):
            raise Exception(f"Line {lineNum}: Register number out of bound")
        return "v"
    if op.isdigit() or (op[0] == '-' and op[1:].isdigit()): 
        return 'i'
    
    raise Exception(f"Line {lineNum}: Invalid operand {op}")

def verifyCode(code_file):
    with open(code_file) as f:
        lines = rmComments(f.read().splitlines(), withLineNum=True) # remove comments
        if 'HALT' not in map(lambda x: x[1], lines):
            # Here I don't check if HALT is in the end
            # because there can be a program where HALT is not at the end of the file
            # since we can branch to the HALT
            raise Exception(f"Program doesn't HALT")
        
        for lineNum, line in lines:
            instrType, op1, op2, op3 = padding(line.split())
            opType = getOpType(op1, lineNum), getOpType(op2, lineNum), getOpType(op3, lineNum)
            num_of_op = len(line.split()) - 1
            
            # if valid instr
            if instrType not in allIntrs:
                raise Exception(f"Line {lineNum}: Invalid Instruction {instrType}")
            
            # Num of operands
            if num_of_op > 3 or (num_of_op < 3 and instrType not in numOfOp[num_of_op]):
                raise Exception(f"Line {lineNum}: Incorrect number of operands for {instrType}")
            
            # Check different combinations of operands like: ('v', 'v', 'v') is all vector
            # make sure the combination exists in opCombos, and its for the correct instr
            if opType not in opCombos or instrType not in opCombos[opType]:
                raise Exception(f"Line {lineNum}: Invalid operand type")
    
    print("Verification successful.")

if __name__ == "__main__":
    # parse arguments for code file location
    parser = argparse.ArgumentParser(description='Verify Assembly program for correctness')
    parser.add_argument('--codefile', default="", type=str, help='Path to the asm code file to verfiy')
    args = parser.parse_args()

    codefile = os.path.abspath(args.codefile)
    verifyCode(codefile)