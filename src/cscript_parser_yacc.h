/* header guard generated by genheader.pl */
#ifndef _cscript_parser_yacc_h_
#define _cscript_parser_yacc_h_


/*    num_operator [operator_1 need_free? [operator_2 need_free?]] */
/*
  every instruction has 2 attribute -- the size and the stack size,
  the size means the number of argument does intrustion take,
  and the stack size means how many stack item will be reduced.
  every command takes one seat on stack after it executed.
 */
enum {
	/*1,0*/	_STRING = 1,
	/*1,0*/	_INTEGER,
	/*1,0*/	_FLOAT,
	/*1,0*/	_NAME,
	/*1,1*/	_EXP_POINT_NAME,
	/*1,1*/	_EXP_ARROW_NAME,
	/*0,2*/	_ARRAY_SUB,
	/*1,2*/	_ASGN,
	/*0,2*/	_DIRECT_ASGN,
	/*1,1*/	_FUNC_DECL,
	/*1,0*/	_NAME_LIST_ITEM,
	/*0,0*/	_VAR_DECL,
	/*1,0*/	_VAR_DECL_ITEM,
	/*1,1*/	_VAR_DECL_ITEM_INIT,
	/*0,2*/	_GLUE,
	/*0,2*/	_PLUS,
	/*0,2*/	_MINUS,
	/*0,2*/	_MULTIPLY,
	/*0,2*/	_DIVIDE,
	/*0,2*/	_MOD,
	/*1,2*/	_SHIFT,
	/*1,2*/	_EQUALITY,
	/*1,2*/	_RELATION,
	/*0,2*/	_BITAND,
	/*0,2*/	_BITOR,
	/*0,2*/	_BITXOR,
	/*0,2*/	_LOGIAND,
	/*0,2*/	_LOGIOR,
	/*0,1*/	_PRE_INC,
	/*0,1*/	_PRE_DEC,
	/*0,1*/	_POST_INC,
	/*0,1*/	_POST_DEC,
	/*0,1*/	_LOGINOT,
	/*0,1*/	_BITCOM,
	/*0,1*/	_UMINUS,
	/*0,1*/	_UPLUS,
	/*0,1*/	_INDIRECTION,
	/*0,1*/	_ADDRESS,
	/*0,2*/	_TYPE_CAST,
	/*0,0*/	_TYPE_INT,
	/*0,0*/	_TYPE_STR,
	/*0,0*/	_TYPE_FLOAT,
	/*0,1*/	_TYPE_POINTER,
	/*1,0*/	_TYPE_STRUCT,
	/*0,3*/	_COND,
	/*0,2*/	_IF_PART,
	/*0,1*/	_ELSE_PART,
	/*0,1*/	_SOLE_IF,
	/*0,2*/	_IF_ELSE,
	/*0,1*/	_STATEMENT,
	/*0,2*/	_WHILE,
	/*0,4*/	_FOR,
	/*1,2*/	_FOR_IN,
	/*0,2*/	_DO_WHILE,
	/*0,0*/	_CONTINUE,
	/*0,0*/	_CONTINUE_LVL,
	/*0,0*/	_BREAK,
	/*0,0*/	_BREAK_LVL,
	/*0,0*/	_REDO,
	/*0,0*/	_REDO_LVL,
	/*0,2*/	_SWITCH,
	/*0,1*/	_SWITCH_EMPTY,
	/*0,2*/	_SWITCH_ITEM,
	/*0,1*/	_SWITCH_ITEM_DEFUALT,
	/*1,2*/	_FUNC_DEF,
	/*0,2*/	_FUNC_CALL,
	/*0,1*/	_EXPR_LIST_ITEM,
	/*0,1* /	_EXPR,*/
	/*0,2*/	_IN,
	/*0,0*/	_EMPTY_STMT,
	/*0,0*/	_EMPTY_BLOCK,
	/*0,0*/	_XNULL,
	/*vir*/	_INST_MAX,
};

extern void
cscript_emit0(int type),
cscript_emit1(int type, void *a1),
cscript_emit2(int type, void *a1, void *a2),
cscript_emit3(int type, void *a1, void *a2, void *a3);



#endif /* _cscript_parser_yacc_h_ */
