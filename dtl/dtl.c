#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "dtl.h"

/* dtl string */
dtlstr* str_new() {
    dtlstr* str;

    str = malloc(sizeof(struct dtlstr));
    str->str = 0;
    str->len = 0;

    return str;
}

dtlstr* str_new_cstr(const char* init) {
    dtlstr* str;

    assert(init != 0);

    str = malloc(sizeof(struct dtlstr));
    str->len = strlen(init);
    str->str = malloc(str->len);
    memcpy(str->str, init, str->len);

    return str;
}

dtlstr* str_copy(dtlstr** dest, dtlstr* src) {
    assert(src != 0 && dest != 0);

    str_free(*dest);
    
    *dest = malloc(sizeof(struct dtlstr));
    (*dest)->len = src->len;
    (*dest)->str = malloc(src->len);
    memcpy((*dest)->str, src->str, src->len);

    return *dest;
}

int str_to_cstr(dtlstr* str, char** bufptr) {
    int cplen;

    assert(str != 0 && bufptr != 0);

    *bufptr = malloc(str->len + 1);
    memcpy(buf, str->str, str->len);
    buf[str->len + 1] = '\0';

    return cplen;
}

int str_len(const dtlstr* str) {
    return str->len;
}

void str_free(dtlstr** str) {
    assert(str != 0);

    if (*str == 0) {
        return;
    }

    if ((*str)->len) {
        free((*str)->str);
    }

    free(*str);
    *str = 0;
}

void str_repl(dtlstr** dest, const dtlstr* str, int pos1,
              int pos2, const varstr* repltext) {
    assert(dest != 0 && str != 0);
    assert(pos1 <= str->len &&
           pos1 >= -str->len &&
           pos2 <= str->len &&
           pos2 >= -str->len);

    /* tweak pos to avaliable position */
    if (pos1 < 0) pos1 = str->len + pos1;
    if (pos2 < 0) pos2 = str->len + pos2;
    if (pos2 < pos1) pos2 ^= pos1 ^= pos2 ^= pos1; /* swap */
    
    str_free(dest);
    *dest = malloc(sizeof(struct dtlstr));

    if (repltext == 0) { /* erase mode */
        (*dest)->len = str->len - (pos2 - pos1);
        (*dest)->str = malloc((*dest)->len);

        memcpy((*dest)->str, str->str, pos1);
        memcpy((*dest)->str + pos1, str->str + pos1,
               str->len - pos2);
    } else { /* replace mode */
        (*dest)->len = str->len - (pos2 - pos1) + repltext->len;
        (*dest)->str = malloc((*dest)->len);

        memcpy((*dest)->str, str->str, pos1);
        memcpy((*dest)->str + pos1, repltext->str, repltext->len);
        memcpy((*dest)->str + pos1 + repltext->len,
               str->str + pos1, str->len - pos2);
    }
}

#ifdef DEBUG
void str_print(const dtlstr* str) {
    fwrite(str->str, 1, str->len, stdout);
}
#endif /* DEBUG */


/* end dtl string */
