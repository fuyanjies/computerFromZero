@256
D = A
@SP
M = D
@Sys.init
0; JMP
(Main.fibonacci)
@ARG
D = M
@0
A = D + A
D = M
@SP
M = M + 1
A = M - 1
M = D
@2
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
D = M - D
@lt.true.0
D; JLT
@SP
A = M - 1
M = 0
@lt.end.0
0; JMP
(lt.true.0)
@SP
A = M - 1
M = -1
(lt.end.0)
@SP
M = M - 1
A = M
D = M
@Main.fibonacci$N_LT_2
D; JNE
@Main.fibonacci$N_GE_2
0; JMP
(Main.fibonacci$N_LT_2)
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
(Main.fibonacci$N_GE_2)
@ARG
D = M
@0
A = D + A
D = M
@SP
M = M + 1
A = M - 1
M = D
@2
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
@R13
M = D
@SP
D = M
@1
D = D - A
@ARG
M = D
@Main.fibonacci.call.Main.fibonacci.times.0.end
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
@Main.fibonacci
0; JMP
(Main.fibonacci.call.Main.fibonacci.times.0.end)
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
@R13
M = D
@SP
D = M
@1
D = D - A
@ARG
M = D
@Main.fibonacci.call.Main.fibonacci.times.1.end
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
@Main.fibonacci
0; JMP
(Main.fibonacci.call.Main.fibonacci.times.1.end)
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
(Sys.init)
@4
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
@Sys.init.call.Main.fibonacci.times.0.end
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
@Main.fibonacci
0; JMP
(Sys.init.call.Main.fibonacci.times.0.end)
(Sys.init$END)
@Sys.init$END
0; JMP
