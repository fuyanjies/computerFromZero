#include <stdio.h>
#include <stdlib.h>

#include "../includes/assembler.h"

int main(int argc, char *argv[])
{
    assembler_t *assembler = NULL;

    if (2 != argc) {
        fprintf(stderr, "The argumenrs' number can't match\n");
        exit(1);
    }

    assembler = assembler_create();
    if (NULL == assembler) {
        fprintf(stderr, "Some wrongs happend with assembler\n");
        exit(1);
    }

    assembler_process(assembler, argv[1]);
    assembler_free(assembler);

    return 0;
}