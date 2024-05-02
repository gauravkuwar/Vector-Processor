# Fully Connected Layer
LS SR1 SR0 0 # SR1 = 1
POP SR7      # SR7 = 64
LS SR4 SR0 3 # SR4 = 1024; W mem address
LS SR5 SR0 5 # SR5 = 256

# Multiply a row in W by a
ADDVV VR1 VR0 VR0 # clear VR1
LS SR3 SR0 4 # SR3 = 512; a mem address
LS SR2 SR0 1 # SR2 = 4

LV VR2 SR3   # load a
LV VR3 SR4   # load W
MULVV VR3 VR2 VR3   # a * W
ADDVV VR1 VR1 VR3   # VR1 += a * W
ADD SR3 SR3 SR7     # SR3 += 64
ADD SR4 SR4 SR7     # SR4 += 64
SUB SR2 SR2 SR1     # SR2 -= 1
BGE SR2 SR0 -7      # until SR2 >= 0

# Accumulate
LS SR2 SR0 2 # SR2 = 7

PACKHI VR4 VR1 VR0  
PACKLO VR1 VR1 VR0
ADDVV VR1 VR1 VR4
SUB SR2 SR2 SR1 # SR2 -= 1
BGE SR2 SR0 -4  # until SR2 >= 0

SV VR1 SR6  # SR6 is result address
ADD SR6 SR6 SR1 # SR6 += 1
BLT SR6 SR5 -19 # until SR6 < 256

# adding bias
ADD SR3 SR0 SR0 # SR3 = 0
LS SR2 SR0 1 # SR2 = 4
LS SR4 SR0 6 # SR4 = 768 ; b mem address

LV VR1 SR3  # load a*W result
LV VR2 SR4  # load bias
ADDVV VR1 VR1 VR2 # add bias
SV VR1 SR3  # store bias
ADD SR3 SR3 SR7 # SR3 += 64
ADD SR4 SR4 SR7 # SR4 += 64
SUB SR2 SR2 SR1     # SR2 -= 1
BGE SR2 SR0 -7      # until SR2 >= 0
HALT