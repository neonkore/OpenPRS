/*                               -*- Mode: C -*- 
 * oprsdum-pub.h -- 
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

#ifndef INCLUDE_oprs_dump
#define INCLUDE_oprs_dump

typedef enum {DPT_END,
	      DPT_ACTION_FIELD,
	      DPT_BODY,
	      DPT_COND,
	      DPT_EDGE,
	      DPT_ENVAR,
	      DPT_EXPR,
	      DPT_FACT,
	      DPT_FLOAT_ARRAY,
	      DPT_FLUSH,
	      DPT_FRAME,
	      DPT_GOAL,
	      DPT_IG,
	      DPT_INSTRUCTION,
	      DPT_INTENTION,
	      DPT_INT_ARRAY,
	      DPT_IOG,
	      DPT_LENV,
	      DPT_LIST_EDGE,
	      DPT_LIST_EXPR,
	      DPT_LIST_NODE,
	      DPT_LIST_OP,
	      DPT_LIST_PROPERTY,
	      DPT_LIST_SYMBOL,
	      DPT_L_LIST,
	      DPT_NODE,
	      DPT_OG,
	      DPT_OP,
	      DPT_OP_INSTANCE,
	      DPT_PRED_FUNC_REC,
	      DPT_PROPERTY,
	      DPT_SYMBOL,
	      DPT_TERM,
	      DPT_TIB,
	      DPT_TL,
	      DPT_TYPE,
	      DPT_UNDO,
	      DPT_UNDO_LIST,
	      DPT_U_MEMORY,
	      DPT_VAR_LIST,
	      DPT_MAX
} Dump_Type;

extern char *dump_type_name[DPT_MAX];

extern TypeList type_conversion;

typedef struct object_dump {
     void *addr;		/* Its adress. */
     unsigned short obj_refcount;
     Dump_Type type;		/* Type of the object. */
} Object_Dump;


typedef struct object_reloc {
     void *old_addr;
     union {
	  Slist *addr_ref_list;	/* List of adresses pointing to it, if not yet loaded.*/
	  void *new_addr;	/* Its new adress, if already loaded. */
     } u;
     unsigned short refcount;
     Dump_Type type;		/* Type of the object. */
     PBoolean loaded;		/* Has it been loaded yet? */
} Object_Reloc;

extern int dump_format_version;

#define DUMP_LOAD_TABLE_SIZE 8192
#define MAX_OP_PER_DUMP 20

#endif
