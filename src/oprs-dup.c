/*                               -*- Mode: C -*- 
 * oprs-dup.c -- Most of the dup functions.
 * 
 * Copyright (c) 1991-2011 Francois Felix Ingrand.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */



#include "oprs-type.h"
#include "oprs-error.h"


/* dup function... */

Frame *dup_frame(Frame *fp)
{
     if (fp != empty_env) {
	  DUP(fp);
     }
     return fp;
}

List_Envar dup_lenv(List_Envar le)
{
     if (le != empty_list)
	  DUP_SLIST(le);

     return le;
}

Expression *dup_expr(Expression *expr)
{
     DUP(expr);
     return expr;
}
     
TermList dup_terms(TermList terms)
{
     DUP_SLIST(terms);
     return terms;
}

Term *dup_term(Term *term)
{
     if (term == NULL) return NULL; /* Sometimes we have a null term pointed by an unbound variable */
     else DUP(term);		/* Add a reference count. */

     return term;

}

VarList dup_vars_list(VarList vars)
{
     DUP_SLIST(vars);
     return vars;
}

Envar * dup_envar(Envar * ev)
{
     DUP(ev);
     return ev;
}
