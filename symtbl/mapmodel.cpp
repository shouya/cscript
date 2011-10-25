#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>
#include <vector>
#include <string>

#include "export.h"

using namespace std;

typedef struct sym_t {
    int flags; /* special */
    void* body; /* special(dtlobj*) */
} sym_t;

typedef map<string, sym_t> stack_t;
typedef vector<stack_t> stack_array_t;

static stack_array_t mainstack;
static stack_t* crt_stack = NULL;

static stack_t* get_stack_byno(stack_no stackno);

int stack_level(void) {
    return mainstack.size();
}

stack_no current_stack(void) {
    return mainstack.size() - 1;
}

void push_stack(void) {
    mainstack.push_back(stack_t());
    crt_stack = &mainstack.back();
}

void pop_stack(void) {
    mainstack.pop_back();
    crt_stack = &mainstack.back();
}

int forall_stack(stack_no which_stack,
                 void (*do_what)(const char*, int*, void**)) {
    stack_t* thestack = get_stack_byno(which_stack);
    stack_t::iterator it;

    if (thestack == NULL) {
        return ERR_WRONG_ARG;
    }
    it = thestack->begin();
    for (; it != thestack->end(); ++it) {
        (*do_what)(it->first.c_str(), &it->second.flags, &it->second.body);
    }
    return ERR_NO_ERROR;
}

int delete_symbol(stack_no which_stack, const char* name) {
    stack_t* thestack = get_stack_byno(which_stack);
    stack_t::iterator it;

    if (thestack == NULL ||
        name == NULL) {
        return ERR_WRONG_ARG;
    }
    it = thestack->find(name);
    if (it == thestack->end()) {
        return ERR_NOT_FOUND;
    }

    thestack->erase(it);

    return ERR_NO_ERROR;
}

int insert_symbol(stack_no which_stack,
                  const char* name, int flags, void* body) {
    stack_t* thestack = get_stack_byno(which_stack);
    stack_t::iterator it;

    if (thestack == NULL ||
        name == NULL) {
        return ERR_WRONG_ARG;
    }
    if (thestack->find(name) != thestack->end()) {
        return ERR_ITEM_EXIST;
    }

    it = thestack->insert(
        pair<string, sym_t>(name, sym_t())).first;

    it->second.flags = flags;
    it->second.body = body;

    return ERR_NO_ERROR;
}

int moveto_global(stack_no old_stackno, const char* name) {
    stack_t* thestack = get_stack_byno(old_stackno);
    stack_t::iterator it;
    stack_t* globalstack = get_stack_byno(STKNO_GLOBAL);

    if (thestack == NULL ||
        name == NULL) {
        return ERR_WRONG_ARG;
    }
    if ((it = thestack->find(name)) == thestack->end()) {
        return ERR_NOT_FOUND;
    }

    globalstack->insert(*it);
    thestack->erase(it);

    return ERR_NO_ERROR;
}


int find_byname(const char* name, /* name special to find */
                int* flags, void** body, /* flags & body returns */
                stack_no* which_stack) {
    stack_no crt_stk_no;
    stack_t::iterator it;
    stack_t* crt_stk = NULL;

    if (name == NULL) {
        return ERR_WRONG_ARG;
    }

    crt_stk_no = current_stack();
    for (; crt_stk_no >= 0; --crt_stk_no) {
        crt_stk = get_stack_byno(crt_stk_no);
        it = crt_stk->find(name);
        if (it == crt_stk->end()) {
            /* Not Found */
            continue;
        }
        /* Found */
        if (which_stack != NULL) {
            *which_stack = crt_stk_no;
        }
        if (flags != NULL) {
            *flags = it->second.flags;
        }
        if (body != NULL) {
            *body = it->second.body;
        }

        return ERR_NO_ERROR;
    }
    return ERR_NOT_FOUND;
}

/* static funtion */
stack_t* get_stack_byno(stack_no stackno) {
    if (stackno >= stack_level() ||
        (stackno < 0 && stackno != STKNO_CURRENT)) {
        return NULL;
    }
    if (stackno == STKNO_CURRENT) {
        return get_stack_byno(current_stack());
    }
    if (stackno == STKNO_GLOBAL) {
        return &mainstack.front();
    }

    return (&mainstack.at(stackno));
}

