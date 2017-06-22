/*
 * Copyright (C) 2016 Mohamed Rashad
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * adouble with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "mpc.h"

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

mpc_parser_t* Number;
mpc_parser_t* Symbol;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* Lispy;

/* Forward Declarations */

struct smplx_val;
struct smplx_scope;
typedef struct smplx_val smplx_val;
typedef struct smplx_scope smplx_scope;

/* Lisp Value */

enum {
    LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_STR,
    LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR
};

typedef smplx_val* (*lbuiltin) (smplx_scope*, smplx_val*);

struct smplx_val {
    int type;

    /* Basic */
    double num;
    char* err;
    char* sym;
    char* str;

    /* Function */
    lbuiltin builtin;
    smplx_scope* env;
    smplx_val* formals;
    smplx_val* body;

    /* Expression */
    int count;
    smplx_val** cell;
};

smplx_val* smplx_val_num(double x) {
    smplx_val* v = malloc(sizeof (smplx_val));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

smplx_val* smplx_val_err(char* fmt, ...) {
    smplx_val* v = malloc(sizeof (smplx_val));
    v->type = LVAL_ERR;
    va_list va;
    va_start(va, fmt);
    v->err = malloc(512);
    vsnprintf(v->err, 511, fmt, va);
    v->err = realloc(v->err, strlen(v->err) + 1);
    va_end(va);
    return v;
}

smplx_val* smplx_val_sym(char* s) {
    smplx_val* v = malloc(sizeof (smplx_val));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

smplx_val* smplx_val_str(char* s) {
    smplx_val* v = malloc(sizeof (smplx_val));
    v->type = LVAL_STR;
    v->str = malloc(strlen(s) + 1);
    strcpy(v->str, s);
    return v;
}

smplx_val* smplx_val_builtin(lbuiltin func) {
    smplx_val* v = malloc(sizeof (smplx_val));
    v->type = LVAL_FUN;
    v->builtin = func;
    return v;
}

smplx_scope* smplx_scope_new(void);

smplx_val* smplx_val_lambda(smplx_val* formals, smplx_val* body) {
    smplx_val* v = malloc(sizeof (smplx_val));
    v->type = LVAL_FUN;
    v->builtin = NULL;
    v->env = smplx_scope_new();
    v->formals = formals;
    v->body = body;
    return v;
}

smplx_val* smplx_val_sexpr(void) {
    smplx_val* v = malloc(sizeof (smplx_val));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

smplx_val* smplx_val_qexpr(void) {
    smplx_val* v = malloc(sizeof (smplx_val));
    v->type = LVAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

void smplx_scope_del(smplx_scope* e);

void smplx_val_del(smplx_val* v) {

    switch (v->type) {
        case LVAL_NUM: break;
        case LVAL_FUN:
            if (!v->builtin) {
                smplx_scope_del(v->env);
                smplx_val_del(v->formals);
                smplx_val_del(v->body);
            }
            break;
        case LVAL_ERR: free(v->err);
            break;
        case LVAL_SYM: free(v->sym);
            break;
        case LVAL_STR: free(v->str);
            break;
        case LVAL_QEXPR:
        case LVAL_SEXPR:
            for (int i = 0; i < v->count; i++) {
                smplx_val_del(v->cell[i]);
            }
            free(v->cell);
            break;
    }

    free(v);
}

smplx_scope* smplx_scope_copy(smplx_scope* e);

smplx_val* smplx_val_copy(smplx_val* v) {
    smplx_val* x = malloc(sizeof (smplx_val));
    x->type = v->type;
    switch (v->type) {
        case LVAL_FUN:
            if (v->builtin) {
                x->builtin = v->builtin;
            } else {
                x->builtin = NULL;
                x->env = smplx_scope_copy(v->env);
                x->formals = smplx_val_copy(v->formals);
                x->body = smplx_val_copy(v->body);
            }
            break;
        case LVAL_NUM: x->num = v->num;
            break;
        case LVAL_ERR: x->err = malloc(strlen(v->err) + 1);
            strcpy(x->err, v->err);
            break;
        case LVAL_SYM: x->sym = malloc(strlen(v->sym) + 1);
            strcpy(x->sym, v->sym);
            break;
        case LVAL_STR: x->str = malloc(strlen(v->str) + 1);
            strcpy(x->str, v->str);
            break;
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            x->count = v->count;
            x->cell = malloc(sizeof (smplx_val*) * x->count);
            for (int i = 0; i < x->count; i++) {
                x->cell[i] = smplx_val_copy(v->cell[i]);
            }
            break;
    }
    return x;
}

smplx_val* smplx_val_add(smplx_val* v, smplx_val* x) {
    v->count++;
    v->cell = realloc(v->cell, sizeof (smplx_val*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

smplx_val* smplx_val_join(smplx_val* x, smplx_val* y) {
    for (int i = 0; i < y->count; i++) {
        x = smplx_val_add(x, y->cell[i]);
    }
    free(y->cell);
    free(y);
    return x;
}

smplx_val* smplx_val_pop(smplx_val* v, int i) {
    smplx_val* x = v->cell[i];
    memmove(&v->cell[i],
            &v->cell[i + 1], sizeof (smplx_val*) * (v->count - i - 1));
    v->count--;
    v->cell = realloc(v->cell, sizeof (smplx_val*) * v->count);
    return x;
}

smplx_val* smplx_val_take(smplx_val* v, int i) {
    smplx_val* x = smplx_val_pop(v, i);
    smplx_val_del(v);
    return x;
}

void smplx_val_print(smplx_val* v);

void smplx_val_print_expr(smplx_val* v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v->count; i++) {
        smplx_val_print(v->cell[i]);
        if (i != (v->count - 1)) {
            putchar(' ');
        }
    }
    putchar(close);
}

void smplx_val_print_str(smplx_val* v) {
    /* Make a Copy of the string */
    char* escaped = malloc(strlen(v->str) + 1);
    strcpy(escaped, v->str);
    /* Pass it through the escape function */
    escaped = mpcf_escape(escaped);
    /* Print it between " characters */
    printf("\"%s\"", escaped);
    /* free the copied string */
    free(escaped);
}

void smplx_val_print(smplx_val* v) {
    switch (v->type) {
        case LVAL_FUN:
            if (v->builtin) {
                printf("<builtin>");
            } else {
                printf("(@ ");
                smplx_val_print(v->formals);
                putchar(' ');
                smplx_val_print(v->body);
                putchar(')');
            }
            break;
        case LVAL_NUM: printf("%.3f", v->num);
            break;
        case LVAL_ERR: printf("Error: %s", v->err);
            break;
        case LVAL_SYM: printf("%s", v->sym);
            break;
        case LVAL_STR: smplx_val_print_str(v);
            break;
        case LVAL_SEXPR: smplx_val_print_expr(v, '(', ')');
            break;
        case LVAL_QEXPR: smplx_val_print_expr(v, '[', ']');
            break;
    }
}

void smplx_val_println(smplx_val* v) {
    smplx_val_print(v);
    putchar('\n');
}

int smplx_val_eq(smplx_val* x, smplx_val* y) {

    if (x->type != y->type) {
        return 0;
    }

    switch (x->type) {
        case LVAL_NUM: return (x->num == y->num);
        case LVAL_ERR: return (strcmp(x->err, y->err) == 0);
        case LVAL_SYM: return (strcmp(x->sym, y->sym) == 0);
        case LVAL_STR: return (strcmp(x->str, y->str) == 0);
        case LVAL_FUN:
            if (x->builtin || y->builtin) {
                return x->builtin == y->builtin;
            } else {
                return smplx_val_eq(x->formals, y->formals) && smplx_val_eq(x->body, y->body);
            }
        case LVAL_QEXPR:
        case LVAL_SEXPR:
            if (x->count != y->count) {
                return 0;
            }
            for (int i = 0; i < x->count; i++) {
                if (!smplx_val_eq(x->cell[i], y->cell[i])) {
                    return 0;
                }
            }
            return 1;
            break;
    }
    return 0;
}

char* ltype_name(int t) {
    switch (t) {
        case LVAL_FUN: return "Function";
        case LVAL_NUM: return "Number";
        case LVAL_ERR: return "Error";
        case LVAL_SYM: return "Symbol";
        case LVAL_STR: return "String";
        case LVAL_SEXPR: return "S-Expression";
        case LVAL_QEXPR: return "Q-Expression";
        default: return "Unknown";
    }
}


#endif /* DEFINITIONS_H */

