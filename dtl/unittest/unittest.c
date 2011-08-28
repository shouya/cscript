#include <stdlib.h>
#include <stdio.h>

#include "../dtl.h"


int main() {
    dtlobj* obj;

    obj = obj_new(DTL_STR);
    str_assign_cstr((dtlstr**)&obj->content, "hello world!");

    str_print(stdout, (dtlstr*)obj->content);

    return 0;
}
