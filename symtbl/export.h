#ifndef SYMTBL_EXPORT_H
#define SYMTBL_EXPORT_H

#define SYM_TYPE_NIL    0
#define SYM_TYPE_DTLOBJ 1

#define ERR_NO_ERROR  0
#define ERR_NOT_FOUND 1
#define ERR_WRONG_ARG 2
#define ERR_ITEM_EXIST 3

#define STKNO_GLOBAL 0
#define STKNO_CURRENT (-1)

typedef int stack_no;

#ifdef __cplusplus
extern "C" {
#endif

int stack_level(void);
stack_no current_stack(void);
void push_stack(void);
void pop_stack(void);
int forall_stack(stack_no which_stack,
                 void (*do_what)(const char* name, int* flags, void** body));

int delete_symbol(stack_no which_stack, const char* name);
int insert_symbol(stack_no which_stack,
                  const char* name, int flags, void* body);
int moveto_global(stack_no old_tackno, const char* name);

int find_byname(const char* name, /* name special to find */
                int* flags, void** body, /* flags & body returns */
                stack_no* which_stack); /* location stack returns */

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SYMTBL_EXPORT_H */
