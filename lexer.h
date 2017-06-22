/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   lexer.h
 * Author: root
 *
 * Created on June 15, 2017, 6:54 AM
 */

#ifndef LEXER_H
#define LEXER_H


/* Reading */

smplx_val* smplx_val_read_num(mpc_ast_t* t) {
    errno = 0;
    double x = atof(t->contents);
    return errno != ERANGE ? smplx_val_num(x) : smplx_val_err("Invalid Number.");
}

smplx_val* smplx_val_read_str(mpc_ast_t* t) {
    /* Cut off the final quote character */
    t->contents[strlen(t->contents) - 1] = '\0';
    /* Copy the string missing out the first quote character */
    char* unescaped = malloc(strlen(t->contents + 1) + 1);
    strcpy(unescaped, t->contents + 1);
    /* Pass through the unescape function */
    unescaped = mpcf_unescape(unescaped);
    /* Construct a new smplx_val using the string */
    smplx_val* str = smplx_val_str(unescaped);
    /* Free the string and return */
    free(unescaped);
    return str;
}


/**
 * 
 *  Takes the parsed code from mpc and starts evaluating it according to its content
 *  
 *  - Number
 *  - Symbolic Expression
 *  - Qouted Expression
 * 
 * @param t
 * @return 
 */
smplx_val* smplx_val_read(mpc_ast_t* t) {

    if (strstr(t->tag, "number")) {
        return smplx_val_read_num(t);
    }
    if (strstr(t->tag, "string")) {
        return smplx_val_read_str(t);
    }
    if (strstr(t->tag, "symbol")) {
        return smplx_val_sym(t->contents);
    }

    smplx_val* x = NULL;
    if (strcmp(t->tag, ">") == 0) {
        x = smplx_val_sexpr();
    }
    if (strstr(t->tag, "sexpr")) {
        x = smplx_val_sexpr();
    }
    if (strstr(t->tag, "qexpr")) {
        x = smplx_val_qexpr();
    }

    for (int i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(") == 0) {
            continue;
        }
        if (strcmp(t->children[i]->contents, ")") == 0) {
            continue;
        }
        if (strcmp(t->children[i]->contents, "]") == 0) {
            continue;
        }
        if (strcmp(t->children[i]->contents, "[") == 0) {
            continue;
        }
        if (strcmp(t->children[i]->tag, "regex") == 0) {
            continue;
        }
        if (strstr(t->children[i]->tag, "comment")) {
            continue;
        }
        x = smplx_val_add(x, smplx_val_read(t->children[i]));
    }

    return x;
}


#endif /* LEXER_H */

