# dot product on 450 and 450 len arrays
# save output in addr 2048 in vdm

# load constants
LS SR1 SR0 0 # SR1 = 2
LS SR4 SR0 1 # SR4 = 450

# Initial 2 values since 450 % 64 != 0
MTCL SR1 # Vector Len = 2
LV VR1 SR0
LV VR2 SR4
MULVV VR3 VR1 VR2 # VR3 = [VR1[0]*VR2[0], VR1[1]*VR2[1], 0, ...]


POP SR2 # SR2 = 64
MTCL SR2 # Vector Len = 64
ADD SR4 SR4 SR1 # SR4 = 452
LS SR3 SR0 1 # SR3 = 450

# loop start
LV VR1 SR1 # SR1 = 2, so starting from address 2 to 65
LV VR2 SR4
MULVV VR4 VR1 VR2
ADDVV VR3 VR3 VR4 # keep adding to VR3, which is the result register
ADD SR1 SR1 SR2 # SR1 += 64
ADD SR4 SR4 SR2 # SR4 += 64
BNE SR1 SR3 -6 # loop end

LS SR5 SR0 2
LS SR1 SR0 3

# loop start - shuffling to accumulate VR3
PACKLO VR5 VR3 VR0 # I use shuffling to accumulate add the last vector
PACKHI VR6 VR3 VR0 # idea is to keep splitting the vector in half, and adding each half
ADDVV VR3 VR5 VR6  # this also makes it pretty optimized
ADD SR6 SR6 SR1 
BNE SR6 SR5 -4 # loop end

LS SR1 SR0 4 # SR1 = 2048
SV VR3 SR1
HALT