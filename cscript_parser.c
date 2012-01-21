#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cscript_parser.h"

#define ALLOC(ptr)					\
	do {						\
	(ptr) = calloc(1, sizeof(*(ptr)));		\
	if ((ptr) == NULL) {				\
		cscript_parser("Out of memory.");	\
		exit(EXIT_FAILURE);			\
	}		   \
	} while (0)

#define SETUP(ptr, type, typechar)		\
	struct type *ptr;			\
	do {					\
	ALLOC(ptr);				\
	ptr->nodetype = (typechar);		\
	while (0)

#define RET(ptr)				\
	return ((struct cs_bt *)ptr)



struct cs_bt;
struct cs_int;
struct cs_flt;
struct cs_str;
struct cs_tok;
struct cs_asgn;
struct cs_bin;
struct cs_unary;
struct cs_tern;
struct cs_symlst;
struct cs_explst;
struct cs_if;
struct cs_while;
struct cs_list;
struct cs_hash;
struct cs_stmt;
struct cs_block;
struct cs_code;

struct cs_bt *
mkbt(int nodetype, struct cs_bt *l, struct cs_bt *r)
{
	SETUP(b, cs_bt, nodetype);
	ret->l = l;
	ret->r = r;
	RET(b);
}
struct cs_bt *
mkint(long val)
{
	SETUP(i, cs_int, 'i');
	i->val = val;
	RET(i);
}
struct cs_bt *
mkflt(double val)
{
	SETUP(f, cs_flt, 'f');
	f->val = val;
	RET(f);
}
struct cs_bt *
mkstr(char *str)
{
	SETUP(s, cs_str, 's');
	s->str = str;
	RET(s);
}
struct cs_bt *
mktok(char *name)
{
	SETUP(t, cs_tok, 't');
	t->name = name;
	RET(t);
}
struct cs_bt *
mkasgn(int assignment, struct cs_bt *lhs, struct cs_bt *rhs)
{
	SETUP(a, cs_asgn, '=');
	switch (assignment) {
	case 1:
		rhs = mkbin(CS_BIN_PLU, lhs, rhs);break;
	case 2:
		rhs = mkbin(CS_BIN_MIN, lhs, rhs);break;
	case 3:
		rhs = mkbin(CS_BIN_MUL, lhs, rhs);break;
	case 4:
		rhs = mkbin(CS_BIN_DIV, lhs, rhs);break;
	default:;
	}
	a->lhs = (struct cs_tok *)lhs;
	a->rhs = rhs;
	RET(a);
}
struct cs_bt *
mkbin(int type, struct cs_bt *lhs, struct cs_bt *rhs)
{
	SETUP(b, cs_bin, '+');
	b->lhs = lhs;
	b->rhs = rhs;
	b->type = type;
	RET(b);
}
struct cs_bt *
mkunary(int type, struct cs_bt *operand)
{
	SETUP(u, cs_unary, '-');
	u->operand = operand;
	u->type = type;
	RET(u);
}
struct cs_bt *
mktern(int type, struct cs_bt *op1, struct cs_bt *op2, struct cs_bt *op3)
{
	SETUP(t, cs_tern, '?');
	t->type = type;
	t->op1 = op1;
	t->op2 = op2;
	t->op3 = op3;
	RET(t);
}
struct cs_bt *
mksymlst(struct cs_bt *lst, struct cs_bt *newitem)
{
	SETUP(l, cs_symlst, 'a');
	if (prev != NULL)
		((struct cs_symlst *)prev)->next = l;
	l->name = (struct cs_tok *)newitem;
	RET(l);
}
struct cs_bt *
mkexplst(struct cs_bt *lst, struct cs_bt *newitem)
{
	SETUP(l, cs_explst, 'e');
	if (prev != NULL)
		((struct cs_explst *)prev)->next = l;
	l->item = newitem;
	RET(l);
}
struct cs_bt *
mkif(struct cs_bt *cond, struct cs_bt *if_part, struct cs_bt *else_part)
{
	SETUP(i, cs_if, 'I');
	i->cond = cond;
	i->if_part = (struct cs_stmt *)if_part;
	i->else_part = (struct cs_stmt *)else_part;
	RET(i);
}

struct cs_bt *
mkunless(struct cs_bt *cond, struct cs_bt *unless_part, struct cs_bt*else_part)
{
	SETUP(u, cs_if, 'I');
	u->cond = mkunary(CS_UN_NOT, cond);
	u->if_part = (struct cs_stmt *)if_part;
	u->else_part = (struct cs_stmt *)else_part;
	RET(u);
}


struct cs_bt *
mkwhile(int mode, struct cs_bt* cond, struct cs_bt *loop_part)
{
	SETUP(w, cs_while, 'W');
        w->mode = mode;
	w->cond = cond;
	w->loop = (struct cs_stmt *)loop_part;
	RET(w);
}

struct cs_bt *
mkfor(struct cs_bt *start, struct cs_bt *cond,
      struct cs_bt *end, struct cs_bt *loop)
{
	SETUP(f, cs_for, 'F');
	f->start = (struct cs_stmt *)start;
	f->cond = cond;
	f->end = (struct cs_stmt *)end;
	f->loop = (struct cs_stmt *)loop;
	RET(f);
}
struct cs_bt *
mkforin(struct cs_bt *var, struct cs_bt *array, struct cs_bt *loop)
{
	SETUP(f, cs_forin, 'Q');
	f->name = (struct cs_tok *)var;
	f->list = (struct cs_list *)array;
	f->loop = (struct cs_stmt *)loop;
	RET(f);
}

struct cs_bt *
mklist(struct cs_bt *exp_list)
{
	SETUP(l, cs_list, 'l');
	l->content = (struct cs_explst *)exp_list;
	RET(l);
}
struct cs_bt *
mkhash(void)
{
	/* INCOMPLETED */
	RET(NULL);
}
struct cs_bt *
mkstmt(struct cs_bt *exp)
{
	SETUP(s, cs_stmt, 'S');
	s->stmt = exp;
	RET(s);
}
struct cs_bt *
mkblock(struct cs_bt *code_block)
{
	SETUP(b, cs_block, 'B');
	b->code = (struct cs_code *)code_block;
	RET(b);
}

struct cs_bt *
mkcode(struct cs_bt *code, struct cs_bt *nextstmt)
{
	SETUP(c, cs_code, 'C');
	if (code != NULL)
		((struct cs_code *)code)->next = c;
	c->stmt = nextstmt;
	RET(c);
}

struct cs_bt *
mkfcall(struct cs_bt *func, struct cs_bt *args)
{
	SETUP(f, cs_fcall, 'c');
	f->func = func;
	f->arg = (struct cs_explst *)args;
	RET(f);
}

void
free_tree(struct cs_bt *tree)
{
	if (tree == NULL)
		return;

	switch (tree->nodetype) {
	case 'i': case 'f': break;
	case 's': case 't':
		free((struct cs_str *)tree->str);
		break;
	case '=':
		freetree(((struct cs_asgn *)tree)->lhs);
		freetree(((struct cs_asgn *)tree)->rhs);
		break;
	case '+':
		freetree(((struct cs_bin *)tree)->lhs);
		freetree(((struct cs_bin *)tree)->rhs);
		break;
	case '-':
		freetree(((struct cs_unary *)tree)->operand);
		break;
	case '?':
		freetree(((struct cs_tern *)tree)->op1);
		freetree(((struct cs_tern *)tree)->op2);
		freetree(((struct cs_tern *)tree)->op3);
		break;
	case 'a':
		freetree(((struct cs_symlst *)tree)->name);
		freetree(((struct cs_symlst *)tree)->next);
		break;
	case 'e':
		freetree(((struct cs_explst *)tree)->item);
		freetree(((struct cs_explst *)tree)->next);
		break;
	case 'I':
		freetree(((struct cs_if *)tree)->cond);
		freetree(((struct cs_if *)tree)->if_part);
		freetree(((struct cs_if *)tree)->else_part);
		break;
	case 'W':
		freetree(((struct cs_while *)tree)->cond);
		freetree(((struct cs_while *)tree)->loop);
		break;
	case 'F':
		freetree(((struct cs_for *)tree)->start);
		freetree(((struct cs_for *)tree)->end);
		freetree(((struct cs_for *)tree)->cond);
		freetree(((struct cs_for *)tree)->loop);
		break;
	case 'Q':
		freetree(((struct cs_forin *)tree)->name);
		freetree(((struct cs_forin *)tree)->list);
		freetree(((struct cs_forin *)tree)->loop);
		break;
	case 'l':
		freetree(((struct cs_list *)tree)->content);
		break;
	case 'h':
		/* TODO */break;
	case 'S':
		freetree(((struct cs_stmt *)tree)->stmt);
		break;
	case 'B':
		freetree(((struct cs_block *)tree)->code);
		break;
	case 'C':
		freetree(((struct cs_code *)tree)->stmt);
		freetree(((struct cs_code *)tree)->next);
		break;
	case 'c':
		freetree(((struct cs_fcall *)tree)->func);
		freetree(((struct cs_fcall *)tree)->arg);
		break;
	default:
		cscript_error("unkown type (%d)[%c].",
			      tree->nodetype, tree->nodetype);
	}
	free(tree);
}

void
print_tree(struct cs_bt *tree)
{
	/* TODO: print whole tree */
	return;
}
