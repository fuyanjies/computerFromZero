#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "../includes/parse.h"
#include "../includes/symbol_table.h"

typedef struct parse_private_t {
    char *content;
    const char *command;
    table_t *symbol_table;
    FILE *output;
    size_t content_size;
    size_t left, right;
    size_t command_line;
    size_t command_size;
    size_t file_line;
    command_type_t command_type;
} parse_private_t;

static const char *to_bin(unsigned int num)
{
    static char code[17] = {0};

    int i = 15;

    memset(code, '0', 16 * sizeof(char));

    while (num) {
        code[i--] += num & 1;
        num >>= 1;
    }

    return code;
}

static void assembler_error()
{
    fprintf(stderr, "Some wrongs hannped with assembler\n");
    exit(1);
}

static void parse_command_check(parse_private_t *obj)
{
    int i = 0;
    int size = obj->command_size;

    if (2 > obj->command_size) {
        fprintf(stderr, "file line %zd error\n", obj->file_line);
        exit(1);
    }

    if ('/' == obj->command[0] && '/' == obj->command[1]) {
        obj->command_type = BLANK;
        return ;
    }

    if ('@' == obj->command[0]) {
        for (i = 1; i < size; ++i) {
            char c = obj->command[i];
            if (!isalnum(c) && ':' != c && '.' != c && '_' != c && '$' != c) {
                fprintf(stderr, "file line %zd error\n", obj->file_line);
                exit(1);
            }
        }
        obj->command_type = A_COMMAND;
        return ;
    }

    if ('(' == obj->command[0]) {
        for (i = 1; i < size - 1; ++i) {
            char c = obj->command[i];
            if (!isalnum(c) && ':' != c && '.' != c && '_' != c && '$' != c) {
                fprintf(stderr, "file line %zd error\n", obj->file_line);
                exit(1);
            }
        }
        if (')' != obj->command[size-1]) {
            fprintf(stderr, "file line %zd error\n", obj->file_line);
            exit(1);
        }
        obj->command_type = L_COMMAND;
        return ;
    }

    for (i = 0; i < size; ++i) {
        char c = obj->command[i];
        if ('A' != c && 'M' != c && 'D' != c && ';' != c && '=' != c && 'J' != c && 'E' != c && 
            'Q' != c && 'L' != c && 'T' != c && 'G' != c && '+' != c && '-' != c && 'P' != c &&
            '1' != c && '0' != c && 'N' != c && '!' != c && '&' != c && '|' != c) {
            fprintf(stderr, "file line %zd error\n", obj->file_line);
            exit(1);
        }
    }
    obj->command_type = C_COMMAND;
    return ;
}

static void parse_preview(parse_private_t *obj)
{
    const char *symbol = NULL;
    entry_t *entry = NULL;
    int symbol_size = 0;

    while (parse_advance((parse_t *)obj)) {
        if (L_COMMAND != obj->command_type) {
            continue;
        }

        symbol = obj->command + 1;
        symbol_size = obj->command_size - 2;

        entry = table_get_entry(obj->symbol_table, symbol, symbol_size);
        if (NULL != entry) {
            fprintf(stderr, "The label had already declared\n");
            exit(1);
        }
        table_add_entry(obj->symbol_table, symbol, symbol_size, obj->command_line);
    }

    obj->left = 0;
    obj->file_line = 0;
    obj->right = -1;
    obj->command_line = 0;
    obj->command = NULL;
    obj->command_size = 0;
    obj->command_type = BLANK;
}

parse_t *parse_init(const char *file_name)
{
    parse_private_t *obj = NULL;
    FILE *file = NULL;
    char *output_file_name;
    int i;

    for (i = 0; file_name[i]; ++i);
    output_file_name = (char *)calloc(i + 2, sizeof(char));
    if (NULL == output_file_name) {
        assembler_error();
    }
    for (--i; '.' != file_name[i]; --i);
    if ('a' != file_name[i+1] && 's' != file_name[i+2] && 'm' != file_name[i+3] && 0 != file_name[i+4]) {
        fprintf(stderr, "Can not unstandard file suffix\n");
        exit(1);
    }
    memcpy(output_file_name, file_name, i+1);
    sprintf(output_file_name+i+1, "hack");
    // output_file_name[i+1] = 'h';
    // output_file_name[i+2] = 'a';
    // output_file_name[i+3] = 'c';
    // output_file_name[i+4] = 'k';
    // output_file_name[i+5] = 0;

    obj = (parse_private_t *)calloc(1, sizeof(parse_private_t));
    if (NULL == obj) {
        assembler_error();
    }

    obj->symbol_table = table_create();
    if (NULL == obj->symbol_table) {
        assembler_error();
    }

    obj->output = fopen(output_file_name, "wb");
    if (NULL == obj->output) {
        assembler_error();
    }
    free(output_file_name);

    file = fopen(file_name, "rb");
    if (NULL == file) {
        assembler_error();
    }

    fseek(file, 0, SEEK_END);
    obj->content_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    obj->content = (char *)calloc(obj->content_size + 4, sizeof(char));
    if (NULL == obj->content) {
        assembler_error();
    }

    fread(obj->content, sizeof(char), obj->content_size, file);
    obj->content[obj->content_size] = 0;

    obj->command = NULL;
    obj->left = 0;
    obj->right = -1;
    obj->command_size = 0;
    obj->command_line = obj->file_line = 0;
    obj->command_type = BLANK;

    fclose(file);

    table_add_entry(obj->symbol_table, "R0", 2, 0);
    table_add_entry(obj->symbol_table, "R1", 2, 1);
    table_add_entry(obj->symbol_table, "R2", 2, 2);
    table_add_entry(obj->symbol_table, "R3", 2, 3);
    table_add_entry(obj->symbol_table, "R4", 2, 4);
    table_add_entry(obj->symbol_table, "R5", 2, 5);
    table_add_entry(obj->symbol_table, "R6", 2, 6);
    table_add_entry(obj->symbol_table, "R7", 2, 7);
    table_add_entry(obj->symbol_table, "R8", 2, 8);
    table_add_entry(obj->symbol_table, "R9", 2, 9);
    table_add_entry(obj->symbol_table, "R10", 3, 10);
    table_add_entry(obj->symbol_table, "R11", 3, 11);
    table_add_entry(obj->symbol_table, "R12", 3, 12);
    table_add_entry(obj->symbol_table, "R13", 3, 13);
    table_add_entry(obj->symbol_table, "R14", 3, 14);
    table_add_entry(obj->symbol_table, "R15", 3, 15);
    table_add_entry(obj->symbol_table, "SP", 2, 0);
    table_add_entry(obj->symbol_table, "LCL", 3, 1);
    table_add_entry(obj->symbol_table, "ARG", 3, 2);
    table_add_entry(obj->symbol_table, "THIS", 4, 3);
    table_add_entry(obj->symbol_table, "THAT", 4, 4);
    table_add_entry(obj->symbol_table, "SCREEN", 6, 16384);
    table_add_entry(obj->symbol_table, "KBD", 3, 24576);

    parse_preview(obj);
    // printf("preview down\n");

    return (parse_t *)obj;
}

inline int parse_has_more_commands(parse_t *obj)
{
    parse_private_t *pri = (parse_private_t *)obj;

    return pri->right < pri->content_size;
}

int parse_advance(parse_t *obj)
{
    parse_private_t *pri = (parse_private_t *)obj;

    int mid = 0;

B:  
    if (pri->right >= pri->content_size) {
        return 0;
    }

    pri->left = pri->right + 1;
    for (++pri->right; '\n' != pri->content[pri->right] && pri->right < pri->content_size; ++pri->right);
    for (; ' ' == pri->content[pri->left] && pri->left < pri->right; ++pri->left);
    for (mid = pri->right - 1; mid > pri->left && (' ' == pri->content[mid-1] || !pri->content[mid-1]); --mid);
    ++pri->file_line;
    if (pri->left >= pri->right || mid <= pri->left) {
        goto B;
    }
    pri->content[mid] = 0;
    pri->command = pri->content + pri->left;
    pri->command_size = mid - pri->left;
    parse_command_check(pri);
    if (BLANK == pri->command_type) {
        goto B;
    }
    if (L_COMMAND != pri->command_type) {
        ++pri->command_line;
    }

    return 1;
}

inline const char *parse_command(parse_t *obj)
{
    parse_private_t *pri = (parse_private_t *)obj;

    return pri->command;
}

const char *parse_symbol(parse_t *obj)
{
    parse_private_t *pri = (parse_private_t *)obj;

    if (A_COMMAND != pri->command_type) {
        return NULL;
    }

    return pri->command + 1;
}

void parse_divide(parse_t *obj, const char **dest, const char **comp, const char **jump)
{
    int i = 0;
    int size = 0;
    int f = 0;
    int s = 0;
    parse_private_t *pri = (parse_private_t *)obj;
    const char *command = pri->command;

    if (NULL == dest || NULL == comp || NULL == jump) {
        fprintf(stderr, "can't discord dest, comp or jump\n");
        exit(1);
    }
    
    size = pri->command_size;
    *dest = NULL;
    *comp = NULL;
    *jump = NULL;

    for (; i < size; ++i) {
        if ('=' == command[i]) {
            if (NULL != *dest || NULL != *comp) {
                fprintf(stderr, "file line %zd error\n", pri->file_line);
                exit(1);
            }
            *dest = command;
            pri->content[pri->left + i] = 0;
            f = i + 1;
        } else if (';' == command[i]) {
            if (NULL != *comp) {
                fprintf(stderr, "file line %zd error\n", pri->file_line);
                exit(1);
            }
            *comp = command + f;
            pri->content[pri->left + i] = 0;
            s = i + 1;
        }
    }
    
    if (NULL != *dest && NULL == *comp) {
        *comp = command + f;
    } else if (NULL == *dest && NULL != *comp) {
        *jump = command + s;
    } else {
        fprintf(stderr, "file line %zd error\n", pri->file_line);
        exit(1);
    }
}

command_type_t parse_command_type(parse_t *obj)
{
    parse_private_t *pri = (parse_private_t *)obj;

    return pri->command_type;
}

void parse_free(parse_t *obj)
{
    parse_private_t *pri = (parse_private_t *)obj;

    free(pri->content);
    free(pri->symbol_table);
    fclose(pri->output);
    free(pri);
}

int parse_code_A(parse_t *obj)
{
    static unsigned int symbol_address = 16;
    unsigned int address = 0;
    parse_private_t *pri = (parse_private_t *)obj;
    const char *code = NULL;
    entry_t *entry = NULL;

    if (A_COMMAND != pri->command_type) {
        return 0;
    }

    const char *symbol = parse_symbol(obj);
    int symbol_size = pri->command_size - 1;

    if ('0' <= symbol[0] && symbol[0] <= '9') {
        char *tmp = NULL;
        address = strtol(symbol, &tmp, 10);
        if (*tmp) {
            fprintf(stderr, "file line %zd error\n", pri->file_line);
            exit(1);
        }
    } else {
        entry = table_get_entry(pri->symbol_table, symbol, symbol_size);
        if (NULL == entry) {
            table_add_entry(pri->symbol_table, symbol, symbol_size, symbol_address++);
            entry = table_get_entry(pri->symbol_table, symbol, symbol_size);
        }
        address = entry->address;
    }
    
    code = to_bin(address);
    fprintf(pri->output, "0%s\n", code+1);
    return 1;
}

int parse_code_C(parse_t *obj)
{
    static char dest_code[4] = {0};
    static char comp_code[8] = {0};
    static char jump_code[4] = {0};

    parse_private_t *pri = (parse_private_t *)obj;
    char real_comp[4] = {0};
    const char *dest = NULL;
    const char *comp = NULL;
    const char *jump = NULL;

    memset(dest_code, '0', 3 * sizeof(char));
    memset(comp_code, '0', 7 * sizeof(char));
    memset(jump_code, '0', 3 * sizeof(char));

    if (C_COMMAND != pri->command_type) {
        return 0;
    }

    parse_divide(obj, &dest, &comp, &jump);

    if (NULL == dest) {
        goto JUMP;
    }
    if ('A' == dest[0]) {
        dest_code[0] = '1';
        if ('M' == dest[1]) {
            dest_code[2] = '1';
            if ('D' == dest[2]) {
                dest_code[1] = '1';
            } else if (dest[2]) {
                fprintf(stderr, "file line %zd error\n", pri->file_line);
                exit(1);
            }
        } else if ('D' == dest[1]) {
            dest_code[1] = '1';
            if (dest[2]) {
                fprintf(stderr, "file line %zd error\n", pri->file_line);
                exit(1);
            }
        } else if (dest[1]) {
            fprintf(stderr, "file line %zd error\n", pri->file_line);
            exit(1);
        }
    } else if ('M' == dest[0]) {
        dest_code[2] = '1';
        if ('D' == dest[1]) {
            dest_code[1] = '1';
            if (dest[2]) {
                fprintf(stderr, "file line %zd error\n", pri->file_line);
                exit(1);
            }
        } else if (dest[1]) {
            fprintf(stderr, "file line %zd error\n", pri->file_line);
            exit(1);
        }
    } else if ('D' == dest[0]) {
        dest_code[1] = '1';
        if (dest[1]) {
            fprintf(stderr, "file line %zd error\n", pri->file_line);
            exit(1);
        }
    } else if (dest[0]) {
        fprintf(stderr, "file line %zd error\n", pri->file_line);
        exit(1);
    }

JUMP:
    if (NULL == jump) {
        goto COMP;
    }
    if ('J' != jump[0]) {
        fprintf(stderr, "file line %zd error\n", pri->file_line);
        exit(1);
    }
    if ('G' == jump[1]) {
        jump_code[2] = '1';
        if ('E' == jump[2]) {
            jump_code[1] = '1';
        } else if ('T' != jump[2] && jump[2]) {
            fprintf(stderr, "file line %zd error\n", pri->file_line);
            exit(1);
        }
    } else if ('L' == jump[1]) {
        jump_code[0] = '1';
        if ('E' == jump[2]) {
            jump_code[1] = '1';
        } else if ('T' != jump[2] && jump[2]) {
            fprintf(stderr, "file line %zd error\n", pri->file_line);
            exit(1);
        }
    } else if ('E' == jump[1] && 'Q' == jump[2]) {
        jump_code[1] = '1';
    } else if ('N' == jump[1] && 'E' == jump[2]) {
        jump_code[0] = jump_code[2] = '1';
    } else if ('M' == jump[1] && 'P' == jump[2]) {
        jump_code[0] = jump_code[1] = jump_code[2] = '1';
    } else if (jump[1]) {
        fprintf(stderr, "file line %zd error\n", pri->file_line);
        exit(1);
    }

COMP:
    int real_comp_idx = 0;
    if (NULL == comp) {
        fprintf(stderr, "file line %zd error\n", pri->file_line);
        exit(1);
    }
    for (int i = 0; comp[i]; ++i) {
        if (' ' == comp[i]) {
            continue;
        }
        if (3 == real_comp_idx) {
            fprintf(stderr, "file line %zd error\n", pri->file_line);
            exit(1);
        }
        real_comp[real_comp_idx++] = comp[i];
    }

    if (0 == real_comp_idx) {
        fprintf(stderr, "file line %zd error\n", pri->file_line);
        exit(1);
    }
    if ('!' == real_comp[0]) {
        if (2 < real_comp_idx) {
            fprintf(stderr, "file line %zd error\n", pri->file_line);
            exit(1);
        }
        if ('M' == real_comp[1]) {
            sprintf(comp_code, "1110001");
        } else if ('A' == real_comp[1]) {
            sprintf(comp_code, "0110001");
        } else if ('D' == real_comp[1]) {
            sprintf(comp_code, "0001101");
        } else {
            fprintf(stderr, "file line %zd error\n", pri->file_line);
            exit(1);
        }
    } else if ('0' == real_comp[0]) {
        if (2 <= real_comp_idx) {
            fprintf(stderr, "file line %zd error\n", pri->file_line);
            exit(1);
        }
        sprintf(comp_code, "0101010");
    } else if ('1' == real_comp[0]) {
        if (2 <= real_comp_idx) {
            fprintf(stderr, "file line %zd error\n", pri->file_line);
            exit(1);
        }
        sprintf(comp_code, "0111111");
    } else if ('-' == real_comp[0]) {
        if (2 != real_comp_idx) {
            fprintf(stderr, "file line %zd error\n", pri->file_line);
            exit(1);
        }
        if ('1' == real_comp[1]) {
            sprintf(comp_code, "0111010");
        } else if ('D' == real_comp[1]) {
            sprintf(comp_code, "0001111");
        } else if ('A' == real_comp[1]) {
            sprintf(comp_code, "0110011");
        } else if ('M' == real_comp[1]) {
            sprintf(comp_code, "1110011");
        } else {
            fprintf(stderr, "file line %zd error\n", pri->file_line);
            exit(1);
        }
    } else if ('D' == real_comp[0]) {
        if (1 == real_comp_idx) {
            sprintf(comp_code, "0001100");
        } else {
            if (3 != real_comp_idx) {
                fprintf(stderr, "file line %zd error\n", pri->file_line);
                exit(1);
            }
            if ('-' == real_comp[1]) {
                if ('1' == real_comp[2]) {
                    sprintf(comp_code, "0001110");
                } else if ('A' == real_comp[2]) {
                    sprintf(comp_code, "0010011");
                } else if ('M' == real_comp[2]) {
                    sprintf(comp_code, "1010011");
                } else {
                    fprintf(stderr, "file line %zd error\n", pri->file_line);
                    exit(1);
                }
            } else if ('+' == real_comp[1]) {
                if ('1' == real_comp[2]) {
                    sprintf(comp_code, "0011111");
                } else if ('A' == real_comp[2]) {
                    sprintf(comp_code, "0000010");
                } else if ('M' == real_comp[2]) {
                    sprintf(comp_code, "1000010");
                } else {
                    fprintf(stderr, "file line %zd error\n", pri->file_line);
                    exit(1);
                }
            } else if ('|' == real_comp[1]) {
                if ('A' == real_comp[2]) {
                    sprintf(comp_code, "0010101");
                } else if ('M' == real_comp[2]) {
                    sprintf(comp_code, "1010101");
                } else {
                    fprintf(stderr, "file line %zd error\n", pri->file_line);
                    exit(1);
                }
            } else if ('&' == real_comp[1]) {
                if ('A' == real_comp[2]) {
                    sprintf(comp_code, "0000000");
                } else if ('M' == real_comp[2]) {
                    sprintf(comp_code, "1000000");
                } else {
                    fprintf(stderr, "file line %zd error\n", pri->file_line);
                    exit(1);
                }
            } else {
                fprintf(stderr, "file line %zd error\n", pri->file_line);
                exit(1);
            }
        }
    } else if ('A' == real_comp[0]) {
        if (1 == real_comp_idx) {
            sprintf(comp_code, "0110000");
        } else {
            if (3 != real_comp_idx) {
                fprintf(stderr, "file line %zd error\n", pri->file_line);
                exit(1);
            }
            if ('-' == real_comp[1]) {
                if ('1' == real_comp[2]) {
                    sprintf(comp_code, "0110010");
                } else if ('D' == real_comp[2]) {
                    sprintf(comp_code, "0000111");
                } else {
                    fprintf(stderr, "file line %zd error\n", pri->file_line);
                    exit(1);
                }
            } else if ('+' == real_comp[1]) {
                if ('1' == real_comp[2]) {
                    sprintf(comp_code, "0110111");
                } else {
                    fprintf(stderr, "file line %zd error\n", pri->file_line);
                    exit(1);
                }
            } else {
                fprintf(stderr, "file line %zd error\n", pri->file_line);
                exit(1);
            }
        }
    } else if ('M' == real_comp[0]) {
        if (1 == real_comp_idx) {
            sprintf(comp_code, "1110000");
        } else {
            if (3 != real_comp_idx) {
                fprintf(stderr, "file line %zd error\n", pri->file_line);
                exit(1);
            }
            if ('-' == real_comp[1]) {
                if ('1' == real_comp[2]) {
                    sprintf(comp_code, "1110010");
                } else if ('D' == real_comp[2]) {
                    sprintf(comp_code, "1000111");
                } else {
                    fprintf(stderr, "file line %zd error\n", pri->file_line);
                    exit(1);
                }
            } else if ('+' == real_comp[1]) {
                if ('1' == real_comp[2]) {
                    sprintf(comp_code, "1110111");
                } else {
                    fprintf(stderr, "file line %zd error\n", pri->file_line);
                    exit(1);
                }
            } else {
                fprintf(stderr, "file line %zd error\n", pri->file_line);
                exit(1);
            }
        }
    }

    fprintf(pri->output, "111%s%s%s\n", comp_code, dest_code, jump_code);

    return 1;
}