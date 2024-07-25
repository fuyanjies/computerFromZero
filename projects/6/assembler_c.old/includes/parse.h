#ifndef __PARSE_H_

typedef struct parse_t parse_t;

typedef enum command_type_t {
    BLANK, A_COMMAND, C_COMMAND, L_COMMAND,
} command_type_t;

parse_t *parse_init(const char *file_name);
int parse_has_more_commands(parse_t *obj);
int parse_advance(parse_t *obj);
const char *parse_command(parse_t *obj);
const char *parse_symbol(parse_t *obj);
void parse_divide(parse_t *obj, const char **dest, const char **comp, const char **jump);
command_type_t parse_command_type(parse_t *obj);
void parse_free(parse_t *obj);
int parse_code_A(parse_t *obj);
int parse_code_C(parse_t *obj);

#endif