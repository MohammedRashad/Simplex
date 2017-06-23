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


#ifndef BUILT_IN_H
#define BUILT_IN_H

/* Builtins */

#define LASSERT(args, cond, fmt, ...) \
  if (!(cond)) { smplx_val* err = smplx_val_err(fmt, ##__VA_ARGS__); smplx_val_del(args); return err; }

#define LASSERT_TYPE(func, args, index, expect) \
  LASSERT(args, args->cell[index]->type == expect, \
    "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s.", \
    func, index, ltype_name(args->cell[index]->type), ltype_name(expect))

#define LASSERT_NUM(func, args, num) \
  LASSERT(args, args->count == num, \
    "Function '%s' passed incorrect number of arguments. Got %i, Expected %i.", \
    func, args->count, num)

#define LASSERT_NOT_EMPTY(func, args, index) \
  LASSERT(args, args->cell[index]->count != 0, \
    "Function '%s' passed {} for argument %i.", func, index);

smplx_val* smplx_val_eval(smplx_scope* e, smplx_val* v);

smplx_val* builtin_lambda(smplx_scope* e, smplx_val* a) {
    LASSERT_NUM("@", a, 2);
    LASSERT_TYPE("@", a, 0, LVAL_QEXPR);
    LASSERT_TYPE("@", a, 1, LVAL_QEXPR);

    for (int i = 0; i < a->cell[0]->count; i++) {
        LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM),
                "Cannot define non-symbol. Got %s, Expected %s.",
                ltype_name(a->cell[0]->cell[i]->type), ltype_name(LVAL_SYM));
    }

    smplx_val* formals = smplx_val_pop(a, 0);
    smplx_val* body = smplx_val_pop(a, 0);
    smplx_val_del(a);

    return smplx_val_lambda(formals, body);
}

smplx_val* builtin_list(smplx_scope* e, smplx_val* a) {
    a->type = LVAL_QEXPR;
    return a;
}

smplx_val* builtin_head(smplx_scope* e, smplx_val* a) {
    LASSERT_NUM("head", a, 1);
    LASSERT_TYPE("head", a, 0, LVAL_QEXPR);
    LASSERT_NOT_EMPTY("head", a, 0);

    smplx_val* v = smplx_val_take(a, 0);
    while (v->count > 1) {
        smplx_val_del(smplx_val_pop(v, 1));
    }
    return v;
}

smplx_val* builtin_tail(smplx_scope* e, smplx_val* a) {
    LASSERT_NUM("tail", a, 1);
    LASSERT_TYPE("tail", a, 0, LVAL_QEXPR);
    LASSERT_NOT_EMPTY("tail", a, 0);

    smplx_val* v = smplx_val_take(a, 0);
    smplx_val_del(smplx_val_pop(v, 0));
    return v;
}

smplx_val* builtin_eval(smplx_scope* e, smplx_val* a) {
    LASSERT_NUM("eval", a, 1);
    LASSERT_TYPE("eval", a, 0, LVAL_QEXPR);

    smplx_val* x = smplx_val_take(a, 0);
    x->type = LVAL_SEXPR;
    return smplx_val_eval(e, x);
}

smplx_val* builtin_join(smplx_scope* e, smplx_val* a) {

    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE("join", a, i, LVAL_QEXPR);
    }

    smplx_val* x = smplx_val_pop(a, 0);

    while (a->count) {
        smplx_val* y = smplx_val_pop(a, 0);
        x = smplx_val_join(x, y);
    }

    smplx_val_del(a);
    return x;
}

smplx_val* builtin_op(smplx_scope* e, smplx_val* a, char* op) {

    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE(op, a, i, LVAL_NUM);
    }

    smplx_val* x = smplx_val_pop(a, 0);

    if ((strcmp(op, "-") == 0) && a->count == 0) {
        x->num = -x->num;
    }

    while (a->count > 0) {
        smplx_val* y = smplx_val_pop(a, 0);

        if (strcmp(op, "+") == 0) {
            x->num += y->num;
        }
        if (strcmp(op, "-") == 0) {
            x->num -= y->num;
        }
        if (strcmp(op, "*") == 0) {
            x->num *= y->num;
        }
        if (strcmp(op, "/") == 0) {
            if (y->num == 0) {
                smplx_val_del(x);
                smplx_val_del(y);
                x = smplx_val_err("Division By Zero.");
                break;
            }
            x->num /= y->num;
        }

        smplx_val_del(y);
    }

    smplx_val_del(a);
    return x;
}

smplx_val* builtin_add(smplx_scope* e, smplx_val* a) {
    return builtin_op(e, a, "+");
}

smplx_val* builtin_sub(smplx_scope* e, smplx_val* a) {
    return builtin_op(e, a, "-");
}

smplx_val* builtin_mul(smplx_scope* e, smplx_val* a) {
    return builtin_op(e, a, "*");
}

smplx_val* builtin_div(smplx_scope* e, smplx_val* a) {
    return builtin_op(e, a, "/");
}

smplx_val* builtin_var(smplx_scope* e, smplx_val* a, char* func) {
    LASSERT_TYPE(func, a, 0, LVAL_QEXPR);

    smplx_val* syms = a->cell[0];
    for (int i = 0; i < syms->count; i++) {
        LASSERT(a, (syms->cell[i]->type == LVAL_SYM),
                "Function '%s' cannot define non-symbol. "
                "Got %s, Expected %s.",
                func, ltype_name(syms->cell[i]->type), ltype_name(LVAL_SYM));
    }

    LASSERT(a, (syms->count == a->count - 1),
            "Function '%s' passed too many arguments for symbols. "
            "Got %i, Expected %i.",
            func, syms->count, a->count - 1);

    for (int i = 0; i < syms->count; i++) {
        if (strcmp(func, "define") == 0) {
            smplx_scope_def(e, syms->cell[i], a->cell[i + 1]);
        }
        if (strcmp(func, "=") == 0) {
            smplx_scope_put(e, syms->cell[i], a->cell[i + 1]);
        }
    }

    smplx_val_del(a);
    return smplx_val_sexpr();
}

smplx_val* builtin_def(smplx_scope* e, smplx_val* a) {
    return builtin_var(e, a, "define");
}

smplx_val* builtin_put(smplx_scope* e, smplx_val* a) {
    return builtin_var(e, a, "let");
}

smplx_val* builtin_ord(smplx_scope* e, smplx_val* a, char* op) {
    LASSERT_NUM(op, a, 2);
    LASSERT_TYPE(op, a, 0, LVAL_NUM);
    LASSERT_TYPE(op, a, 1, LVAL_NUM);

    int r;
    if (strcmp(op, ">") == 0) {
        r = (a->cell[0]->num > a->cell[1]->num);
    }
    if (strcmp(op, "<") == 0) {
        r = (a->cell[0]->num < a->cell[1]->num);
    }
    if (strcmp(op, ">=") == 0) {
        r = (a->cell[0]->num >= a->cell[1]->num);
    }
    if (strcmp(op, "<=") == 0) {
        r = (a->cell[0]->num <= a->cell[1]->num);
    }
    smplx_val_del(a);
    return smplx_val_num(r);
}

smplx_val* builtin_gt(smplx_scope* e, smplx_val* a) {
    return builtin_ord(e, a, ">");
}

smplx_val* builtin_lt(smplx_scope* e, smplx_val* a) {
    return builtin_ord(e, a, "<");
}

smplx_val* builtin_ge(smplx_scope* e, smplx_val* a) {
    return builtin_ord(e, a, ">=");
}

smplx_val* builtin_le(smplx_scope* e, smplx_val* a) {
    return builtin_ord(e, a, "<=");
}

smplx_val* builtin_cmp(smplx_scope* e, smplx_val* a, char* op) {
    LASSERT_NUM(op, a, 2);
    int r;
    if (strcmp(op, "==") == 0) {
        r = smplx_val_eq(a->cell[0], a->cell[1]);
    }
    if (strcmp(op, "!=") == 0) {
        r = !smplx_val_eq(a->cell[0], a->cell[1]);
    }
    smplx_val_del(a);
    return smplx_val_num(r);
}

smplx_val* builtin_eq(smplx_scope* e, smplx_val* a) {
    return builtin_cmp(e, a, "==");
}

smplx_val* builtin_ne(smplx_scope* e, smplx_val* a) {
    return builtin_cmp(e, a, "!=");
}

smplx_val* builtin_if(smplx_scope* e, smplx_val* a) {
    LASSERT_NUM("if", a, 3);
    LASSERT_TYPE("if", a, 0, LVAL_NUM);
    LASSERT_TYPE("if", a, 1, LVAL_QEXPR);
    LASSERT_TYPE("if", a, 2, LVAL_QEXPR);

    smplx_val* x;
    a->cell[1]->type = LVAL_SEXPR;
    a->cell[2]->type = LVAL_SEXPR;

    if (a->cell[0]->num) {
        x = smplx_val_eval(e, smplx_val_pop(a, 1));
    } else {
        x = smplx_val_eval(e, smplx_val_pop(a, 2));
    }

    smplx_val_del(a);
    return x;
}

smplx_val* smplx_val_read(mpc_ast_t* t);

////////////////////////////////////////////////////////////////////////////////////////////////////////

smplx_val* builtin_load(smplx_scope* e, smplx_val* a) {
    LASSERT_NUM("load", a, 1);
    LASSERT_TYPE("load", a, 0, LVAL_STR);

    /* Parse File given by string name */

    mpc_result_t r;
    if (mpc_parse_contents(a->cell[0]->str, Lispy, &r)) {

        smplx_val* expr = smplx_val_read(r.output);

        smplx_val* x = smplx_val_eval(e, expr);
        smplx_val_println(x);
        smplx_val_del(x);

        mpc_ast_delete(r.output);

        return smplx_val_sexpr();

    } else {
        /* Get Parse Error as String */
        char* err_msg = mpc_err_string(r.error);
        mpc_err_delete(r.error);

        /* Create new error message using it */
        smplx_val* err = smplx_val_err("Could not load Library %s", err_msg);
        free(err_msg);
        smplx_val_del(a);

        /* Cleanup and return error */
        return err;
    }

}




////////////////////////////////////////////////////////////////////////////////////////////////////////

smplx_val* builtin_scan(smplx_scope* e, smplx_val* a) {

    char input[1000];
    FILE *fptr;

    fptr = fopen("define.sx", "w");

    if (fptr == NULL) {
        printf("Error!");
    }


    fgets(input, 1000, stdin);
    fprintf(fptr, "(define [%s] %s)", a->cell[0]->str, input);
    fclose(fptr);


    mpc_result_t r;
    if (mpc_parse_contents("define.sx", Lispy, &r)) {

        smplx_val* expr = smplx_val_read(r.output);

        smplx_val* x = smplx_val_eval(e, expr);
        smplx_val_println(x);
        smplx_val_del(x);

        mpc_ast_delete(r.output);

        return smplx_val_sexpr();

    } else {
        /* Get Parse Error as String */
        char* err_msg = mpc_err_string(r.error);
        mpc_err_delete(r.error);

        /* Create new error message using it */
        smplx_val* err = smplx_val_err("Could not load Library %s", err_msg);
        free(err_msg);
        smplx_val_del(a);

        /* Cleanup and return error */
        return err;
    }

}

smplx_val* builtin_print(smplx_scope* e, smplx_val* a) {

    /* Print each argument followed by a space */
    for (int i = 0; i < a->count; i++) {
        smplx_val_print(a->cell[i]);
        putchar(' ');
    }

    /* Print a newline and delete arguments */
    putchar('\n');
    smplx_val_del(a);

    return smplx_val_sexpr();
}

smplx_val* builtin_error(smplx_scope* e, smplx_val* a) {
    LASSERT_NUM("error", a, 1);
    LASSERT_TYPE("error", a, 0, LVAL_STR);

    /* Construct Error from first argument */
    smplx_val* err = smplx_val_err(a->cell[0]->str);

    /* Delete arguments and return */
    smplx_val_del(a);
    return err;
}

void smplx_scope_add_builtin(smplx_scope* e, char* name, lbuiltin func) {
    smplx_val* k = smplx_val_sym(name);
    smplx_val* v = smplx_val_builtin(func);
    smplx_scope_put(e, k, v);
    smplx_val_del(k);
    smplx_val_del(v);
}

void smplx_scope_add_builtins(smplx_scope* e) {
    /* Variable Functions */
    smplx_scope_add_builtin(e, "@", builtin_lambda);
    smplx_scope_add_builtin(e, "define", builtin_def);
    smplx_scope_add_builtin(e, "let", builtin_put);

    /* List Functions */
    smplx_scope_add_builtin(e, "list", builtin_list);
    smplx_scope_add_builtin(e, "head", builtin_head);
    smplx_scope_add_builtin(e, "tail", builtin_tail);
    smplx_scope_add_builtin(e, "eval", builtin_eval);
    smplx_scope_add_builtin(e, "join", builtin_join);

    /* Mathematical Functions */
    smplx_scope_add_builtin(e, "+", builtin_add);
    smplx_scope_add_builtin(e, "-", builtin_sub);
    smplx_scope_add_builtin(e, "*", builtin_mul);
    smplx_scope_add_builtin(e, "/", builtin_div);

    /* Comparison Functions */
    smplx_scope_add_builtin(e, "if", builtin_if);
    smplx_scope_add_builtin(e, "==", builtin_eq);
    smplx_scope_add_builtin(e, "!=", builtin_ne);
    smplx_scope_add_builtin(e, ">", builtin_gt);
    smplx_scope_add_builtin(e, "<", builtin_lt);
    smplx_scope_add_builtin(e, ">=", builtin_ge);
    smplx_scope_add_builtin(e, "<=", builtin_le);

    /* String Functions */
    smplx_scope_add_builtin(e, "load", builtin_load);
    smplx_scope_add_builtin(e, "error", builtin_error);
    smplx_scope_add_builtin(e, "print", builtin_print);
   smplx_scope_add_builtin(e, "scan", builtin_scan);

}


#endif /* BUILT_IN_H */
