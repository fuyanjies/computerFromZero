#include <stdio.h>
#include <stdlib.h>

#include "../includes/parse.h"

int main(int argc, char *argv[])
{
    if (2 != argc) {
        fprintf(stderr, "the arguments' number are not match\n");
        exit(1);
    }

    parse_t *parse = parse_init(argv[1]);

    while (parse_advance(parse)) {
        switch (parse_command_type(parse)) {
        case A_COMMAND :
            parse_code_A(parse);
            break;
        case C_COMMAND :
            parse_code_C(parse);
            break;
        default :
            break;
        }
    }

    parse_free(parse);

    return 0;
}