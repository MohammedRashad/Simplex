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

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

/* Lisp Environment */

struct smplx_scope {
    smplx_scope* par;
    int count;
    char** syms;
    smplx_val** vals;
};

smplx_scope* smplx_scope_new(void) {
    smplx_scope* e = malloc(sizeof (smplx_scope));
    e->par = NULL;
    e->count = 0;
    e->syms = NULL;
    e->vals = NULL;
    return e;
}

void smplx_scope_del(smplx_scope* e) {
    for (int i = 0; i < e->count; i++) {
        free(e->syms[i]);
        smplx_val_del(e->vals[i]);
    }
    free(e->syms);
    free(e->vals);
    free(e);
}

smplx_scope* smplx_scope_copy(smplx_scope* e) {
    smplx_scope* n = malloc(sizeof (smplx_scope));
    n->par = e->par;
    n->count = e->count;
    n->syms = malloc(sizeof (char*) * n->count);
    n->vals = malloc(sizeof (smplx_val*) * n->count);
    for (int i = 0; i < e->count; i++) {
        n->syms[i] = malloc(strlen(e->syms[i]) + 1);
        strcpy(n->syms[i], e->syms[i]);
        n->vals[i] = smplx_val_copy(e->vals[i]);
    }
    return n;
}

smplx_val* smplx_scope_get(smplx_scope* e, smplx_val* k) {

    for (int i = 0; i < e->count; i++) {
        if (strcmp(e->syms[i], k->sym) == 0) {
            return smplx_val_copy(e->vals[i]);
        }
    }

    if (e->par) {
        return smplx_scope_get(e->par, k);
    } else {
        return smplx_val_err("Unbound Symbol '%s'", k->sym);
    }
}

void smplx_scope_put(smplx_scope* e, smplx_val* k, smplx_val* v) {

    for (int i = 0; i < e->count; i++) {
        if (strcmp(e->syms[i], k->sym) == 0) {
            smplx_val_del(e->vals[i]);
            e->vals[i] = smplx_val_copy(v);
            return;
        }
    }

    e->count++;
    e->vals = realloc(e->vals, sizeof (smplx_val*) * e->count);
    e->syms = realloc(e->syms, sizeof (char*) * e->count);
    e->vals[e->count - 1] = smplx_val_copy(v);
    e->syms[e->count - 1] = malloc(strlen(k->sym) + 1);
    strcpy(e->syms[e->count - 1], k->sym);
}

void smplx_scope_def(smplx_scope* e, smplx_val* k, smplx_val* v) {
    while (e->par) {
        e = e->par;
    }
    smplx_scope_put(e, k, v);
}


#endif /* ENVIRONMENT_H */

