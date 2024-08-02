
(SimpleFunction.test)
@LCL
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
@1
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
@SP
A = M - 1
M = !M
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
@SP
A = M - 1
M = D + M
@ARG
D = M
@1
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
M = M - D
@SP
A = M - 1
D = M
@R13
M = D
@LCL
D = M
@SP
M = D
@SP
M = M - 1
A = M
D = M
@THAT
M = D
@SP
M = M - 1
A = M
D = M
@THIS
M = D
@SP
M = M - 1
A = M
D = M
@R14
M = D
@SP
M = M - 1
A = M
D = M
@LCL
M = D
@SP
M = M - 1
A = M
D = M
@R15
M = D
@ARG
D = M
@SP
M = D + 1
@R13
D = M
@SP
A = M - 1
M = D
@R14
D = M
@ARG
M = D
@R15
A = M