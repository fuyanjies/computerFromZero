#include <stdio.h>
#include <stdlib.h>

#include "../includes/code.h"
#include "../includes/symbol_table.h"

typedef struct code_private_t {
    table_t *AL_table;
    table_t *COMP_table;
    table_t *JUMP_table;
    table_t *DEST_table;
} code_private_t;

static const char *to_bin(unsigned int num)
{
    static char code[17];

    memset(code, '0', 16 * sizeof(char));

    for (int i = 15; num && i >= 0; --i, num >>= 1) {
        code[i] += (num & 1);
    }

    return code;
}

static int slen(const char *s)
{
    if (NULL == s) {
        return 0;
    }

    int i = 0;
    for (; s[i]; ++i);

    return i;
}

code_t *code_create()
{
    code_private_t *obj = NULL;

    obj = (code_private_t *)malloc(sizeof(code_private_t));
    if (NULL == obj) {
        goto end;
    }

    obj->AL_table = table_create();
    if (NULL == obj->AL_table) {
        free(obj);
        obj = NULL;
        goto end;
    }

    obj->COMP_table = table_create();
    if (NULL == obj->COMP_table) {
        table_free(obj->AL_table);
        free(obj);
        obj = NULL;
        goto end;
    }

    obj->JUMP_table = table_create();
    if (NULL == obj->JUMP_table) {
        table_free(obj->AL_table);
        table_free(obj->COMP_table);
        free(obj);
        obj = NULL;

        goto end;
    }

    obj->DEST_table = table_create();
    if (NULL == obj->DEST_table) {
        table_free(obj->AL_table);
        table_free(obj->COMP_table);
        table_free(obj->JUMP_table);
        free(obj);
        obj = NULL;

        goto end;
    }

    table_add_entry(obj->AL_table, "R0", 2, 0);
    table_add_entry(obj->AL_table, "R1", 2, 1);
    table_add_entry(obj->AL_table, "R2", 2, 2);
    table_add_entry(obj->AL_table, "R3", 2, 3);
    table_add_entry(obj->AL_table, "R4", 2, 4);
    table_add_entry(obj->AL_table, "R5", 2, 5);
    table_add_entry(obj->AL_table, "R6", 2, 6);
    table_add_entry(obj->AL_table, "R7", 2, 7);
    table_add_entry(obj->AL_table, "R8", 2, 8);
    table_add_entry(obj->AL_table, "R9", 2, 9);
    table_add_entry(obj->AL_table, "R10", 3, 10);
    table_add_entry(obj->AL_table, "R11", 3, 11);
    table_add_entry(obj->AL_table, "R12", 3, 12);
    table_add_entry(obj->AL_table, "R13", 3, 13);
    table_add_entry(obj->AL_table, "R14", 3, 14);
    table_add_entry(obj->AL_table, "R15", 3, 15);
    table_add_entry(obj->AL_table, "SP", 2, 0);
    table_add_entry(obj->AL_table, "LCL", 3, 1);
    table_add_entry(obj->AL_table, "ARG", 3, 2);
    table_add_entry(obj->AL_table, "THIS", 4, 3);
    table_add_entry(obj->AL_table, "THAT", 4, 4);
    table_add_entry(obj->AL_table, "SCREEN", 6, 16384);
    table_add_entry(obj->AL_table, "KBD", 3, 24576);

    table_add_entry(obj->DEST_table, NULL, 0, 0);
    table_add_entry(obj->DEST_table, "A", 1, 4);
    table_add_entry(obj->DEST_table, "M", 1, 1);
    table_add_entry(obj->DEST_table, "D", 1, 2);
    table_add_entry(obj->DEST_table, "MD", 2, 3);
    table_add_entry(obj->DEST_table, "AM", 2, 5);
    table_add_entry(obj->DEST_table, "AD", 2, 6);
    table_add_entry(obj->DEST_table, "AMD", 3, 7);

    table_add_entry(obj->JUMP_table, NULL, 0, 0);
    table_add_entry(obj->JUMP_table, "JGT", 3, 1);
    table_add_entry(obj->JUMP_table, "JEQ", 3, 2);
    table_add_entry(obj->JUMP_table, "JGE", 3, 3);
    table_add_entry(obj->JUMP_table, "JLT", 3, 4);
    table_add_entry(obj->JUMP_table, "JNE", 3, 5);
    table_add_entry(obj->JUMP_table, "JLE", 3, 6);
    table_add_entry(obj->JUMP_table, "JMP", 3, 7);

    table_add_entry(obj->COMP_table, "0", 1, 0b0101010);
    table_add_entry(obj->COMP_table, "1", 1, 0b0111111);
    table_add_entry(obj->COMP_table, "-1", 2, 0b0111010);
    table_add_entry(obj->COMP_table, "D", 1, 0b0001100);
    table_add_entry(obj->COMP_table, "A", 1, 0b0110000);
    table_add_entry(obj->COMP_table, "M", 1, 0b1110000);
    table_add_entry(obj->COMP_table, "!D", 2, 0b0001101);
    table_add_entry(obj->COMP_table, "!A", 2, 0b0110001);
    table_add_entry(obj->COMP_table, "!M", 2, 0b1110001);
    table_add_entry(obj->COMP_table, "-D", 2, 0b0001111);
    table_add_entry(obj->COMP_table, "-A", 2, 0b0110011);
    table_add_entry(obj->COMP_table, "-M", 2, 0b1110011);
    table_add_entry(obj->COMP_table, "D+1", 3, 0b0011111);
    table_add_entry(obj->COMP_table, "1+D", 3, 0b0011111);
    table_add_entry(obj->COMP_table, "A+1", 3, 0b0110111);
    table_add_entry(obj->COMP_table, "1+A", 3, 0b0110111);
    table_add_entry(obj->COMP_table, "M+1", 3, 0b1110111);
    table_add_entry(obj->COMP_table, "1+M", 3, 0b1110111);
    table_add_entry(obj->COMP_table, "D-1", 3, 0b0001110);
    table_add_entry(obj->COMP_table, "A-1", 3, 0b0110010);
    table_add_entry(obj->COMP_table, "M-1", 3, 0b1110010);
    table_add_entry(obj->COMP_table, "D+A", 3, 0b0000010);
    table_add_entry(obj->COMP_table, "A+D", 3, 0b0000010);
    table_add_entry(obj->COMP_table, "D+M", 3, 0b1000010);
    table_add_entry(obj->COMP_table, "M+D", 3, 0b1000010);
    table_add_entry(obj->COMP_table, "D-A", 3, 0b0010011);
    table_add_entry(obj->COMP_table, "D-M", 3, 0b1010011);
    table_add_entry(obj->COMP_table, "A-D", 3, 0b0000111);
    table_add_entry(obj->COMP_table, "M-D", 3, 0b1000111);
    table_add_entry(obj->COMP_table, "D&A", 3, 0b0000000);
    table_add_entry(obj->COMP_table, "A&D", 3, 0b0000000);
    table_add_entry(obj->COMP_table, "D&M", 3, 0b1000000);
    table_add_entry(obj->COMP_table, "M&D", 3, 0b1000000);
    table_add_entry(obj->COMP_table, "D|A", 3, 0b0010101);
    table_add_entry(obj->COMP_table, "A|D", 3, 0b0010101);
    table_add_entry(obj->COMP_table, "D|M", 3, 0b1010101);
    table_add_entry(obj->COMP_table, "M|D", 3, 0b1010101);

end:
    return (code_t *)obj;
}

const char *code_A(code_t *obj, const char *symbol)
{
    static unsigned address = 16;

    code_private_t *pri = (code_private_t *)obj;
    const char *code = NULL;
    entry_t *entry = NULL;
    int symbol_size = 0;
    unsigned ad = 0;

    if ('0' <= symbol[0] && symbol[0] <= '9') {
        ad = strtol(symbol, &code, 10);
        if (*code) {
            fprintf(stderr, "No means symbol [%s]\n", symbol);
            exit(1);
        }
        goto wri;
    }

    // printf("here\n");

    symbol_size = slen(symbol);

    entry = table_get_entry(pri->AL_table, symbol, symbol_size);
    // printf("getted %p\n", entry);
    if (NULL == entry) {
        // printf("NULL\n");
        ad = address++;
        table_add_entry(pri->AL_table, symbol, symbol_size, ad);
    } else {
        ad = entry->address;
    }
    
wri:
    code = to_bin(ad);

    // printf("done\n");

    return code;
}

const char *code_C(code_t *obj, const char *dest, const char *comp, const char *jump)
{
    static char code[17] = {'1', '1', '1', };

    code_private_t *pri = (code_private_t *)obj;
    entry_t *dest_entry = NULL;
    entry_t *comp_entry = NULL;
    entry_t *jump_entry = NULL;
    const char *temp = NULL;
    int dest_size = 0;
    int comp_size = 0;
    int jump_size = 0;

    dest_size = slen(dest);
    comp_size = slen(comp);
    jump_size = slen(jump);

    // printf("[%s %d] [%s %d] [%s %d] ", dest, dest_size, comp, comp_size, jump, jump_size);
    
    dest_entry = table_get_entry(pri->DEST_table, dest, dest_size);
    if (NULL == dest_entry) {
        return NULL;
    }
    comp_entry = table_get_entry(pri->COMP_table, comp, comp_size);
    if (NULL == comp_entry) {
        return NULL;
    }
    jump_entry = table_get_entry(pri->JUMP_table, jump, jump_size);
    if (NULL == jump_entry) {
        return NULL;
    }

    temp = to_bin(comp_entry->address);
    sprintf(code+3, temp+9);
    temp = to_bin(dest_entry->address);
    sprintf(code+10, temp+13);
    temp = to_bin(jump_entry->address);
    sprintf(code+13, temp+13);
    
    // printf("done\n");

    return code;
}

void code_L(code_t *obj, const char *label, unsigned int command_line)
{
    code_private_t *pri = (code_private_t *)obj;
    entry_t *entry = NULL;
    int label_size = 0;

    label_size = slen(label);
    entry = table_get_entry(pri->AL_table, label, label_size-1);
    if (NULL != entry) {
        fprintf(stderr, "The %s had already declared\n", label);
        exit(1);
    }
    table_add_entry(pri->AL_table, label, label_size-1, command_line);
}

void code_free(code_t *obj)
{
    code_private_t *pri = (code_private_t *)obj;

    table_free(pri->AL_table);
    table_free(pri->COMP_table);
    table_free(pri->JUMP_table);
    table_free(pri->DEST_table);
    free(pri);
}