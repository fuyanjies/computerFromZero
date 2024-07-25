@sum
M = 0

(LOOP)
@R1
D = M
@END
D;JEQ
@R1
M = M - 1


@R0
D = M
@sum
M = M + D

@LOOP
0;JMP

(END)

@sum
D = M
@R2
M = D