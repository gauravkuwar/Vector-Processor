# Code for Convolution Layer of 256x256 matrix and 3x3 kernel with stride 2,
# and zero padding, meaning padding of 1 row and 1 col on right and bottom of matrix.
# the output matrix is 128x128

# first 9 addresses in SDMEM is the kernel flattened.
# first 256x256 values in VDMEM is the matrix f flattened.

LS SR1 SR0 10       # SR1 = 1 ; incr num
LS SR2 SR0 9        # SR2 = 2 ; stride
LS SR3 SR0 11       # SR3 = 3 

# loop1
AND SR5 SR5 SR0     # SR5 = base = 0
# loop2
AND SR6 SR6 SR0     # SR6 = index = 0
# loop3
LS SR4 SR0 12       # get vector address from mem
ADD SR4 SR4 SR6     # add index to vector address
LVWS VR1 SR4 SR2    # load into vector reg

BNE SR6 SR2 3       # make vector len 63 for last col padding
LS SR7 SR0 14       # SR7 = 63
MTCL SR7            # make vector len 63

LS SR7 SR0 16      # SR7 = 128
ADD SR4 SR4 SR7     # vector address += 128
LVWS VR2 SR4 SR2    # load into vector reg

# get kernel value
# kernel index = base * 3 + index
# base * 3 with ADDs
ADD SR4 SR0 SR5     # SR4 += base
ADD SR4 SR4 SR5     # SR4 += base
ADD SR4 SR4 SR5     # SR4 += base
ADD SR4 SR4 SR6     # resulting in base * 3 + index
LS SR4 SR4 0        # load kernel value from kernel index

# perform multiply by kernel value and add on each half of the row
MULVS VR1 VR1 SR4
MULVS VR2 VR2 SR4
ADDVV VR6 VR6 VR1
ADDVV VR7 VR7 VR2

LS SR7 SR0 15       # SR7 = 64
MTCL SR7            # make sure vector len is 64

ADD SR6 SR6 SR1     # increment index by 1
BLT SR6 SR3 -21     # loop3 end : loop until base < 3

ADD SR5 SR5 SR1     # increment base by 1
BEQ SR5 SR3 5       # skip changing for last iter

LS SR4 SR0 12       # get vector address from mem
LS SR7 SR0 17       # SR7 = 256
ADD SR4 SR4 SR7     # vector address += 256
SS SR4 SR0 12       # store vector address into mem

BLT SR5 SR3 -29     # loop2 end : loop until base < 3

LS SR5 SR0 18       # get output mem address
SV VR6 SR5
MFCL SR6            # SR6 = 64
ADD SR5 SR5 SR6     # SR5 += 64
SV VR7 SR5
ADD SR5 SR5 SR6     # SR5 += 64

ADDVV VR6 VR0 VR0
ADDVV VR7 VR0 VR0

SS SR5 SR0 18       # store next output mem address
LS SR6 SR0 19       # get max mem address
BLT SR5 SR6 -41     # loop1 end : loop until output mem address < 16384
HALT
