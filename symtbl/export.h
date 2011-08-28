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

typedef struct sym_t {
    const char* name; /* special */
    int flags; /* special */
    void* body; /* special(dtlobj*) */
} sym_t;


typedef int stack_no;

#ifdef __cplusplus
extern "C" {
#endif

    int stack_level(void);
    stack_no current_stack(void);
    void push_stack(void);
    void pop_stack(void);
    void forall_stack(void (*do_what)(sym_t*));
    
    int delete_symbol(stack_no which_stack, const sym_t* symbol);
    int insert_symbol(stack_no which_stack, const sym_t* symbol);
    int moveto_global(stack_no old_tackno, const char* symbol_name);
    
    int find_byname(const char* sym_name,
                    const sym_t** sym_ptr, stack_no* which_stack);


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SYMTBL_EXPORT_H */
