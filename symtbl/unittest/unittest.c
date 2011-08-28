#include <stdio.h>

#include "../export.h"

int main() {
    
    int stk_lvl = stack_level();
    stack_no stk_no = current_stack();

    printf("stack %d/%d\n", stk_no, stk_lvl);

    push_stack();
    insert_symbol(STKNO_GLOBAL, "a", 0, 0);
    push_stack();
    insert_symbol(STKNO_CURRENT, "local", 0, 0);
    insert_symbol(STKNO_CURRENT, "another", 0, 0);




    return 0;
}
