#include "cscript_htbl.h"

#include "cscript_runtime.h"


#define T struct cscript_runstack
#define RUNTIME_HASHSIZE 10241


T *
cscript_runstack_new(T *parent, const char *name)
{
	T *rs = calloc(1, sizeof(*rs));

	rs->name = strdup(name);
	rs->parent = parent;
	rs->symtbl = cscript_htbl_new(RUNTIME_HASHSIZE, NULL,
				      /* TODO: Special a usable function */
				      NULL);
	return rs;
}

void
cscript_runstack_free(T *rs)
{
	if (rs->name)
		free(rs->name);
	if (rs->symtbl)
		cscript_htbl_free(rs->symtbl);
}

void
cscript_runstack_add(T *rs, const char *symname, void *value)
{
	cscript_htbl_add(rs->symtbl, symname, value);
}

void
cscript_runstack_del(T *rs, const char *symname)
{
	cscript_htbl_del(rs->symtbl, symname);
}




int
cscript_runstack_find(T *rs,
		      const char *name, void **value)
{
	int result = cscript_runstack_find_no_parent(rs, name, value);
	if (result == 0) /* found */
		return 0;
	if (rs->parent)
		return cscript_runstack_find(rs->parent, name, value);
	return -1;
}
int
cscript_runstack_find_no_parent(T *rs,
				const char *name, void **value)
{
	if (cscript_htbl_find(rs->symtbl, name, value) == 0)
		return 0;
	return -1;
}

T *
cscript_runstack_global(T *rs)
{
	T *p = rs;
	while (p->parent != 0)
		p = p->parent;
	return p;
}



