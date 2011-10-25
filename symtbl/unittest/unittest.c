#include <stdio.h>

#include "/home/cmlr/project/util.h"

#include "../export.h"

int forall_stack(stack_no which_stack,
                 void (*do_what)(const char* name, int* flags, void** body));

void callback(const char*, int*, void**);


int main() {
    
    push_stack();

    int stk_lvl = stack_level();
    stack_no stk_no = current_stack();

    printf("stack %d/%d\n", stk_no, stk_lvl);

    ut(insert_symbol(STKNO_GLOBAL, "global", 0, 1) == ERR_NO_ERROR);
    
    push_stack();
    ut(insert_symbol(STKNO_CURRENT, "local", 0, 2) == ERR_NO_ERROR);
    ut(insert_symbol(STKNO_CURRENT, "another", 0, 3) == ERR_NO_ERROR);
    ut(insert_symbol(STKNO_CURRENT, "another", 0, 4) == ERR_ITEM_EXIST);
    ut(insert_symbol(1, "another_way_express_stkno", 0, 4) == ERR_NO_ERROR);
    ut(insert_symbol(2, "no_this_stack", 0, 5) == ERR_WRONG_ARG);

    stack_no position;
    int content;

    ut(find_byname("global", 0, &content, &position) == ERR_NO_ERROR);
    ut(content == 1 && position == 0);
    
    ut(find_byname("local", 0, &content, &position) == ERR_NO_ERROR);
    ut(content == 2 && position == 1);

    ut(find_byname("another", 0, &content, &position) == ERR_NO_ERROR);
    ut(content == 3 && position == 1);


    ut(find_byname("noexist", 0, 0, 0) == ERR_NOT_FOUND);

    ut(find_byname(NULL, 0,0,0) == ERR_WRONG_ARG);

    ut(delete_symbol(STKNO_CURRENT, "global") == ERR_NOT_FOUND);
    ut(delete_symbol(STKNO_GLOBAL, "global") == ERR_NO_ERROR);
    ut(delete_symbol(STKNO_GLOBAL, NULL) == ERR_WRONG_ARG);

    ut(find_byname("global", 0, 0, 0) == ERR_NOT_FOUND);

    push_stack();
    push_stack();
    push_stack();

    ut(stack_level() == 5);
    ut(current_stack() == 4);

    pop_stack();
    pop_stack();
    pop_stack();

    pS("now for all current stack");
    forall_stack(STKNO_CURRENT, &callback);

    ut(moveto_global(STKNO_CURRENT, "another") == ERR_NO_ERROR);

    pS("now for all global stack");
    forall_stack(STKNO_GLOBAL, &callback);

    pS("now for all current stack again");
    forall_stack(STKNO_CURRENT, &callback);
    

    return 0;
}

void callback(const char* name, int* flg, void** body) {
    pS(name);
    pI(*flg);
    pI(*body);
    pS("---------");
}
