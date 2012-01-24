#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "cscript_executor.h"
#include "cscript_parser.h"
#include "cscript_runstack.h"
#include "cscript_constant.h"

#define DETERVAR(tree_node) ((tree_node) && ((tree_node)->nodetype == 'v'))

#define GETNUM(var)				\
	((var)->type == 'i' ? (var)->val->i :	\
	 (var)->type == 'f' ? (var)->val->f : 0)
#define GETSTR(var)				\
	((var)->val->s)
#define GETINT(var)				\
	((var)->val->i)


#define ISSTR(var) ((var)->type == 's')
#define ISINT(var) ((var)->type == 'i')
#define ISFLT(var) ((var)->type == 'f')
#define ISNUM(var) (ISINT(var) || ISFLT(var))

#define MKSTR(str) (mkvar(CS_VAR_STR, NULL, CS_VAR_FLAG_TMP, (str)))
#define MKINT(i)   (mkvar(CS_VAR_INT, NULL, CS_VAR_FLAG_TMP, (i)))
#define MKFLT(f)   (mkvar(CS_VAR_FLT, NULL, CS_VAR_FLAG_TMP, (f)))


#define BIN_ARG struct cs_var *lhs, struct cs_var *rhs, struct cs_runstack *rs
#define UNARY_ARG struct cs_var *o, struct cs_runstack *rs

#define DEC_BIN(name) \
	struct cs_var * \
	_cs_bin_##name(BIN_ARG)
#define DEC_UNARG(name) \
	struct cs_var *	\
	_cs_un_##name(UNARY_ARG)

#define CALL_BIN(name, l, r, rs)		\
	_cs_bin_##name(l, r, rs)



/* private operator declaration */
DEC_BIN(plus);
DEC_BIN(minus);
DEC_BIN(multiply);
DEC_BIN(divide);
DEC_BIN(bitand);
DEC_BIN(bitxor);
DEC_BIN(bitor);
DEC_BIN(lshift);
DEC_BIN(rshift);
DEC_BIN(logand);
DEC_BIN(logor);
DEC_BIN(eq);
DEC_BIN(ne);
DEC_BIN(lt);
DEC_BIN(gt);
DEC_BIN(le);
DEC_BIN(ge);



/* interface */
struct cs_bt *
eval_tree(struct cs_bt *tree, struct cscript_runstack *rs)
{
	if (tree == NULL)
		return NULL;

	switch (tree->nodetype) {
	case 'v':
		return tree;
	case 'i': case 'f': case 's':
		return eval_literal(tree, rs);
	case '=':
		return eval_asgn(tree, rs);
	case '+':
		return eval_bin(tree, rs);
	}
}

struct cs_bt *
eval_asgn(struct cs_bt *asgnmt, struct cscript_runstack *rs)
{
	struct cs_asgn *a = (struct cs_asgn *)asgnmt;
	struct cs_bt *rhs = eval_tree(rhs);
	if (DETERVAR(rhs)) {
		cscript_runstack_add(rs, a->lhs->name, rhs);
	}
	free_tree(a->lhs);
	return rhs;
}

struct cs_bt *
eval_bin(struct cs_bt *bin, struct cscript_runstack *rs)
{
	struct cs_bin *b = (struct cs_bin *)bin;
	struct cs_bt *l = eval_tree(b->lhs, rs);
	struct cs_bt *r = eval_tree(b->rhs, rs);
	struct cs_var *v;

	if (!DETERVAR(l) || !DETERVAR(r)) {
		free_tree(l); free_tree(r);
		return NULL;
	}

	switch (b->type) {
	case '+':
		v = CALL_BIN(plus, l, r, rs);
		break;
	case '-':
		v = CALL_BIN(minus, l, r, rs);
	}
	free_tree(l);
	free_tree(r);
	return v;
}

struct cs_bt * /* deal 'i', 'f', 's' */
eval_literal(struct cs_bt *l, struct cscript_runstack *rs)
{
	struct cs_var *var = NULL;
	switch (l->nodetype) {
	case 'i':
		var = mkvar(CS_VAR_INT, NULL, CS_VAR_FLAG_TMP, l->val);
	case 'f':
		var = mkvar(CS_VAR_FLT, NULL, CS_VAR_FLAG_TMP, l->val);
	case 's':
		var = mkvar(CS_VAR_STR, NULL, CS_VAR_FLAG_TMP, l->str);
	default:;
	}

	free_tree(l);

	return var;
}

#define NUMBER_OP(l, r, op)						\
	do {								\
	if (l->type == 'f' || r->type == 'f') {				\
		double d1, d2;						\
		d1 = GETNUM(lhs);					\
		d2 = GETNUM(rhs);					\
		return mkvar(CS_VAR_FLT, NULL, CS_VAR_FLAG_TMP, d1 op d2); \
	} else {							\
		long i1, i2;						\
		i1 = GETINT(lhs);					\
		i2 = GETINT(rhs);					\
		return mkvar(CS_VAR_INT, NULL, CS_VAR_FLAG_TMP, i1 op i2); \
	}								\
	while (0)

/* implement */
DEC_BIN(plus)
{
	if (ISNUM(lhs) && ISNUM(rhs)) {
		NUMBER_OP(lhs, rhs, +);
	} else if (ISSTR(lhs) && ISSTR(lhs)) {
		char *s1. *s2, *cat;
		s1 = GETSTR(lhs);
		s2 = GETSTR(rhs);
		cat = strcpy(malloc(strlen(s1) + strlen(s2) + 1), s1);
		strcat(cat, s2);
		return mkvar(CS_VAR_STR, NULL, CS_VAR_FLAG_TMP, cat);
	}

	return NULL;
}


DEC_BIN(minus)
{
	if (ISNUM(lhs) && ISNUM(rhs)) {
		NUMBER_OP(lhs, rhs, -);
	}
	return NULL;
}

DEC_BIN(multiply)
{
	if (ISNUM(lhs) && ISNUM(rhs)) {
		NUMBER_OP(lhs, rhs, *);
	} else if (ISSTR(lhs) && ISINT(rhs)) {
		char *s, *news;
		long n;
		s = GETSTR(lhs);
		n = GETINT(lhs);
		news = malloc(n * strlen(s) + 1);
		while (n--) {
			strcat(news, s);
		}
		return MKSTR(news);
	}
	return NULL;
	
}

DEC_BIN(divide)
{
	if (ISNUM(lhs) && ISNUM(rhs)) {
		NUMBER_OP(lhs, rhs, /);
	}
	return NULL;
}


#define INT_OP(lhs, rhs, op)			\
	do {					\
		if (ISINT(lhs) && ISINT(rhs)) {	\
			long l, r;		\
			l = GETINT(lhs);	\
			r = GETINT(rhs);	\
			return MKINT(l op r);	\
		}				\
		return NULL;			\
	} while (0)

DEC_BIN(bitand)
{
	INT_OP(lhs, rhs, &);
}
DEC_BIN(bitxor)
{
	INT_OP(lhs, rhs, ^);
}
DEC_BIN(bitor)
{
	INT_OP(lhs, rhs, |);
}
DEC_BIN(lshift)
{
	INT_OP(lhs, rhs, <<);
}
DEC_BIN(rshift)
{
	INT_OP(lhs, rhs, >>);
}
DEC_BIN(logand)
{
	INT_OP(lhs, rhs, &&);
}
DEC_BIN(logor)
{
	INT_OP(lhs, rhs, ||);
}


DEC_BIN(eq)
{
	if (ISINT(lhs) && ISINT(rhs)) {					
		long l1, l2;						
		l1 = GETINT(lhs);					
		l2 = GETINT(rhs);					
		return MKINT(l1 == l2);				
	} else 	if (ISNUM(lhs) && ISNUM(rhs)) {				
		double d1, d2;						
		d1 = GETFLT(lhs);					
		d2 = GETFLT(rhs);					
		return MKINT(abs(d1 - d2) < CSCRIPT_FLOAT_EQUALITY_LIMIT); 
	} else if (ISSTR(lhs) && ISSTR(rhs)) {
		char *s1, *s2;
		s1 = GETSTR(lhs);
		s2 = GETSTR(rhs);
		return MKINT(strcmp(s1, s2) == 0);
	}
}

DEC_BIN(ne)
{
	struct cs_var *var = CALL_BIN(eq, lhs, rhs, rs);
	if (var) {
		GETINT(var) = !GETINT(var);
		return var;
	}
	return NULL;
}

DEC_BIN(lt);
DEC_BIN(gt);
DEC_BIN(le);
DEC_BIN(ge);
