#include <stdlib.h>

#include "../includes/symbol_table.h"

typedef struct table_private_t {
    entry_t *entrys;
} table_private_t;

table_t *table_create()
{
    table_private_t *obj = NULL;

    obj = (table_private_t *)malloc(sizeof(table_private_t));
    if (NULL == obj) {
        goto end;
    }

    obj->entrys = NULL;

end:
    return (table_t *)obj;
}

int table_add_entry(table_t *obj, const char *symbol, int symbol_size, unsigned int address)
{
    table_private_t *pri = (table_private_t *)obj;
    entry_t *entry = NULL;

    HASH_FIND(hh, pri->entrys, symbol, symbol_size, entry);
    if (NULL != entry) {
        return 0;
    }
    
    entry = (entry_t *)malloc(sizeof(entry_t));
    if (NULL == entry) {
        return 0;
    }
    entry->symbol = symbol;
    entry->address = address;

    HASH_ADD_KEYPTR(hh, pri->entrys, symbol, symbol_size, entry);

    return 1;
}

entry_t *table_get_entry(table_t *obj, const char *symbol, int symbol_size)
{
    table_private_t *pri = (table_private_t *)obj;
    entry_t *entry = NULL;

    HASH_FIND(hh, pri->entrys, symbol, symbol_size, entry);

    return entry;
}

void table_free(table_t *obj)
{
    table_private_t *pri = (table_private_t *)obj;
    entry_t *cur = NULL;
    entry_t *temp = NULL;

    HASH_ITER(hh, pri->entrys, cur, temp) {
        HASH_DELETE(hh, pri->entrys, cur);
        free(cur);
    }

    free(pri);
}