/*                               -*- Mode: C -*- 
 * oprs-type-pub.h -- Public OPRS type.
 * 
 * $Id$
 * 
 * Copyright (c) 1991-2003 Francois Felix Ingrand.
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

#ifndef INCLUDE_oprs_type_pub
#define INCLUDE_oprs_type_pub

#include "macro-pub.h"
#include "opaque-pub.h"

/*  
 *  This file contains public definitions of various structures used in 
 *  OPRS. This file should not be modified in any way. The user 
 *  should not even assume that the real definitions are the same than 
 *  the one defined here.
 */


 /* Definition of the types used in a Term */
typedef enum {INTEGER, TT_FLOAT, STRING, TT_ATOM, EXPRESSION, VARIABLE,
	      LISP_LIST, INT_ARRAY, FLOAT_ARRAY, C_LIST, TT_FACT, TT_GOAL, 
	      TT_INTENTION, TT_OP_INSTANCE, U_POINTER, U_MEMORY} Term_Type;

typedef enum {ETOT_ACHIEVE, ETOT_TEST, ETOT_CALL, ETOT_CONCLUDE, ETOT_RETRACT} External_Temporal_Operator_Type;


/* These two symbols are defined by Windows 95... */
#ifndef WIN95			
#define ATOM TT_ATOM
#define FLOAT TT_FLOAT
#endif

/* typedef enum {PF_UNKNOWN, PF_PRED, PF_EF, PF_ACT, PF_ENVAR} Pred_Func_Rec_Type; */

typedef struct pred_func_rec {
     Symbol name;		/* If NULL, it is a variable. */
     union {
	  Envar *envar;
	  struct {
	       Eval_Funct *ef;
	       Action *act;
	       Pred_Rec *pr;
	  }u;
     }u;
} Pred_Func_Rec;

struct expression {
     Pred_Func_Rec *pfr;
     TermList terms;
};

/* Definition of a term (a typed union) */
struct term {
     union {
	  int 	intval;
	  double *doubleptr;
	  char *string;
	  char *id;
	  Expression *expr;
	  Envar *var;
	  L_List l_list;
	  Op_Instance *opi;
	  Intention *in;
	  Fact *fact;
	  Goal *goal;
	  Int_array *int_array;
	  Float_array *float_array;
	  void *u_pointer;
	  U_Memory *u_memory;
	  Slist *c_list;		
     }u;
     Term_Type type BITFIELDS(:8); 
};

extern const L_List l_nil;	/* The lisp nil constant. */

extern Symbol lisp_t_sym;	/* The lisp t symbol. */

extern Symbol nil_sym;

extern Oprs *current_oprs;

extern Thread_Intention_Block *current_tib;

extern PBoolean print_var_value;

extern char *print_float_format;

typedef struct timeval PDate;

#ifdef WIN95
typedef unsigned char   u_char;
#endif

#endif /* INCLUDE_oprs_type_pub */

#ifdef __cplusplus
}
#endif
