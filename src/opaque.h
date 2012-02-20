/*                               -*- Mode: C -*- 
 * opaque.h -- Opaque declarations
 * 
 * $Id$
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

#ifndef INCLUDE_opaque
#define INCLUDE_opaque

#include "opaque-pub.h"

typedef struct frame *FramePtr;

typedef Slist *ExprList;		/* Les elements sont des Expression */
typedef Slist *ExprFrameList;	/* les elements sont des pairs, expression, frame */
typedef Slist *ExprListFrameList; /* les elements sont des pairs, list d'expression, frame */
typedef Slist *FrameList;
typedef Slist *TermTypeList;

typedef struct draw_data Draw_Data;
typedef struct int_draw_data Int_Draw_Data;
typedef struct og OG;
typedef Slist *List_OG;
typedef struct iog IOG;
typedef Slist *List_IOG;

typedef struct op_structure Op_Structure;

typedef struct control_point Control_Point;
typedef struct control_point Node;

typedef struct logic Logic;
typedef struct logic Edge;

typedef Slist *Subst_List;
typedef Slist *Op_List;
typedef Slist *Op_Instance_List;


typedef Slist *Condition_List;

typedef Slist *Op_Expr_List;

typedef Slist *CpList;		/*Control point list */
typedef Slist *NodeList;

typedef Slist *EdgeList;
typedef Slist *LogicList;

typedef Slist *Fact_List;
typedef Slist *Goal_List;

typedef Slist *BE_Clean_List;

typedef Slist *ListLines;

typedef struct oprs_client Oprs_Client;

typedef Slist *Thread_Intention_Block_List;

typedef Slist *Intention_Paire_List;	/* List de paire d'intentions */

/* Pointer on function returning int... */
typedef int (*PFI)();		

/* Pointer on function returning void... */
typedef void (*PFV)();		

/* Pointer on function returning pointer on void... */
typedef void * (*PFPV)();		

/* Pointer on function returning OPRS_NODE... */
/* typedef OPRS_NODE (*PFN)();		 */

/* Pointer on function returning Intention_List. */
typedef Intention_List (*PFIL)();

/* Pointer on function returning ListLines. */
typedef ListLines (*PFLL)();

typedef struct database Database;

typedef struct relevant_op Relevant_Op;

typedef struct body Body;

typedef struct key_elt Key_Elt;

typedef struct key Key;

typedef Slist *SymList;

typedef struct type Type;

typedef Slist *TypeList;

#endif /* INCLUDE_opaque */
