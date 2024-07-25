CC = gcc -std=c17
C = ./catch
O = ./out

assembler.exe : $(C)/symbol_table.o $(C)/code.o $(C)/parse.o $(C)/assembler.o $(C)/main.o
	$(CC) $(C)/symbol_table.o $(C)/code.o $(C)/parse.o $(C)/assembler.o $(C)/main.o -o $(O)/assembler.exe -Iincludes/

$(C)/main.o : src/main.c
	$(CC) src/main.c -c -o $(C)/main.o -Iincludes/

$(C)/assembler.o : src/assembler.c
	$(CC) src/assembler.c -c -o $(C)/assembler.o -Iincludes/

$(C)/parse.o : src/parse.c
	$(CC) src/parse.c -c -o $(C)/parse.o -Iincludes/

$(C)/code.o : src/code.c
	$(CC) src/code.c -c -o $(C)/code.o -Iincludes/

$(C)/symbol_table.o : src/symbol_table.c
	$(CC) src/symbol_table.c -c -o $(C)/symbol_table.o -Iincludes/