#ifndef _DTL_DTL_H_
#define _DTL_DTL_H_

#define DTL_NIL /* null reference */
#define DTL_STR /* string */
#define DTL_ARR /* array */
#define DTL_HSH /* hash table(reserve, i have not been design how to wrote it) */


/* a dtl object */
typedef struct dtlobj {
    void* content; /* its content */
    int ref; /* its reference count */
    int type; /* object's type */
} dtlobj;

/* null object reference */
extern dtlobj* obj_nil;

/* dtl string object, it is not mutable */
typedef struct dtlstr {
    char* str; /* string, UTF8 encoding */
    int len; /* string's length */
} dtlstr;

/* dtl array object, it is mutable */
typedef struct dtlarr {
    dtlobj** data; /* data of dtlobj inside */
    int len; /* item count of the array */
    int a_len; /* allocate count of the array */
} dtlarr;

/* dtl object's operartion */
void obj_incref(dtlobj* obj);
void obj_decref(dtlobj* obj);
void obj_incref_range(dtlobj** objarr, int length);
void obj_decref_range(dtlobj** objarr, int length);

void obj_free(dtlobj** obj);
int obj_isnil(const dtlobj* obj);
dtlobj* obj_new(int type, ...);
int obj_to_printable(dtlobj* obj, char** bufptr);


/* dtl string's operation */
dtlstr* str_new(void);
dtlstr* str_new_cstr(const char* init);
dtlstr* str_copy(dtlstr** dest, dtlstr* src);
int str_to_cstr(dtlstr* str, char** bufptr);
int str_len(const dtlstr* str);
void str_free(dtlstr** str);
void str_replace(dtlstr** dest, const dtlstr* str, int pos1, int pos2, const dtlstr* repltext);
void str_substr(dtlstr** dest, const dtlstr* str, int pos1, int pos2);


#ifdef DEBUG
void str_print(const dtlstr* str);
#endif /* DEBUG */

/* dtl array's operation */
dtlarr* arr_new(void);
dtlarr* arr_new_len(int initlen);
dtlarr* arr_new_obj(dtlobj* elem);
dtlarr* arr_copy(dtlarr** dest, dtlarr* src);
int arr_len(const dtlarr* arr);
dtlobj* arr_get(dtlarr* arr, int pos);
voib arr_free(dtlarr** arr);
void arr_insert(dtlarr* arr, int pos, dtlobj* obj);
void arr_replace(dtlarr* arr, int pos1, int pos2, dtlarr* replarr);
void arr_range(dtlarr** newarr, dtlarr* arr, int pos1, int pos2);


#endif /* _DTL_DTL_H_ */
