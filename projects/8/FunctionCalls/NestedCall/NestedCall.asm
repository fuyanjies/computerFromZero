
@Sys.init
0; JMP
(Sys.init)
@4000
D = A
@SP
M = M + 1
A = M - 1
M = D
@SP
M = M - 1
A = M
D = M
@3
M = D
@5000
D = A
@SP
M = M + 1
A = M - 1
M = D
@SP
M = M - 1
A = M
D = M
@4
M = D
@ARG
D = M
@R13
M = D
@SP
D = M
@0
D = D - A
@ARG
M = D
@Sys.init.call.Sys.main.end
D = A
@SP
M = M + 1
A = M - 1
M = D
@LCL
D = M
@SP
M = M + 1
A = M - 1
M = D
@R13
D = M
@SP
M = M + 1
A = M - 1
M = D
@THIS
D = M
@SP
M = M + 1
A = M - 1
M = D
@THAT
D = M
@SP
M = M + 1
A = M - 1
M = D
@SP
D = M
@LCL
M = D
@0
D = A
@SP
M = M + 1
A = M - 1
M = D
@0
D = A
@SP
M = M + 1
A = M - 1
M = D
@0
D = A
@SP
M = M + 1
A = M - 1
M = D
@0
D = A
@SP
M = M + 1
A = M - 1
M = D
@0
D = A
@SP
M = M + 1
A = M - 1
M = D
@Sys.main
0; JMP
(Sys.init.call.Sys.main.end)
@SP
M = M - 1
A = M
D = M
@6
M = D
(Sys.init$LOOP)
@Sys.init$LOOP
0; JMP
(Sys.main)
@4001
D = A
@SP
M = M + 1
A = M - 1
M = D
@SP
M = M - 1
A = M
D = M
@3
M = D
@5001
D = A
@SP
M = M + 1
A = M - 1
M = D
@SP
M = M - 1
A = M
D = M
@4
M = D
@200
D = A
@SP
M = M + 1
A = M - 1
M = D
@LCL
D = M
@1
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
@40
D = A
@SP
M = M + 1
A = M - 1
M = D
@LCL
D = M
@2
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
@6
D = A
@SP
M = M + 1
A = M - 1
M = D
@LCL
D = M
@3
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
@123
D = A
@SP
M = M + 1
A = M - 1
M = D
@ARG
D = M
@R13
M = D
@SP
D = M
@1
D = D - A
@ARG
M = D
@Sys.main.call.Sys.add12.end
D = A
@SP
M = M + 1
A = M - 1
M = D
@LCL
D = M
@SP
M = M + 1
A = M - 1
M = D
@R13
D = M
@SP
M = M + 1
A = M - 1
M = D
@THIS
D = M
@SP
M = M + 1
A = M - 1
M = D
@THAT
D = M
@SP
M = M + 1
A = M - 1
M = D
@SP
D = M
@LCL
M = D
@Sys.add12
0; JMP
(Sys.main.call.Sys.add12.end)
@SP
M = M - 1
A = M
D = M
@5
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
@LCL
D = M
@1
A = D + A
D = M
@SP
M = M + 1
A = M - 1
M = D
@LCL
D = M
@2
A = D + A
D = M
@SP
M = M + 1
A = M - 1
M = D
@LCL
D = M
@3
A = D + A
D = M
@SP
M = M + 1
A = M - 1
M = D
@LCL
D = M
@4
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
M = M - 1
A = M
D = M
@SP
A = M - 1
M = D + M
@SP
M = M - 1
A = M
D = M
@SP
A = M - 1
M = D + M
@SP
M = M - 1
A = M
D = M
@SP
A = M - 1
M = D + M
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
0; JMP
(Sys.add12)
@4002
D = A
@SP
M = M + 1
A = M - 1
M = D
@SP
M = M - 1
A = M
D = M
@3
M = D
@5002
D = A
@SP
M = M + 1
A = M - 1
M = D
@SP
M = M - 1
A = M
D = M
@4
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
@12
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
M = D + M
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
0; JMP
