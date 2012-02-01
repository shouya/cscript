#include <string.h>
#include <malloc.h>


#include "cscript_common.h"
#include "cscript_btree.h"

struct _cs_bt_node {
	char *name;
	void *value;
	struct _cs_bt_node *l, *r;
};

/* extern functions' declarations */
extern char *strdup(const char *);

/* static functions' declarations */
static void
_bt_free_node(struct _cs_bt_node *n, void (*free_handler)(void *));

static int
_bt_insert(struct _cs_bt_node *n, const char *name,
	   void *value, void (*free_handler)(void*));
static struct _cs_bt_node *
_bt_new_node(struct _cs_bt_node *n, const char *name, void *value);
static void
_bt_reset_node(struct _cs_bt_node *node, void (*free_handler)(void *));
static struct _cs_bt_node *
_bt_find(struct _cs_bt_node *node, const char *name,
	 struct _cs_bt_node **outparent);
static void
_bt_delete_node(struct _cs_bt_node *node, struct _cs_bt_node *parent,
		void (*free_handler)(void *));
static void
_bt_delete0(struct _cs_bt_node *node, struct _cs_bt_node *parent,
		void (*free_handler)(void *));
static void
_bt_delete1(struct _cs_bt_node *node, struct _cs_bt_node *parent,
		void (*free_handler)(void *));
static void
_bt_delete2(struct _cs_bt_node *node, struct _cs_bt_node *parent,
		void (*free_handler)(void *));

static void
_bt_detach(struct _cs_bt_node *node, struct _cs_bt_node *parent);
static void
_bt_attach(struct _cs_bt_node *node, struct _cs_bt_node *parent);
static void
_bt_swap_node(struct _cs_bt_node *n1, struct _cs_bt_node *p1,
	      struct _cs_bt_node *n2, struct _cs_bt_node *p2);


struct cs_bt *
cscript_btree_new(void (*free_handler)(void *))
{
	struct cs_bt *bt = calloc(1, sizeof(struct cs_bt));
	if (!bt)
		cscript_error("cannot allocate memory");

	bt->free_handler = free_handler;

	return bt;
}

void
cscript_btree_free(struct cs_bt *tree)
{
	_bt_free_node(tree->tree, tree->free_handler);
	free(tree);
}

void
cscript_btree_insert(struct cs_bt *tree, const char *name, void *value)
{
	if (!_bt_insert(tree->tree, name, value, tree->free_handler)) {
		return;
	}
	/* succeeded */
}

int
cscript_btree_delete(struct cs_bt *tree, const char *name)
{
	struct _cs_bt_node *cur, *lnk;

	cur = _bt_find(tree->tree, name, &lnk);
	if (!cur)
		return 0;

	_bt_delete_node(cur, lnk, tree->free_handler);
	return 1;
}

int
cscript_btree_find(struct cs_bt *tree, const char *name, void **outvalue)
{
	struct _cs_bt_node *n = _bt_find(tree->tree, name, 0);
	if (!n)
		return 0;
	*outvalue = n->value;
	return 1;
}


/* local implements */
static void
_bt_free_node(struct _cs_bt_node *n, void (*free_handler)(void *))
{
	if (!n)
		return;
	if (n->l)
		_bt_free_node(n->l, free_handler);
	if (n->r)
		_bt_free_node(n->r, free_handler);

	_bt_reset_node(n, free_handler);

	free(n);
}

static int
_bt_insert(struct _cs_bt_node *n, const char *name,
	   void *value, void (*free_handler)(void*))
{
	struct _cs_bt_node *cur, **lnk;
	int cmp;

	lnk = &n;
	while ((cur = *lnk) != NULL) {
		cmp = strcmp(name, cur->name);
		if (cmp < 0)
			lnk = &cur->l;
		else if (cmp > 0)
			lnk = &cur->r;
		else
			break;
	}

	if (cur) {
		_bt_reset_node(cur, free_handler);
	}

	cur = _bt_new_node(0, name, value);
	*lnk = cur;
	return 1;
}

static struct _cs_bt_node *
_bt_new_node(struct _cs_bt_node *n, const char *name, void *value)
{
	struct _cs_bt_node *nn;
	if (n)
		nn = n;
	else
		nn = calloc(1, sizeof(*nn));

	if (!nn)
		cscript_error("error when allocating memory");

	nn->name = strdup(name);
	nn->value = value;
	nn->l = n->r = NULL;
	return nn;
}

static void
_bt_reset_node(struct _cs_bt_node *node, void (*free_handler)(void *))
{
	if (!node)
		return;

	if (free_handler)
		(*free_handler)(node->value);

	if (node->name)
		free(node->name);

}

static struct _cs_bt_node *
_bt_find(struct _cs_bt_node *node, const char *name,
	 struct _cs_bt_node **outparent)
{
	struct _cs_bt_node *cur, *lnk;
	int cmp;
	if (!node)
		return 0;

	cur = node;

	while (cur && ((cmp = strcmp(name, cur->name)) != 0)) {
		lnk = cur;
		if (cmp < 0)
			cur = cur->l;
		else
			cur = cur->r;
	}

	if (!cur)
		return 0;

	if (outparent)
		*outparent = lnk;

	return cur;
}

static void
_bt_delete_node(struct _cs_bt_node *node, struct _cs_bt_node *parent,
		void (*free_handler)(void *))
{
	if (node->l == 0 && node->r == 0) {
		_bt_delete0(node, parent, free_handler);
	} else if (node->l != 0 && node->r != 0) {
		_bt_delete2(node, parent, free_handler);
	} else {
		_bt_delete1(node, parent, free_handler);
	}

}

static void
_bt_delete0(struct _cs_bt_node *node, struct _cs_bt_node *parent,
		void (*free_handler)(void *))
{
	int branch = (parent->l == node);

	_bt_free_node(node, free_handler);
	if (branch)
		parent->l = 0;
	else
		parent->r = 0;
}

static void
_bt_delete1(struct _cs_bt_node *node, struct _cs_bt_node *parent,
		void (*free_handler)(void *))
{
	int branch = (parent->l == node);
	struct _cs_bt_node *child = (node->l ? node->l : node->r);

	_bt_free_node(node, free_handler);
	if (branch)
		parent->l = child;
	else
		parent->r = child;
}

static void
_bt_delete2(struct _cs_bt_node *node, struct _cs_bt_node *parent,
		void (*free_handler)(void *))
{
	struct _cs_bt_node *max, *par;

	par = parent;
	max = node->l;

	/* find the maximum node */
	while (max->r != NULL) {
		par = max;
		max = max->r;
	}

	_bt_swap_node(max, par,
		      node, parent);
	_bt_delete_node(node, par, free_handler);
}

static void
_bt_detach(struct _cs_bt_node *node, struct _cs_bt_node *parent)
{
	if (!(node && parent))
		return;

	if (parent->r == node)
		parent->r = NULL;
	else if (parent->l == node)
		parent->l = NULL;
}

static void
_bt_attach(struct _cs_bt_node *node, struct _cs_bt_node *parent)
{
	if (!(node && parent))
		return;

	if (strcmp(node->name, parent->name) < 0)
		parent->l = node;
	else
		parent->r = node;
}

static void
_bt_swap_node(struct _cs_bt_node *n1, struct _cs_bt_node *p1,
	      struct _cs_bt_node *n2, struct _cs_bt_node *p2)
{
	struct _cs_bt_node *n1l, *n1r, *n2l, *n2r;
	n1l = n1->l;
	n1r = n1->r;
	n2l = n2->l;
	n2r = n2->r;

	n1->l = n2l;
	n1->r = n2r;
	n2->l = n1l;
	n2->r = n1r;

	_bt_detach(n1, p1);
	_bt_detach(n2, p2);

	_bt_attach(n1, p2);
	_bt_attach(n2, p1);
}
