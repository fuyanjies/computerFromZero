#ifndef __CODE_H_
#define __CODE_H_

typedef struct code_t code_t;

code_t *code_create();
const char *code_A(code_t *obj, const char *symbol);
const char *code_C(code_t *obj, const char *dest, const char *comp, const char *jump);
void code_L(code_t *obj, const char *label, unsigned int command_line);
void code_free(code_t *obj);

#endif