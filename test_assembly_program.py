"""
script to do test on assembly code using random inputs
for dot product, convolution, fully connected layer (matrix multiplication)
"""
import numpy as np
import os

# random int range
MIN_VAL = -128
MAX_VAL = 128

# fcl dims
FCL_A_SHAPE = (256,)
FCL_W_SHAPE = (256, 256)
FCL_B_SHAPE = (256,)

# conv dims
CONV_F_SHAPE = (256, 256)
CONV_G_SHAPE = (3, 3) # kernel

# dot product dims
DP_A_SHAPE = (450)
DP_B_SHAPE = (450) # kernel

# generate random data funcs
def generate_fcl_data():
    # fully connectted layer
    a = np.random.randint(MIN_VAL, MAX_VAL, FCL_A_SHAPE)
    W = np.random.randint(MIN_VAL, MAX_VAL, FCL_W_SHAPE)
    b = np.random.randint(MIN_VAL, MAX_VAL, FCL_B_SHAPE)    
    return a, W, b

def generate_conv_data():
    # convolution
    f = np.random.randint(MIN_VAL, MAX_VAL, CONV_F_SHAPE)
    g = np.random.randint(MIN_VAL, MAX_VAL, CONV_G_SHAPE)
    return f, g

def generate_dp_data():
    # dot product
    a = np.random.randint(MIN_VAL, MAX_VAL, DP_A_SHAPE)
    b = np.random.randint(MIN_VAL, MAX_VAL, DP_B_SHAPE)
    return a, b

# convolution func for 256x256 input and 3x3 kernel
def conv2d(f, g, stride=2, padding=1):
    fwp_shape = (CONV_F_SHAPE[0]+padding, CONV_F_SHAPE[0]+padding)
    out_shape = (128, 128)

    fwp = np.zeros(fwp_shape)
    fwp[:-1, :-1] = f # f with padding
    res = np.zeros(out_shape)

    rows, cols = f.shape
    grows, gcols = g.shape

    for i in range(0, rows, stride):
        for j in range(0, cols, stride):
            res[i//2][j//2] = np.dot(fwp[i:i+grows,j:j+gcols].flatten(), g.flatten())

    return res

# import scipy
# def strideConv(f,arr2,s=2, p=1):
#     arr = np.zeros((256+p, 256+p))
#     arr[:-1, :-1] = f # f with padding
#     cc=scipy.signal.fftconvolve(arr,arr2[::-1,::-1],mode='valid')
#     idx=(np.arange(0,cc.shape[1],s), np.arange(0,cc.shape[0],s))
#     xidx,yidx=np.meshgrid(*idx)
#     return np.round(cc[yidx,xidx])

"""
make arg --iodir for consistency

SDMEM changes (only conv changes this)
VDMEM changes

Update both SDMEM and VDMEM

Note: result always in VDMEM
extract as array
Dot product
    - result in 2048

FCL
    - result in 0 - 256

Conv
    - result in 0 - (128*128)

FFT (WIP)

"""

def run(dir, tests=1):
    match = 0

    for test in range(1, tests+1):

        if dir.startswith("fully_connected_layer"):
            a, W, b = generate_fcl_data()
            # corr = np.dot(a, W) + b # correct
            corr = np.dot(W, a) + b
            vmemdata = np.concatenate((np.zeros(512), a.flatten(), b.flatten(), W.flatten())).astype(int)

        elif dir.startswith("convolution_layer"):
            a, g = generate_conv_data()
            corr = conv2d(a, g).flatten()

            # read sdmem vars
            with open(f'{dir}/SDMEM.txt') as f:
                other_vars = f.read().splitlines()[9:]

            # replace first 9 addr with kernel (g)
            with open(f'{dir}/SDMEM.txt', 'w') as f:
                f.write('\n'.join(map(str, list(g.flatten()) + other_vars)))
            
            vmemdata = np.concatenate((np.zeros(128*128), a.flatten())).astype(int)

        elif dir.startswith("dot_product"):
            a, b = generate_dp_data()
            corr = np.dot(a, b)
            vmemdata = np.concatenate((a, b))


        # store random data
        with open(f'{dir}/VDMEM.txt', 'w') as f:
            f.write('\n'.join(map(str, vmemdata)))

        # run func sim
        os.system(f'python3 funcsimulator.py --iodir {dir}/ > .{dir}.out')

        if dir.startswith("fully_connected_layer"):
            with open(f'{dir}/VDMEMOP.txt') as f:
                outres = np.array(list(map(int, f.read().splitlines()[:256])))
            matched = all(outres == corr)

        elif dir.startswith("convolution_layer"):
            with open(f'{dir}/VDMEMOP.txt') as f:
                outres = np.array(list(map(int, f.read().splitlines()[:128*128])))

            # checks = outres == corr
            # for i in range(len(checks)):
            #     if not checks[i]:
            #         print(i, outres[i], corr[i], corrm[i])
            matched = all(outres == corr)

        elif dir.startswith("dot_product"):
            with open(f'{dir}/VDMEMOP.txt') as f:
                outres = int(f.read().splitlines()[2048])
            matched = outres == corr

        # print(outres, corr)

        # check
        if matched:
            print(f"TESTCASE {test} FOR {dir} PASSED!")
            match += 1
        else:
            raise Exception(f"TESTCASE {test} FOR {dir} FAILED!")

    print(f"{match}/{tests} TESTCASES FOR {dir} PASSED!")


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(
                    prog='test_asm',
                    description='test assembly code')
    
    parser.add_argument('-a', '--asm', help='assembly code dir', type=str)
    parser.add_argument('-n', '--num', help='num of tests', type=int, default=1)
    args = parser.parse_args()
    
    if args.asm == "all":
        for dir in ("fully_connected_layer", "dot_product", "convolution_layer"):
            print(f"Starting {dir} tests ...")
            run(dir, tests=args.num)
    else:
        run(args.asm, tests=args.num)
