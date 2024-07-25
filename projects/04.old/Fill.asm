@16384
D = A
@pos
M = D
@i
M = 0

(LOOP)
    @KBD
    D = M
    @CLR
    D; JEQ
    @FILL
    D; JGT

(CLR)
    @pos
    D = M
    @16384
    D = D - A
    @LOOP
    D; JEQ
    (CLR_POINT)
    @pos
    D = M
    A = D
    M = 0
    @pos
    M = M - 1
    @i
    MD = M + 1
    @32
    D = D - A
    @CLR_POINT
    D; JLT
    @i
    M = 0
    @LOOP
    0; JMP

(FILL)
    @pos
    D = M
    @26575
    D = D - A
    @LOOP
    D; JEQ
    (FILL_POINT)
    @pos
    D = M
    A = D
    M = -1
    @pos
    M = M + 1
    @i
    MD = M + 1
    @32
    D = D - A
    @FILL_POINT
    D; JLT
    @i
    M = 0
    @LOOP
    0; JMP