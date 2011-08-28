#include <stdio.h>

#include <map>
#include <vector>

#include "export.h"

using namespace std;

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

void forall_stack(void (*do_what)(sym_t*)) {
    stack_t::iterator it = stack->begin();
    for (; it != stack->end(); ++it) {
        (*do_what)(*it);
    }
}

int delete_symbol(stack_no which_stack, const sym_t* symbol) {
    stack_t* thestack = get_stack_byno(which_stack);
    stack_t::iterator it;

    if (thestack == NULL ||
        symbol == NULL ||
        symbol->name == NULL) {
        return ERR_WRONG_ARG;
    }
    it = thestack->find(symbol->name);
    if (it == thestack->end()) {
        return ERR_NOT_FOUND;
    }
    thestack->erase(it);

    return ERR_NO_ERROR;
}

int insert_symbol(stack_no which_stack, const sym_t* symbol) {
    stack_t* thestack = get_stack_byno(which_stack);

    if (thestack == NULL ||
        symbol == NULL ||
        symbol->name == NULL) {
        return ERR_WRONG_ARG;
    }
    if (thestack->find(symbol->name) != thestack->end()) {
        return ERR_ITEM_EXIST;
    }
    thestack->insert(*symbol);

    return ERR_NO_ERROR;
}

int moveto_global(stack_no old_stackno, const char* symbol_name) {
    stack_t* thestack = get_stack_byno(old_stackno);
    stack_t::iterator it;
    stack_t* globalstack = get_stack_byno(STKNO_GLOBAL);

    if (thestack == NULL ||
        symbol == NULL ||
        symbol->name == NULL) {
        return ERR_WRONG_ARG;
    }
    if ((it = thestack->find(symbol_name))== thestack->end()) {
        return ERR_NOT_FOUND;
    }
    globalstack->insert(*it);
    thestack->erase(it);

    return ERR_NO_ERROR;
}


int find_byname(const char* sym_name,
                const sym_t** sym_ptr, stack_no* which_stack) {
    stack_no crt_stk_no;
    stack_t::iterator it;
    stack_t* crt_stk = NULL;

    if (sym_ptr == NULL || sym_name == NULL) {
        return ERR_WRONG_ARG;
    }

    crt_stk_no = current_stack();
    for (; crt_stk_no >= 0; --crt_stk_no) {
        crt_stk = get_stack_byno(crt_stk_no);
        it = crt_stk->find(sym_name);
        if (it == crt_stk->end()) {
            /* Not Found */
            continue;
        }
        /* Found */
        if (which_stack != NULL) {
            *which_stack = crt_stk_no;
        }
        *sym_ptr = &*it;
        return ERR_NO_ERROR;
    }
    return ERR_NOT_FOUND;
}

/* internal funtion */
stack_t* get_stack_byno(stack_no stackno) {
    if (stackno >= stack_level() ||
        (stackno < 0 && stackno != STKNO_CURRENT)) {
        return NULL;
    }
    if (stackno == STKNO_CURRENT) {
        return current_stack();
    }
    if (stackno == STKNO_GLOBAL) {
        return &mainstack.front();
    }

    return (&*mainstack.at(stackno));
}
