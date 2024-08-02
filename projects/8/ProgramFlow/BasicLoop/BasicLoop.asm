@256
D = A
@SP
M = D
@0
D = A
@SP
M = M + 1
A = M - 1
M = D
@LCL
D = M
@0
D = D + A
@R13
M = D
@SP
M = M - 1
A = M
D = M
@R13
A = M
M = D
($LOOP)
@ARG
D = M
@0
A = D + A
D = M
@SP
M = M + 1
A = M - 1
M = D
@LCL
D = M
@0
A = D + A
D = M
@SP
M = M + 1
A = M - 1
M = D
@SP
M = M - 1
A = M
D = M
@SP
A = M - 1
M = D + M
@LCL
D = M
@0
D = D + A
@R13
M = D
@SP
M = M - 1
A = M
D = M
@R13
A = M
M = D
@ARG
D = M
@0
A = D + A
D = M
@SP
M = M + 1
A = M - 1
M = D
@1
D = A
@SP
M = M + 1
A = M - 1
M = D
@SP
M = M - 1
A = M
D = M
@SP
A = M - 1
M = M - D
@ARG
D = M
@0
D = D + A
@R13
M = D
@SP
M = M - 1
A = M
D = M
@R13
A = M
M = D
@ARG
D = M
@0
A = D + A
D = M
@SP
M = M + 1
A = M - 1
M = D
@SP
M = M - 1
A = M
D = M
@$LOOP
D; JNE
@LCL
D = M
@0
A = D + A
D = M
@SP
M = M + 1
A = M - 1
M = D
