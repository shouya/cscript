#include <stdlib.h>
#include <string.h>

#include "cscript_common.h"

#include "cscript_dtl.h"

/* private calling declaration */
static struct cscript_list_node *
_cscript_list_node_new(int type, void *val);

static struct cscript_list_node *
_cscript_list_get_last_node(struct cscript_list *list);

static struct cscript_list_node *
_cscript_list_get_pos(struct cscript_list *list, int npos);

static struct cscript_list_node *
_cscript_list_node_dup(struct cscript_list_node *node);

/* public interfaces implement */

struct cscript_str *
cscript_str_new(void)
{
	return (struct cscript_str *)calloc(1, sizeof(struct cscript_str *));
}

struct cscript_str *
cscript_str_new_str(const char *s)
{
	struct cscript_str *str = calloc(1, sizeof(struct csript_str *));
	str->len = strlen(s);
	str->s = strdup(s);
	return str;
}
struct cscript_str *
cscript_str_new_nstr(const char *s, int len)
{
	struct cscript_str *str = calloc(1, sizeof(struct csript_str *));
	str->len = (str->len = strlen(s)) > len ? len : str->len;
	str->s = strndup(s);
	return str;
}

int
cscript_str_len(const struct cscript_str *s)
{
	return s->len;
}

struct cscript_str *
cscript_str_dup(const struct cscript_str *s)
{
	struct cscript_str *new_str = calloc(1, sizeof(*new_str));

	memcpy(new_str, s, sizeof(*s));
	new_str->s = strdup(s->s);
	return new_str;
}

struct cscript_str *
cscript_str_copy(struct cscript_str *dst, const struct cscript_str *src)
{
	free(dst->s);
	dst->s = strdup(src->s);
	dst->len = strlen(dst->s);
	return dst;
}

void
cscript_str_free(struct cscript_str *s)
{
	free(s->s);
	free(s);
}

const char *
cscript_str_c_str(const struct cscript_str *s)
{
	return s->s;
}

char *
cscript_str_c_str_dup(const struct cscript_str *s)
{
	return strdup(s->s);
}


/* list function */
struct cscript_list *
cscript_list_new(void);
{
	return calloc(1, sizeof(struct cscript_list));
}

struct cscript_list *
cscript_list_append(struct cscript_list *list, int type, void *val)
{
	struct cscript_list_node *last_node =
		_cscript_list_get_last_node(list);
	last_node->next = _cscript_list_node_new(type, val);
	++list->len;
	return list;
}

struct cscript_list *
cscript_list_prepend(struct cscript_list *list, int type, void *val)
{
	struct cscript_list_node *new_node =
		_cscript_list_node_new(type, val);
	new_node->next = list->head;
	list->head = new_node;
	return new_node;
}

struct cscript_list *
cscript_list_insert(struct cscript_list *list, int pos, int type, void *val)
{
	struct cscript_list_node *prev, *new_node;
	if (pos < 0 && pos > list->len) {
		cscript_error("List access out of range.");
		return list;
	}
	if (pos == 0) {
		return cscript_list_prepend(list, type, val);
	} else if (pos == list->len) {
		return cscript_list_append(list, type, val);
	}

	prev = _cscript_list_get_pos(list, pos - 1);

	new_node = _cscript_list_node_new(type, val);
	new_node->next = prev->next;
	prev->next = new_node;

	++list->len;

	return list;
}

struct cscript_list *
cscript_list_extend(struct cscript_list *list,
		    const struct cscript_list *list_to_extend)
{
	cscript_list_node *new_node, *org_node;
	new_node = _cscript_list_get_last_node(list);
	org_node = list_to_extend->head;
	while (org_node != NULL) {
		new_node->next = _cscript_list_node_dup(org_node);
		new_node = new_node->next;
		org_node = org_node->next;
		++list->len;
	}
	return list;
}


struct cscript_list *
cscript_list_slice(const struct cscript_list *list, int start, int end)
{
	struct cscript_list *new_list;
	struct cscript_list_node *new_list_node, *org_node;

	if (start < 0 || start > list->len ||
	    end < 0 || end > list->len || start > end) {
		cscript_error("List index error.");
		return NULL;
	}

	new_list = cscript_list_new();
	org_node = _cscript_list_get_pos(list, start);

	cscript_list_append(new_list, org_node->type, org_node->val);

	org_node = org_node->next;
	++start;
	new_list_node = new_list->head;

	while (start++ < end) {
		new_list_node->next = _cscript_list_node_dup(org_node);
		new_list_node = new_list_node->next;
		cscript_list_append(new_list, org_node);
		org_node = org_node->next;
	}

	return new_list;
}

int
cscript_list_len(const struct cscript_list *list)
{
	return list->len;
}

void
cscript_list_free(struct cscript_list *list)
{
	/* TODO(shouya), Sat Jan 21 */
}


/* hash table function */


/* private functions' implements */
static struct cscript_list_node *
_cscript_list_node_new(int type, void *val)
{
	struct cscript_list_node *node =
		calloc(1, sizeof(struct cscript_list_node));
	node->type = type;
	node->value = val;

	return node;
}

static struct cscript_list_node *
_cscript_list_get_pos(struct cscript_list *list, int npos)
{
	struct cscript_list_node *node;
	node = list->head;
	while (pos != 0) {
		--pos;
		node = node->next;
	}
	return node;
}

static struct cscript_list_node *
_cscript_list_get_last_node(struct cscript_list *list)
{
	struct cscript_list_node *node = list->head;
	if (node == NULL)
		return node;
	while (node->next != NULL) {
		node = node->next;
	}
	return node;
}

static struct cscript_list_node *
_cscript_list_node_dup(struct cscript_list_node *node)
{
	return _cscript_list_node_new(node->type, node->val);
}
