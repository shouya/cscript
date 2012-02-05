#include <malloc.h>

#include "cscript_common.h"
#include "cscript_parser.h"
#include "cscript_parser_yacc.h"

struct _stk {
	struct csp_node *data;
	struct _stk *next;
};

static struct _stk* _top = NULL;

static struct csp_node *
_stk_pop(void);
void
_stk_push(struct csp_node *);
static struct csp_node *
_stk_top(void);
void
_stk_free(void);

static struct csp_node *
_mknode0(int type, int flags);
static struct csp_node *
_mknode1(int type, int flags, struct csp_node *op1);
static struct csp_node *
_mknode2(int type, int flags, struct csp_node *op1, struct csp_node *op2);
static struct csp_node *
_mknode3(int type, int flags, struct csp_node *op1, struct csp_node *op2,
	 struct csp_node *op3);
static struct csp_node *
_mknode4(int type, int flags, struct csp_node *op1, struct csp_node *op2,
	 struct csp_node *op3, struct csp_node *op4);

static struct csp_node *
_dealinst(int type, int flags, struct csp_node *op1, struct csp_node *op2,
	  struct csp_node *op3, struct csp_node *op4);

/* interface implements */
void
cscript_parser_init(void)
{
	if (_top != NULL) {
		_stk_free();
	}
}



extern void
cscript_emit0(int type)
{
	switch (type) {
	case _VAR_DECL: /* 0, 0 */
	case _TYPE_INT:
	case _TYPE_STR:
	case _TYPE_FLOAT:
	case _CONTINUE:
	case _BREAK:
	case _REDO:
		_stk_push(_mknode0(type, 0));
		break;

	case _PRE_INC: /* 0, 1 */
	case _PRE_DEC:
	case _POST_INC:
	case _POST_DEC:
	case _LOGINOT:
	case _BITCOM:
	case _UMINUS:
	/* case _UPLUS: useless */
	case _INDIRECTION:
	case _ADDRESS:
	case _TYPE_POINTER:
	case _ELSE_PART:
	case _SOLE_IF:
	case _STATEMENT:
	case _SWITCH_EMPTY:
	case _SWITCH_ITEM_DEFUALT:
	case _EXPR_LIST_ITEM:
	case _EMPTY_STMT:
	case _EMPTY_BLOCK:
	case _XNULL:
		_stk_push(_mknode1(type, 0,
				   _stk_pop()));
		break;

	case _ARRAY_SUB: /* 0,2 */
	case _DIRECT_ASGN:
	case _PLUS:
	case _MINUS:
	case _MULTIPLY:
	case _DIVIDE:
	case _MOD:
	case _BITAND:
	case _BITOR:
	case _BITXOR:
	case _LOGIAND:
	case _LOGIOR:
	case _TYPE_CAST:
	case _IF_PART:
	case _IF_ELSE:
	case _WHILE:
	case _DO_WHILE:
	case _SWITCH:
	case _SWITCH_ITEM:
	case _FUNC_CALL:
	case _IN:
		_stk_push(_mknode2(type, 0,
				   _stk_pop(), _stk_pop()));
		break;

	case _GLUE: /* 0,2 special */
	{
		struct csp_node *c = _stk_pop(), *p = _stk_pop();
		p->next = c;
		_stk_push(p);
	} break;


	case _COND: /* 0, 3 */
		_stk_push(_mknode3(type, 0,
				   _stk_pop(), _stk_pop(), _stk_pop()));
		break;

	case _FOR: /* 0, 4 */
		_stk_push(_mknode4(type, 0, _stk_pop(),
				   _stk_pop(), _stk_pop(), _stk_pop()));
		break;

	default: /* ignore */
		return;
	}
}

void
cscript_emit1(int type, void *a1)
{
	switch (type) {
	case _STRING: /* 1, 0 */
	case _INTEGER:
	case _FLOAT:
	case _NAME:
	case _NAME_LIST_ITEM:
	case _VAR_DECL_ITEM:
	case _TYPE_STRUCT:
		_stk_push(_mknode1(type, 0, a1));
		break;

	case _EXP_POINT_NAME: /* 1, 1 */
	case _EXP_ARROW_NAME:
	case _FUNC_DECL:
	case _VAR_DECL_ITEM_INIT:
		_stk_push(_mknode2(type, 0, a1, _stk_pop()));
		break;

	case _ASGN: /* 1, 2 */
	case _SHIFT:
	case _EQUALITY:
	case _RELATION:
	case _FOR_IN:
	case _FUNC_DEF:
		_stk_push(_mknode3(type, 0, a1,
				   _stk_pop(), _stk_pop()));
		break;

		

	default: /* ignore unrecognized item */
		return;
	}
}


void
cscript_emit2(int type, void *a1, void *a2)
{
	/* no any instruction take more than 2 arguments,
	   so this function is ignored.
	*/
}
void
cscript_emit3(int type, void *a1, void *a2, void *a3)
{
	/* no any instruction take more than 2 arguments,
	   so this function is ignored.
	*/
}


/* local functions implement */
static struct csp_node *
_stk_pop(void)
{
	struct _stk *tmp;
	struct csp_node *data;
	if (!_top) {
		cscript_error("parser stack empty.");
	}
	data = _top->data;
	tmp = _top;
	_top = _top->next;
	free(tmp);
	return data;
}
void
_stk_push(struct csp_node * data)
{
	struct _stk *node = calloc(1, sizeof(*node));
	if (!node) {
		cscript_error("parser stack full.");
	}
	node->data = data;
	node->next = _top;
	_top = node;
}
static struct csp_node *
_stk_top(void)
{
	return (_top->data);
}
void
_stk_free(void)
{
	while (_top)
		_stk_pop();
}

static struct csp_node *
_mknode0(int type, int flags)
{
	struct csp_node *node = calloc(1, sizeof(*node));
	if (!node) {
		cscript_error("cannot allocate memory at making node.");
	}
	node->type = type;
	node->flags = flags;
	return node;
}
static struct csp_node *
_mknode1(int type, int flags, struct csp_node *op1)
{
	struct csp_node *node = _mknode0(type, flags);
	node->v.op1 = op1;
	return node;
}
static struct csp_node *
_mknode2(int type, int flags, struct csp_node *op1, struct csp_node *op2)
{
	struct csp_node *node = _mknode0(type, flags);
	node->v.v2.op1 = op1;
	node->v.v2.op2 = op2;
	return node;
}
static struct csp_node *
_mknode3(int type, int flags, struct csp_node *op1, struct csp_node *op2,
	 struct csp_node *op3)
{
	struct csp_node *node = _mknode0(type, flags);
	node->v.v3.op1 = op1;
	node->v.v3.op2 = op2;
	node->v.v3.op3 = op3;
	return node;
}
static struct csp_node *
_mknode4(int type, int flags, struct csp_node *op1, struct csp_node *op2,
	 struct csp_node *op3, struct csp_node *op4)
{
	struct csp_node *node = _mknode0(type, flags);
	node->v.v4.op1 = op1;
	node->v.v4.op2 = op2;
	node->v.v4.op3 = op3;
	node->v.v4.op4 = op4;
	return node;
}

