#ifndef __ASSEMBLER_H_
#define __ASSEMBLER_H_

typedef struct assembler_t assembler_t;

assembler_t *assembler_create();
int assembler_process(assembler_t *obj, const char *input_file);
void assembler_free(assembler_t *obj);

#endif