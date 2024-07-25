BUILD = ./build

$(BUILD)/main.o : src/main.c includes/parse.h
	@mkdir -q $(BUILD)
	gcc -std=c17 @^ -c -o @$

$(BUILD)/parse.o : src/parse.c includes/symbol_table.h includes/parse.h
	@mkdir -q $(BUILD)
	gcc -std=c17 @^ -c -o @$

$(BUILD)/symbol_table.o : src/symbol_table.c includes/symbol_table.h
	@mkdir -q $(BUILD)
	gcc -std=c17 @^ -c -o @$
	
main.exe : $(BUILD)/main.o $(BUILD)/parse.o $(BUILD)/symbol_table.o
	gcc -std=c17 symbol_table.o parse.o main.o -o main.exe