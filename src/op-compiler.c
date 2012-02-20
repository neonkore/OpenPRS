/*                               -*- Mode: C -*- 
 * op-compiler.c -- Compiler de OP.
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

#include "config.h"

/*
 * I must admit that most of the code for the OP compiler is currently in op-structure. After the compilation is
 * for now just the filling of the op-structure slots... The checking is quite weak but it will do for this release.
 *
 */

#include "constant.h"

#ifdef GRAPHIX
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#endif


#include "op-structure.h"
#include "relevant-op.h"
#include "op-compiler.h"
#include "action.h"
#include "oprs.h"
#include "lang.h"
#include "database.h"
#include "oprs-pred-func.h"

#include "op-structure_f.h"
#include "relevant-op_f.h"
#include "op-compiler_f.h"
#include "oprs-pred-func_f.h"


void add_op_to_relevant_op(Op_Structure *op, Relevant_Op *rk)
{
     add_op_to_relevant_op_internal(op,rk);

     if (debug_trace[OP_COMPILER]) {
	  print_op(op);
	  printf("\n");
     }
}


PBoolean check_ep_in_ip(Op_Structure *op, Pred_Func_Rec *pfr)
{
     if (PFR_EP(pfr)) {
	  fprintf(stderr,LG_STR("ERROR: Invocation Part of %s is invalid!\n\t\
Do not use evaluable predicate %s in invocation part\n\t\
They will not trigger OP application.\n",
				"ERREUR: La partie d'invocation de %s est invalide !\n\t\
N'utilisez pas de prédicat évaluable (en l'occurence %s) dans les parties d'invocation,\n\t\
il ne déclencheront pas l'application du OP.\n"), op->name, pfr->name);
	  return FALSE;
     } else
	  return TRUE;
}

