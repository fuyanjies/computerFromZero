#include <stdlib.h>
#include <stdio.h>

#include "../includes/assembler.h"
#include "../includes/parse.h"
#include "../includes/code.h"

typedef struct assembler_private_t {
    parse_t *parse;
    code_t *code;
    FILE *output;
} assembler_private_t;

extern int parse_command_line(parse_t *obj);

static const char *real_comp(const char *comp)
{
    static char res[4] = {0};
    int res_idx = 0;

    res[0] = res[1] = res[2] = res[3] = 0;

    for (int i = 0; comp[i]; ++i) {
        if (3 <= res_idx) {
            return NULL;
        }
        if (' ' != comp[i]) {
            res[res_idx++] = comp[i];
        }
    }

    return res;
}

assembler_t *assembler_create()
{
    assembler_private_t *obj = NULL;

    obj = (assembler_private_t *)malloc(sizeof(assembler_private_t));
    if (NULL == obj) {
        return NULL;
    }

    obj->code = code_create();
    if (NULL == obj->code) {
        free(obj);
        return NULL;
    }

    obj->parse = parse_init();
    if (NULL == obj->parse) {
        code_free(obj->code);
        free(obj);
        return NULL;
    }

    obj->output = NULL;

    return (assembler_t *)obj;
}

int assembler_process(assembler_t *obj, const char *input_file)
{
    assembler_private_t *pri = (assembler_private_t *)obj;
    char *output_file = NULL;
    const char *symbol = NULL;
    const char *dest = NULL;
    const char *comp = NULL;
    const char *jump = NULL;
    const char *out = NULL;
    int i;

    for (i = 0; input_file[i]; ++i);
    output_file = (char *)calloc(i+4, sizeof(char));
    for (--i; '.' != input_file[i]; --i);
    if ('a' != input_file[i+1] || 's' != input_file[i+2] || 'm' != input_file[i+3]) {
        fprintf(stderr, "The input file name's suffix can't be considered");
        return 0;
    }
    sprintf(output_file, input_file);
    sprintf(output_file + i + 1, "hack");
    
    pri->output = fopen(output_file, "wb");
    free(output_file);

    if (!parse_read_source(pri->parse, input_file)) {
        return 0;
    }

    while (parse_advance(pri->parse)) {
        if (L_COMMAND != parse_command_type(pri->parse)) {
            continue;
        }

        code_L(pri->code, parse_symbol(pri->parse), parse_command_line(pri->parse));
    }

    parse_reparse(pri->parse);

    while (parse_advance(pri->parse)) {
        switch (parse_command_type(pri->parse)) {
        case A_COMMAND :
            symbol = parse_symbol(pri->parse);
            // printf("symbol: %s\n", symbol);
            out = code_A(pri->code, symbol);
            fprintf(pri->output, "%s\n", out);
            break;
        case C_COMMAND :
            parse_divide(pri->parse, &dest, &comp, &jump);
            comp = real_comp(comp);
            if (NULL == comp) {
                fprintf(stderr, "error %s\n", parse_command_line(pri->parse));
                exit(1);
            }
            out = code_C(pri->code, dest, comp, jump);
            fprintf(pri->output, "%s\n", out);
            // printf(":done\n");
            break;
        default:
            break;
        }
    }

    fclose(pri->output);
    pri->output = NULL;
    parse_reset(pri->parse);

    printf("process done\n");
    
    return 1;
}

void assembler_free(assembler_t *obj)
{
    assembler_private_t *pri = (assembler_private_t *)obj;

    code_free(pri->code);
    parse_free(pri->parse);

    free(pri);
}