#ifndef __SYMBOL_TABLE_H_

#include "uthash.h"

typedef struct entry_t {
    const char *symbol;
    unsigned int address;
    int symbol_size;

    UT_hash_handle hh;
} entry_t;

typedef struct table_t table_t;

table_t *table_create();
int table_add_entry(table_t *obj, const char *symbol, int symbol_size, unsigned int address);
entry_t *table_get_entry(table_t *obj, const char *symbol, int symbol_size);
void table_free(table_t *obj);

#endif