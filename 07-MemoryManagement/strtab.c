#include "strtab.h"
#include <linux/flex_array.h>
#include <linux/slab.h>

#define INITIAL_MAX_ELEMENT_SIZE 128

struct strval
{
	char *str;
};

/** Allocates a block and add link to an array
 */
static void * strtab_new_at(struct strtab * tab, unsigned int where)
{
	void* block;
	struct strval dval;

	block = kmem_cache_alloc(tab->strmem, GFP_KERNEL);
	if (block == NULL) {
		return NULL;
	}

	dval.str = block;
	if (flex_array_put(tab->strarr, where, &dval, GFP_KERNEL) != 0) {
		kmem_cache_free(tab->strmem, block);
		return NULL;
	}
	return block;
}
static void * strtab_new(struct strtab * tab)
{
	void* block = strtab_new_at(tab, strtab_size(tab) + 1);
	struct strval *data = (struct strval*)flex_array_get(tab->strarr, 0);
	struct strtab_head *hd = (struct strtab_head*)data->str;

	hd->nstr++;
	return block;
}
/** Frees an allocated bkock and remove link from array
 */
static int strtab_del(struct strtab * tab, void * ptr)
{
	return -1;
}
size_t strtab_maxel_size(struct strtab *tab)
{
	struct strval *data = (struct strval*)flex_array_get(tab->strarr, 0);
	struct strtab_head *hd = (struct strtab_head*)data->str;
    return hd->maxel;
}


struct strtab * strtab_create(void)
{
    struct strtab stab;
	struct strtab *pstab;
	struct strtab_head* phead;

	stab.strarr = flex_array_alloc(sizeof(struct strval), 1024, GFP_KERNEL);
	if (stab.strarr == NULL) {
		return NULL;
	}

	stab.strmem = kmem_cache_create("string allocator",
							INITIAL_MAX_ELEMENT_SIZE, 0, 0, 0);
	if (stab.strmem == NULL) {
		flex_array_free(stab.strarr);
		return NULL;
	}

	pstab = (struct strtab *)kmem_cache_alloc(stab.strmem, GFP_KERNEL);
	if (pstab == NULL) {
		kmem_cache_destroy(stab.strmem);
		flex_array_free(stab.strarr);
		return NULL;
	}
	memcpy(pstab, &stab, sizeof(stab));

	phead = (struct strtab_head*)strtab_new_at(pstab, 0u);
	if (phead == NULL) {
		kmem_cache_free(stab.strmem, pstab);
		kmem_cache_destroy(stab.strmem);
		flex_array_free(stab.strarr);
		return NULL;
	}
	phead->nstr = 0;
	phead->maxel = INITIAL_MAX_ELEMENT_SIZE;
    return pstab;
}

void strtab_destroy(struct strtab *tab)
{
	struct strtab stab;

	if(tab) {
		memcpy(&stab, tab, sizeof(stab));
		kmem_cache_free(stab.strmem, tab);
		kmem_cache_destroy(stab.strmem);
		flex_array_free(stab.strarr);
	}
}

char* strtab_pushback(struct strtab *tab, size_t size)
{
	//todo: implement resizeble
	if (strtab_maxel_size(tab) <= size) {
		return NULL;
	}
    return strtab_new(tab);
}

size_t strtab_size(struct strtab *tab)
{
	struct strval *data = (struct strval *)flex_array_get(tab->strarr, 0u);
	struct strtab_head *hd = (struct strtab_head*)data->str;
    return hd->nstr;
}

const char * strtab_get(struct strtab *tab, size_t idx)
{
	struct strval *data;
	size_t count = strtab_size(tab);

	if(count <= idx) {
		return NULL;
	}
	data = (struct strval *)flex_array_get(tab->strarr, idx + 1);
	return data->str;
}