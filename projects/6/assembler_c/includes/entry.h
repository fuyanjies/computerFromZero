#ifndef __ENTRY_H_
#define __ENTRY_H_

#include "uthash.h"

typedef struct table_t table_t;

typedef struct entry_t {
    const char *symbol;
    unsigned int address;

    UT_hash_handle hh;
} entry_t;

#endif