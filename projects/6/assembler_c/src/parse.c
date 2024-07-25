#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "../includes/symbol_table.h"
#include "../includes/parse.h"

typedef struct parse_private_t {
    char *content;
    const char *command;
    int content_size;
    int command_size;
    int left, right;
    int command_line;
    int file_line;
    command_type_t command_type;
} parse_private_t;

static void parse_command_check(parse_private_t *obj)
{
    int i = 0;
    int size = obj->command_size;

    if (2 > obj->command_size) {
        fprintf(stderr, "1file line %zd error\n", obj->file_line);
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
                fprintf(stderr, "2file line %zd error\n", obj->file_line);
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
                fprintf(stderr, "3file line %zd error\n", obj->file_line);
                exit(1);
            }
        }
        if (')' != obj->command[size-1]) {
            fprintf(stderr, "4file line %zd error\n", obj->file_line);
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
            fprintf(stderr, "5file line %zd error\n", obj->file_line);
            exit(1);
        }
    }
    obj->command_type = C_COMMAND;
    return ;
}

parse_t *parse_init()
{
    parse_private_t *obj = NULL;

    obj = (parse_private_t *)malloc(sizeof(parse_private_t));
    if (NULL == obj) {
        return NULL;
    }

    obj->content = NULL;
    obj->content_size = 0;
    obj->command = NULL;
    obj->command_size = 0;
    obj->left = 0;
    obj->right = -1;
    obj->file_line = obj->command_line = 0;
    obj->command_type = BLANK;

    return (parse_t *)obj;
}

int parse_read_source(parse_t *obj, const char *input_file)
{
    parse_private_t *pri = (parse_private_t *)obj;
    FILE *input = NULL;

    input = fopen(input_file, "rb");
    if (NULL == input) {
        return 0;
    }

    fseek(input, 0, SEEK_END);
    pri->content_size = ftell(input);
    fseek(input, 0, SEEK_SET);

    pri->content = (char *)calloc(pri->content_size + 4, sizeof(char));
    if (NULL == pri->content) {
        free(obj);
        fclose(input);
        return 0;
    }
    fread(pri->content, sizeof(char), pri->content_size, input);
    pri->content[pri->content_size] = 0;
    
    fclose(input);

    return 1;
}

int parse_has_more_commands(parse_t *obj)
{
    parse_private_t *pri = (parse_private_t *)obj;

    return pri->right < pri->content_size;
}

int parse_command_line(parse_t *obj)
{
    parse_private_t *pri = (parse_private_t *)obj;

    return pri->command_line;
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
    // printf("command: [%s]\n", pri->command);

    return 1;
}

command_type_t parse_command_type(parse_t *obj)
{
    parse_private_t *pri = (parse_private_t *)obj;

    return pri->command_type;
}

const char *parse_symbol(parse_t *obj)
{
    parse_private_t *pri = (parse_private_t *)obj;

    if (A_COMMAND != pri->command_type && L_COMMAND != pri->command_type) {
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
                fprintf(stderr, "6file line %zd error\n", pri->file_line);
                exit(1);
            }
            *dest = command;
            pri->content[pri->left + i] = 0;
            f = i + 1;
        } else if (';' == command[i]) {
            if (NULL != *comp) {
                fprintf(stderr, "7file line %zd error\n", pri->file_line);
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
        fprintf(stderr, "8file line %zd error\n", pri->file_line);
        exit(1);
    }
}

void parse_reparse(parse_t *obj)
{
    parse_private_t *pri = (parse_private_t *)obj;

    pri->left = 0;
    pri->right = -1;
    pri->command = NULL;
    pri->command_line = 0;
    pri->command_size = 0;
    pri->command_type = BLANK;
    pri->file_line = 0;
}
void parse_reset(parse_t *obj)
{
    parse_private_t *pri = (parse_private_t *)obj;

    parse_reparse(obj);
    free(pri->content);
    pri->content = NULL;
    pri->content_size = 0;
}


void parse_free(parse_t *obj)
{
    parse_private_t *pri = (parse_private_t *)obj;

    free(pri->content);
    free(pri);
}