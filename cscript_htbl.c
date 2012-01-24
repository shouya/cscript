#include <stdlib.h>
#include <string.h>

#include "cscript_htbl.h"

#define T struct cscript_htbl
#define NT struct cscript_htbl_node

/* private functions */
static long
_cscript_htbl_default_hash(const char *str, int hash_size);
static void
_cscript_htbl_free_node(T* htbl, NT *node);
static NT *
_cscript_htbl_construct_node(const char *k, void *v);
static long
_cscript_htbl_calc_hash(T* htbl, const char *str);

T *
cscript_htbl_new(int size, long (*hash_function)(const char *, int),
		 void (*free_function)(void *))
{
	T *tbl = calloc(1, sizeof(*tbl));
	tbl->hash_size = size;
	tbl->hash_func = hash_function;
	tbl->free_func = free_function;
	tbl->nodes = calloc(size, sizeof(NT *));
}

void
cscript_htbl_free(T *htbl)
{
	NT *p = htbl->nodes[htbl->hash_size];
	while (p != htbl->nodes) {
		--p;
		_cscript_htbl_free_node(htbl, p);
	}
	free(htbl->nodes);
	free(htbl);
}

int /* returns 0 if successed */
cscript_htbl_add(T *htbl, const char *k, void *v)
{
	long hash = _cscript_htbl_calc_hash(htbl, k);
	NT **nodeptr = &htbl->nodes[hash];

	if (*nodeptr)
		_cscript_htbl_free_node(htbl, *nodeptr);
	*nodeptr = _cscript_htbl_construct_node(k, v);
	if (! *nodeptr)
		return -1;
	return 0;
}
void
cscript_htbl_del(struct cscript_htbl *htbl, const char *k)
{
	long hash = _cscript_htbl_calc_hash(htbl, k);
	_cscript_htbl_free_node(htbl, htbl->nodes[hash]);
}

int
cscript_htbl_find(const struct cscript_htbl *htbl, const char *k, void **v)
{
	long hash = _cscript_htbl_calc_hash(htbl, k);
	if (! htbl->nodes[hash])
		return -1;
	*v = htbl->nodes[hash]->v;
	return 0;
}



/* private functions */
static long
_cscript_htbl_default_hash(const char *str, int hash_size)
{
	/* sdbm algorithm */
	unsigned long hash = 0;
	int c;

	while ((c = *str++) != 0)
		hash = c + (hash << 6) + (hash << 16) - hash;

	return hash % hash_size;
}

static void
_cscript_htbl_free_node(T *htbl, NT *node)
{
	if (node->k)
		free(node->k);
	if (htbl->free_func)
		(*htbl->free_func)(node->k);
}

static NT *
_cscript_htbl_construct_node(const char *k, void *v)
{
	NT *node;

	if (!k)
		return NULL;

	node = calloc(1, sizeof(*node));
	node->k = strdup(k);
	node->v = v;

	return node;
}

static long
_cscript_htbl_calc_hash(T* htbl, const char *str)
{
	if (htbl->hash_func)
		return (*htbl->hash_func)(str, htbl->hash_size);
	else
		return _cscript_htbl_default_hash(str, htbl->hash_size);
}

#undef T
#undef NT
