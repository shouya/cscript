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
    int unique_id;
    int flags; /* special */
    void* body; /* special */
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
    int moveto_global(stack_no old_stackno, const sym_t* symbol);
    
    stack_no find_byname(const char* sym_name, sym_t** sym_ptr);
    stack_no find_byid(int sym_id, sym_t** sym_ptr);


#ifdef __cplusplus
}
#endif

#endif /* SYMTBL_EXPORT_H */
