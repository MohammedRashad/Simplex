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

#include  "Simplex.h"

int zstring_search_chr(const char *token,char s){
    if (!token || s=='\0')
        return 0;

    for (;*token; token++)
        if (*token == s)
            return 1;

    return 0;
}


char *zstring_remove_chr(char *str,const char *bad) {
    char *src = str , *dst = str;
    while(*src)
        if(zstring_search_chr(bad,*src))
            src++;
        else
            *dst++ = *src++;  /* assign first, then incement */

    *dst='\0';
        return str;
}

 

int main(int argc, char** argv) {

    char input[4096];

    Number = mpc_new("number");
    Symbol = mpc_new("symbol");
    String = mpc_new("string");
    Comment = mpc_new("comment");
    Sexpr = mpc_new("sexpr");
    Qexpr = mpc_new("qexpr");
    Expr = mpc_new("expr");
    Lispy = mpc_new("simplex");

    mpca_lang(MPCA_LANG_DEFAULT,
            "                                       \
      number  : /-?[0-9]+(\\.[0-9]+)?/ ;            \
      symbol  : /[a-zA-Z0-9_+\\-*\\/\\\\@=<>!&]+/ ; \
      string  : /\"(\\\\.|[^\"])*\"/ ;              \
      comment : /#[^\\r\\n]*/ ;                     \
      sexpr   : '(' <expr>* ')' | <comment>;                   \
      qexpr   : '[' <expr>* ']' ;                   \
      expr    : <number>  | <symbol> | <string>     \
              | <comment> | <sexpr>  | <qexpr>;     \
      simplex   : /^/ <expr>* /$/ ;                 \
    ",
            Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Lispy);

    smplx_scope* e = smplx_scope_new();
    smplx_scope_add_builtins(e);

    /* Interactive Mode */
    if (argc == 1) {

        puts("Simplex BETA (V0.1) -- Interactive Mode");
        puts("Press Ctrl+c to Exit\n");

        while (1) {

            printf("Simplex > ");

            fgets(input, 4096, stdin);

            mpc_result_t r;
            if (mpc_parse("<stdin>", zstring_remove_chr(input,";"), Lispy, &r)) {
                
                smplx_val* expr = smplx_val_read(r.output);

                smplx_val* x = smplx_val_eval(e, expr);
                smplx_val_println(x);
                smplx_val_del(x);

                mpc_ast_delete(r.output);
            } else {
                mpc_err_print(r.error);
                mpc_err_delete(r.error);
            }


        }
    }

    /* Batch Mode */
    if (argc >= 2) {

        /* loop over each supplied filename (starting from 1) */
        for (int i = 1; i < argc; i++) {

            /* Argument list with a single argument, the filename */
            smplx_val* args = smplx_val_add(smplx_val_sexpr(), smplx_val_str(argv[i]));

            /* Pass to builtin load and get the result */
            smplx_val* x = builtin_load(e, args);

            /* If the result is an error be sure to print it */
            if (x->type == LVAL_ERR) {
                smplx_val_println(x);
            }
            smplx_val_del(x);
        }
    }

    /* Cleaning up resources */
    smplx_scope_del(e);

    mpc_cleanup(8, Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Lispy);

    return 0;
}