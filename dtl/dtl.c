#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

#include "dtl.h"

#define SMALL_BUF_SIZE 16

static dtlobj* obj_newnil(void);

dtlobj* obj_nil;

/* libdtl begin */
void dtl_init(void) {
    /* initialize nil object */
    obj_nil = obj_newnil();
}
/* end of libdtl */

/* dtl object */
void obj_incref(dtlobj* obj) {
    assert(obj != 0);

    if (obj->ref > 0) {
        ++obj->ref;
    }
}

void obj_incref_range(dtlobj** objarr, int length) {
    int i;

    assert(objarr != 0);

    for (i = 0; i != length; ++i) {
        obj_incref(objarr[i]);
    }
}

void obj_decref_range(dtlobj** objarr, int length) {
    int i;

    assert(objarr != 0);

    for (i = 0; i != length; ++i) {
        obj_decref(objarr[i]);
    }
}


void obj_decref(dtlobj* obj) {
    assert(obj != 0);

    if (obj->ref > 0) {
        --obj->ref;
    }
    if (obj->ref == 0) {
        obj_free(&obj);
    }
}

/* static */
dtlobj* obj_newnil(void) {
    static dtlobj obj;

    obj.content = 0;
    obj.type = DTL_NIL;
    obj.ref = -1;

    return &obj;
}

void obj_free(dtlobj** obj) {
    assert(obj != 0 && *obj != 0);

    switch ((*obj)->type) {
    case DTL_NIL:
        if (*obj != obj_nil) {
            free(*obj);
        }
        break;

    case DTL_STR:
        str_free((dtlstr**)&((*obj)->content));
        free(*obj);
        break;

        /* TODO */
    default:
        assert(0);
        break;
    }

    *obj = 0;
}

int obj_isnil(const dtlobj* obj) {
    return (obj->type == DTL_NIL);
}

dtlobj* obj_new(int type, ...) {
    dtlobj* obj;
    va_list ap;

    if (type == DTL_NIL) {
        return obj_nil;
    }

    obj = malloc(sizeof(struct dtlobj));
    obj->type = type;
    obj->ref = 0;

    va_start(ap, type);
   
    switch (type) {
    case DTL_STR: {
        char* str = va_arg(ap, char*);
        obj->content = str_new_cstr(str);
    } break;
        /* TODO */
    default:
        assert(0);
        return 0; /* gcc's warning */
    }
    va_end(ap);

    return obj;
}

int obj_printable(dtlobj* obj, char** bufptr) {
    assert(obj != 0 && bufptr != 0);

    switch (obj->type) {
    case DTL_NIL:
        *bufptr = malloc(1);
        strcpy(*bufptr, "");
        return 1;

    case DTL_STR:
        return (str_printable((dtlstr*)obj->content, bufptr));

    case DTL_ARR:
        return (arr_printable((dtlarr*)obj->content, bufptr));

    default:
        assert(0);
        return 0;
    }
}


/* end dtl object */

/* dtl string */
dtlstr* str_new(void) {
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

    str_free(dest);
    
    *dest = malloc(sizeof(struct dtlstr));
    (*dest)->len = src->len;
    (*dest)->str = malloc(src->len);
    memcpy((*dest)->str, src->str, src->len);

    return *dest;
}

int str_to_cstr(dtlstr* str, char** bufptr) {
    assert(str != 0 && bufptr != 0);

    *bufptr = malloc(str->len + 1);
    memcpy(*bufptr, str->str, str->len);
    (*bufptr)[str->len + 1] = '\0';

    return str->len + 1;
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

void str_replace(dtlstr** dest, const dtlstr* str, int pos1,
              int pos2, const dtlstr* repltext) {
    assert(dest != 0 && str != 0);
    assert(pos1 <= str->len &&
           pos1 >= -str->len &&
           pos2 <= str->len &&
           pos2 >= -str->len);

    /* tweak pos to avaliable position */
    if (pos1 < 0) pos1 += str->len;
    if (pos2 < 0) pos2 += str->len;
    if (pos2 < pos1) pos2 ^= pos1 ^= pos2 ^= pos1; /* swap */
    
    str_free(dest);
    *dest = malloc(sizeof(struct dtlstr));

    if (repltext == 0) { /* erase mode */
        (*dest)->len = str->len - (pos2 - pos1);
        (*dest)->str = malloc((*dest)->len);

        memcpy((*dest)->str, str->str, pos1);
        memcpy((*dest)->str + pos1, str->str + pos2,
               str->len - pos2);
    } else { /* replace mode */
        (*dest)->len = str->len - (pos2 - pos1) + repltext->len;
        (*dest)->str = malloc((*dest)->len);

        memcpy((*dest)->str, str->str, pos1);
        memcpy((*dest)->str + pos1, repltext->str, repltext->len);
        memcpy((*dest)->str + pos1 + repltext->len,
               str->str + pos2, str->len - pos2);
    }
}

void str_substr(dtlstr** dest, const dtlstr* str, int pos1, int pos2) {
    assert(dest != 0 && str != 0);
    assert(pos1 <= str->len &&
           pos1 >= -str->len &&
           pos2 <= str->len &&
           pos2 >= -str->len);

    if (pos1 < 0) pos1 += str->len;
    if (pos2 < 0) pos2 += str->len;
    if (pos2 < pos1) pos2 ^= pos1 ^= pos2 ^= pos1; /* swap */

    *dest = str_new();
    (*dest)->str = malloc(pos2 - pos1);
    memcpy((*dest)->str, &str->str[pos1], pos2 - pos1);
    (*dest)->len = pos2 - pos1;
}
int str_printable(dtlstr* str, char** bufptr) {
#ifdef DEBUG
    int cplen;
    char* lenbuf = malloc(SMALL_BUF_SIZE);

    assert(str != 0 && bufptr != 0);

    cplen = sprintf(lenbuf, "%d", str->len);
    cplen += str->len;
    cplen += 3; /* char '[' & ']' & 's'*/

    *bufptr = malloc(cplen + 1);
    
    strcpy(*bufptr, "[s");
    strcat(*bufptr, lenbuf);
    strcat(*bufptr, "]");    
    memcpy(*bufptr + cplen - str->len,
           str->str, str->len);
    (*bufptr)[cplen + 1] = '\0';

    free(lenbuf);

    return cplen;
#else /* DEBUG */
    return str_to_cstr(str, bufptr);
#endif /* DEBUG */
}

void str_print(FILE* stream, dtlstr* str) {
    char* buf = 0;
    str_printable(str, &buf);
    fputs(buf, stream);
}


/* end dtl string */

/* dtl array*/
dtlarr* arr_new(void) {
    dtlarr* newarr = malloc(sizeof(dtlarr));

    newarr->a_len = newarr->len = 0;
    newarr->data = 0;

    return newarr;
}

dtlarr* arr_new_len(int initlen) {
    dtlarr* newarr;

    assert(initlen >= 0);

    if (initlen == 0) {
        return arr_new();
    }

    newarr = malloc(sizeof(dtlarr));

    newarr->data = malloc(sizeof(dtlobj*) * initlen);
    newarr->len = 0;
    newarr->a_len = initlen;

    return newarr;
}

dtlarr* arr_new_obj(dtlobj* elem) {
    dtlarr* newarr = malloc(sizeof(dtlarr));

    newarr->data = malloc(sizeof(dtlobj*) * 1);
    newarr->a_len = newarr->len = 1;

    if (elem == 0) elem = obj_nil;

    newarr->data[0] = elem;

    obj_incref(elem);

    return newarr;
}

dtlarr* arr_copy(dtlarr** dest, dtlarr* src) {
    assert(dest != 0 && src != 0);

    arr_free(dest);

    *dest = arr_new_len(src->len);
    memcpy((*dest)->data, src->data, src->len * sizeof(dtlobj*));
    (*dest)->len = src->len;
    obj_incref_range((*dest)->data, (*dest)->len);

    return (*dest);
}


int arr_len(const dtlarr* arr) {
    assert(arr != 0);

    return (arr->len);
}

dtlobj* arr_get(dtlarr* arr, int pos) {
    assert(arr != 0);
    assert(pos <= arr->len &&
           pos >= -arr->len);

    if (pos < 0) pos += arr->len;

    /* i cannot decide the following statement is or not useful*/
    obj_incref(arr->data[pos]);

    return (arr->data[pos]);
}

void arr_free(dtlarr** arr) {
    assert(arr != 0);

    if (*arr == 0) {
        return;
    }

    if ((*arr)->len != 0) {
        obj_decref_range((*arr)->data, (*arr)->len);
    }

    free(*arr);
    *arr = 0;
}

void arr_insert(dtlarr* arr, int pos, dtlobj* obj) {
    assert(arr != 0);
    assert(pos <= arr->len &&
           pos >= -arr->len);

    if (pos < 0) pos += arr->len;

    if (arr->len == arr->a_len) {
        arr->data = realloc(arr->data, arr->len + 1);
        arr->a_len = arr->len + 1;
    }

    memmove(&arr->data[pos + 1], &arr->data[pos], (arr->len - pos) * sizeof(dtlobj*));

    if (obj == 0) obj = obj_nil;
    arr->data[pos] = obj;
    obj_incref(obj);

    ++arr->len;
}

void arr_replace(dtlarr* arr, int pos1, int pos2, dtlarr* replarr) {
    assert(arr != 0);
    assert(pos1 <= arr->len &&
           pos1 >= -arr->len &&
           pos2 <= arr->len &&
           pos2 >= -arr->len);

    /* tweak pos to avaliable position */
    if (pos1 < 0) pos1 += arr->len;
    if (pos2 < 0) pos2 += arr->len;
    if (pos2 < pos1) pos2 ^= pos1 ^= pos2 ^= pos1; /* swap */
    
    obj_decref_range(&arr->data[pos1], pos2 - pos1);
    if (replarr == 0) { /* erase mode */
        // arr->len -= pos2 - pos1;
        memmove(&arr->data[pos1], &arr->data[pos2], arr->len - pos2);

        arr->len -= pos2 - pos1;
    } else { /* replace mode */
        memmove(&arr->data[pos1 + replarr->len],
                &arr->data[pos2], arr->len - pos2);
        memcpy(&arr->data[pos1], replarr->data, replarr->len);
        obj_incref_range(&arr->data[pos1], replarr->len);

        arr->len += replarr->len - (pos2 - pos1);
    }
}

void arr_range(dtlarr** newarr, dtlarr* arr, int pos1, int pos2) {
    assert(newarr != 0 && arr != 0);
    assert(pos1 <= arr->len &&
           pos1 >= -arr->len &&
           pos2 <= arr->len &&
           pos2 >= -arr->len);

    if (pos1 < 0) pos1 += arr->len;
    if (pos2 < 0) pos2 += arr->len;
    if (pos2 < pos1) pos2 ^= pos1 ^= pos2 ^= pos1; /* swap */

    *newarr = arr_new_len(pos2 - pos1);
    memcpy((*newarr)->data, &arr->data[pos1], (pos2 - pos1) * sizeof(dtlobj*));
    (*newarr)->len = pos2 - pos1;
    obj_incref_range((*newarr)->data, pos2 - pos1);
}



/* end dtl array*/



