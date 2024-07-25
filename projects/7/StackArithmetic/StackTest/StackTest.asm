@256
D = A
@SP
M = D
@17
D = A
@SP
M = M + 1
A = M - 1
M = D
@17
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
@eq.true.0
D; JEQ
@SP
A = M - 1
M = 0
@eq.end.0
0; JMP
(eq.true.0)
@SP
A = M - 1
M = -1
(eq.end.0)
@17
D = A
@SP
M = M + 1
A = M - 1
M = D
@16
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
@eq.true.1
D; JEQ
@SP
A = M - 1
M = 0
@eq.end.1
0; JMP
(eq.true.1)
@SP
A = M - 1
M = -1
(eq.end.1)
@16
D = A
@SP
M = M + 1
A = M - 1
M = D
@17
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
@eq.true.2
D; JEQ
@SP
A = M - 1
M = 0
@eq.end.2
0; JMP
(eq.true.2)
@SP
A = M - 1
M = -1
(eq.end.2)
@892
D = A
@SP
M = M + 1
A = M - 1
M = D
@891
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
@891
D = A
@SP
M = M + 1
A = M - 1
M = D
@892
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
@lt.true.1
D; JLT
@SP
A = M - 1
M = 0
@lt.end.1
0; JMP
(lt.true.1)
@SP
A = M - 1
M = -1
(lt.end.1)
@891
D = A
@SP
M = M + 1
A = M - 1
M = D
@891
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
@lt.true.2
D; JLT
@SP
A = M - 1
M = 0
@lt.end.2
0; JMP
(lt.true.2)
@SP
A = M - 1
M = -1
(lt.end.2)
@32767
D = A
@SP
M = M + 1
A = M - 1
M = D
@32766
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
@gt.true.0
D; JGT
@SP
A = M - 1
M = 0
@gt.end.0
0; JMP
(gt.true.0)
@SP
A = M - 1
M = -1
(gt.end.0)
@32766
D = A
@SP
M = M + 1
A = M - 1
M = D
@32767
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
@gt.true.1
D; JGT
@SP
A = M - 1
M = 0
@gt.end.1
0; JMP
(gt.true.1)
@SP
A = M - 1
M = -1
(gt.end.1)
@32766
D = A
@SP
M = M + 1
A = M - 1
M = D
@32766
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
@gt.true.2
D; JGT
@SP
A = M - 1
M = 0
@gt.end.2
0; JMP
(gt.true.2)
@SP
A = M - 1
M = -1
(gt.end.2)
@57
D = A
@SP
M = M + 1
A = M - 1
M = D
@31
D = A
@SP
M = M + 1
A = M - 1
M = D
@53
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
@112
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
@SP
A = M - 1
M = -M
@SP
M = M - 1
A = M
D = M
@SP
A = M - 1
M = M & D
@82
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
M = M | D
@SP
A = M - 1
M = !M
