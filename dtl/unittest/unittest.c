#include <stdlib.h>
#include <stdio.h>

#include "../dtl.h"


int main() {
    dtlstr *str1, *str2;
    char* buf;
    str1 = str_new_cstr("hello");
    str2 = str_new_cstr("world");

    puts("[str_copy test]");
    str_copy(&str2, str1);
    str_print(str2);
    puts("\n---\n");

    puts("[str_to_cstr test]");
    str_to_cstr(str2, &buf);
    puts(buf);
    free(buf);
    puts("---\n");

    puts("[str_len test]");
    fprintf(stdout, "strlen result: [%d]\n", str_len(str1));
    puts("---\n");

    puts("[str_erase test]");
    str_repl(&str2, str1, 2, -1, 0);
    str_print(str2);
    puts("---\n");
    str_repl(&str2, str1, 2, -1, str_new_cstr("hello"));
    str_print(str2);
    puts("");
    

    return 0;
}
