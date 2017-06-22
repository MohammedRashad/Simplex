/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   evaluate.h
 * Author: root
 *
 * Created on June 15, 2017, 6:53 AM
 */

#ifndef EVALUATE_H
#define EVALUATE_H

/* Evaluation */

smplx_val* smplx_val_call(smplx_scope* e, smplx_val* f, smplx_val* a) {

    if (f->builtin) {
        return f->builtin(e, a);
    }

    int given = a->count;
    int total = f->formals->count;

    while (a->count) {

        if (f->formals->count == 0) {
            smplx_val_del(a);
            return smplx_val_err("Function passed too many arguments. "
                    "Got %i, Expected %i.", given, total);
        }

        smplx_val* sym = smplx_val_pop(f->formals, 0);

        if (strcmp(sym->sym, "&") == 0) {

            if (f->formals->count != 1) {
                smplx_val_del(a);
                return smplx_val_err("Function format invalid. "
                        "Symbol '&' not followed by single symbol.");
            }

            smplx_val* nsym = smplx_val_pop(f->formals, 0);
            smplx_scope_put(f->env, nsym, builtin_list(e, a));
            smplx_val_del(sym);
            smplx_val_del(nsym);
            break;
        }

        smplx_val* val = smplx_val_pop(a, 0);
        smplx_scope_put(f->env, sym, val);
        smplx_val_del(sym);
        smplx_val_del(val);
    }

    smplx_val_del(a);

    if (f->formals->count > 0 && strcmp(f->formals->cell[0]->sym, "&") == 0) {

        if (f->formals->count != 2) {
            return smplx_val_err("Function format invalid. "
                    "Symbol '&' not followed by single symbol.");
        }

        smplx_val_del(smplx_val_pop(f->formals, 0));

        smplx_val* sym = smplx_val_pop(f->formals, 0);
        smplx_val* val = smplx_val_qexpr();
        smplx_scope_put(f->env, sym, val);
        smplx_val_del(sym);
        smplx_val_del(val);
    }

    if (f->formals->count == 0) {
        f->env->par = e;
        return builtin_eval(f->env, smplx_val_add(smplx_val_sexpr(), smplx_val_copy(f->body)));
    } else {
        return smplx_val_copy(f);
    }

}

smplx_val* smplx_val_eval_sexpr(smplx_scope* e, smplx_val* v) {

    for (int i = 0; i < v->count; i++) {
        v->cell[i] = smplx_val_eval(e, v->cell[i]);
    }
    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type == LVAL_ERR) {
            return smplx_val_take(v, i);
        }
    }

    if (v->count == 0) {
        return v;
    }
    if (v->count == 1) {
        return smplx_val_eval(e, smplx_val_take(v, 0));
    }

    smplx_val* f = smplx_val_pop(v, 0);
    if (f->type != LVAL_FUN) {
        smplx_val* err = smplx_val_err(
                "S-Expression starts with incorrect type. "
                "Got %s, Expected %s.",
                ltype_name(f->type), ltype_name(LVAL_FUN));
        smplx_val_del(f);
        smplx_val_del(v);
        return err;
    }

    smplx_val* result = smplx_val_call(e, f, v);
    smplx_val_del(f);
    return result;
}

smplx_val* smplx_val_eval(smplx_scope* e, smplx_val* v) {
    if (v->type == LVAL_SYM) {
        smplx_val* x = smplx_scope_get(e, v);
        smplx_val_del(v);
        return x;
    }
    if (v->type == LVAL_SEXPR) {
        return smplx_val_eval_sexpr(e, v);
    }
    return v;
}


#endif /* EVALUATE_H */

