#ifndef _STRTAB_H
#define _STRTAB_H

#include <linux/types.h>

struct flex_array;
struct kmem_cache;

struct strtab_head {
    size_t nstr;
    size_t maxel;
};

struct strtab {
    struct flex_array *strarr;
    struct kmem_cache *strmem;
};

struct strtab * strtab_create(void);

/* Must release all allocated strings still containing in a strtab
*/
void strtab_destroy(struct strtab *tab);


char* strtab_pushback(struct strtab *tab, size_t size);


size_t strtab_size(struct strtab *tab);


const char * strtab_get(struct strtab *tab, size_t idx);

#endif /*_STRTAB_H*/