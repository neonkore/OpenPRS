/*                               -*- Mode: C -*- 
 * opaque-pub.h -- Public Opaque declarations...
 * 
 * Copyright (c) 1991-2013 Francois Felix Ingrand.
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

#ifdef  __cplusplus
extern "C"  {
#endif

#ifndef INCLUDE_opaque_pub
#define INCLUDE_opaque_pub

#include "slistPack.h"
#include "slistPack_f.h"
#include "shashPack.h"

#include "oprs-sprint-pub.h"

/* 
 * An opaque pointer is, most of the time, a pointer to a structure not 
 * yet defined. AS slong as you do not dereference ce structure, 
 * that's OK, the compiler is able to handle this opaque pointer.
 * 
 * The opaque pointer defined in this file are required to compile 
 * user-evaluable-* modules. But the definition of the structure to 
 * which they are pointing is not required (and may be changed in 
 * future release).
 */

typedef char PBoolean;

typedef unsigned char Uchar;

typedef const char *Symbol;	/* Symbol are "fixed" pointers. */

typedef struct pred_rec Pred_Rec;
typedef struct func_rec Func_Rec;

typedef char *PString;

typedef Slist *TermList;	       
typedef Slist *Intention_List;	       
typedef struct term Term;
typedef struct term *TermPtr;
typedef TermPtr (*PFPT)(TermList); /* PFPT is pointer to a function taking a TermList and returning a
				      pointer to a Term... I love C decl.. */

typedef Slist *(*PFPL)(Slist *); /* PFPL is pointer to a function taking a Slist *and returning a
				      pointer to Slist *.*/

/* Pointer on function returning void... */
typedef void (*PFV)();		

typedef void entrypoint(void);
typedef void entrypoint_name(char *);

typedef struct expression Expression;

typedef Slist *VarList;

typedef struct envar Envar;

typedef struct l_list *L_List;

typedef struct int_array Int_array;
typedef struct float_array Float_array;

typedef struct eval_pred Eval_Pred;

typedef struct eval_funct Eval_Funct;

typedef struct eval_funct Action;

/*  pointer to a function returning a PBoolean */
typedef PBoolean (*PFB)(); 
/*  pointer to a function returning a PBoolean, taking  TermList*/
typedef PBoolean (*PFBPTL)(TermList);

typedef struct oprs Oprs;
typedef struct thread_intention_block Thread_Intention_Block;
typedef struct intention Intention;
typedef struct op_instance Op_Instance;
typedef struct intention_graph Intention_Graph;

typedef struct fact Fact;
typedef struct goal Goal;
typedef struct u_memory U_Memory;

#endif /* INCLUDE_opaque_pub */

#ifdef __cplusplus
}
#endif
