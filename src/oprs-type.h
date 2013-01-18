/*                               -*- Mode: C -*- 
 * oprs-type.h -- Types utilises par OPRS
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

#ifndef INCLUDE_oprs_type
#define INCLUDE_oprs_type

#ifdef VXWORKS
#include "vxWorks.h"
#include "stdioLib.h"
#include "semLib.h"
#else
#include <stdio.h>
#endif

#include "slistPack.h"
#include <shashPack.h>

#include "lisp-list.h"
#include "opaque.h"
#include "macro.h"

#include "oprs-type-pub.h"
#include "other_proto.h"
 
/* type de Variable */
typedef enum {LOGICAL_VARIABLE, PROGRAM_VARIABLE} Variable_Type;

/* type d'operateur temporel pour les buts. The order is very important, 
cached information depend on it.*/
typedef enum {ACHIEVE = 1, CALL, TEST, ACT_MAINT, PASS_MAINT, WAIT} Temporal_Ope;

typedef enum {DB_CONC = 1, DB_RET, DB_COND_CONC, DB_COND_RET} DB_Ope;


/* quelques list */

typedef Slist *PropertyList;

typedef Slist *List_Envar;	/* Les elements sont des Envar */

typedef Slist *List_Undo;	/* Les elements sont des Undo */

struct u_memory {
     int size;
     void *mem;
};

typedef struct action_field {
     Expression *function;
     union {
	  List_Envar list_envar;
	  Envar *envar;
     } u;
     unsigned int special BITFIELDS(:1);
     unsigned int multi BITFIELDS(:1);
} Action_Field;

typedef struct property Property;

struct property {
     PString name;
     Term *value;
};

/* Definition d'un environement de binding */

typedef struct frame {		/* Un Frame c'est quoi? */
     List_Undo  list_undo;	/* Une list de undo slot pour pouvoir le reinstaller ou le desinstaller */
     FramePtr previous_frame;	/* Un pointeur sur le precedent frame */
     PBoolean installe;		/* Un flag pour savoir s'il est installe */
} Frame;

typedef struct exprframe {	/* Une paire Expression . Frame */
     Expression *expr;
     Frame *frame;
} ExprFrame;

typedef struct exprlistframe {	/* Une paire ExpressionList . Frame */
     ExprList exprl;
     Frame *frame;
} ExprListFrame;

struct envar {			/* Un envar */
     Symbol name;		/* son name */
     Term *value;		/* le term sur lequel elle pointe */
     Type *unif_type;		
     Variable_Type type BITFIELDS(:8);	/* Le type de la variable */
};

typedef struct undo {		/* Un undo */
     Envar *envar;		/* l'envar */
     Term *value;		/* et sa value */
} Undo;

struct int_array {
     int *array;
     unsigned short size;
};


struct float_array {
     double *array;
     unsigned short size;
};

#ifdef IGNORE
typedef struct add_del_list {
     ExprList add;
     ExprList del;
     ExprList cond_add;		/* Pairs condition expr (cond1 expr1 cond2 expr2 cond3 expr3) */
     ExprList cond_del;
} Add_Del_List;
#endif

typedef ExprList Add_Del_List;

extern const FramePtr empty_env; /* L'environement vide (initialise dans unification) */

extern List_Envar current_var_list; /* Current var list when the parser swallow expr with variables */

extern Slist *empty_list;	/* This is a constant... do not use it to start a list or I kill you */

extern Symbol object_sym;
extern Symbol format_sym;
extern Symbol current_sym;
extern Symbol quote_sym;
extern Symbol decision_procedure_sym;
extern Symbol not_sym;
extern Symbol lisp_t_sym;
extern Symbol spec_act_sym;
extern Symbol wait_sym;
extern Symbol wait_son_sym;
extern Symbol soak_sym;
extern Symbol app_ops_fact_sym;
extern Symbol fact_invoked_ops_sym;
extern Symbol db_satisf_goal_sym;
extern Symbol app_ops_goal_sym;
extern Symbol goal_invoked_ops_sym;
extern Symbol failed_goal_sym;
extern Symbol failed_sym;
extern Symbol request_sym;
extern Symbol achieved_sym;
extern Symbol wake_up_intention_sym;
extern Symbol read_response_id_sym;
extern Symbol read_response_sym;

extern Symbol or_sym;
extern Symbol and_sym;
extern Symbol not_sym;
extern Symbol cond_add_sym;
extern Symbol cond_del_sym;
extern Symbol temp_conclude_sym;
extern Symbol retract_sym;
extern Symbol achieve_sym;
extern Symbol test_sym;
extern Symbol preserve_sym;
extern Symbol twait_sym;
extern Symbol maintain_sym;
extern Symbol call_sym;

extern char *oprs_prompt;

extern List_Envar global_var_list;

extern Shash *id_hash;
#ifdef VXWORKS
extern SEM_ID id_hash_sem;
#endif     

extern Shash *pred_hash;

extern Shash *func_hash;

extern PBoolean check_pfr;

/* The MAX value must be the last one... */
typedef enum {POST_GOAL, POST_FACT, RECEIVE_MESSAGE, SEND_MESSAGE, DATABASE, DATABASE_KEY,
	      DATABASE_FRAME, CONCLUDE_FROM_PARSER, INTENTION_FAILURE, INT_FAIL_BIND, SUC_FAIL,
	      GRAPHIC_INTEND, INTEND, OP_COMPILER, RELEVANT_OP, SOAK, GRAPHIC_OP, TEXT_OP,
	      THREADING,
	      MAX_DEBUG} Debug_Trace;

extern PBoolean debug_trace[MAX_DEBUG];

typedef enum {USER_TRACE, UT_POST_GOAL, UT_POST_FACT, UT_RECEIVE_MESSAGE, UT_SEND_MESSAGE,
	      /* UT_DATABASE, UT_INTENTION_FAILURE, */ UT_SUC_FAIL, UT_INTEND,
	      UT_RELEVANT_OP, UT_SOAK, UT_OP, UT_THREADING, 
	      MAX_USER_TRACE} User_Trace;

extern PBoolean user_trace[MAX_USER_TRACE];

typedef enum {META_LEVEL, SOAK_MF, FACT_INV, GOAL_INV, APP_OPS_FACT, APP_OPS_GOAL,
	      MAX_META_OPTION} Meta_Option; 

extern PBoolean meta_option[MAX_META_OPTION];

#ifdef OPRS_PROFILING
typedef enum {PROFILING, 
	      PROF_DB,
	      PROF_ACTION_OP,	/* Action OP Execution */
	      PROF_OP, 
	      PROF_EP,		/* Evaluable Predicate Execution */
	      PROF_EF,		/* Evaluable Function Execution */
	      PROF_AC,		/* Action Execution */
	      MAX_PROFILING_OPTION} Profiling_Option;

extern PBoolean profiling_option[MAX_PROFILING_OPTION];
#endif

typedef enum {EVAL_ON_POST, PAR_POST, PAR_INTEND, PAR_EXEC, MAX_RUN_OPTION} Run_Option;

extern PBoolean run_option[MAX_RUN_OPTION];

typedef enum {CHECK_SYMBOL, CHECK_PFR, CHECK_ACTION, MAX_COMPILER_OPTION} Compiler_Option;

extern PBoolean compiler_option[MAX_COMPILER_OPTION];

extern PBoolean allow_variable_parsing;

extern PBoolean check_symbol;

extern PBoolean lower_case_id;
extern PBoolean no_case_id;

extern PBoolean check_all_vars_in_subst_lenv;

extern PBoolean pretty_print;
extern PBoolean print_backslash;
extern PBoolean pretty_fill;
extern PBoolean print_var_name;
extern PBoolean print_english_operator;
extern PBoolean replace_cr;

extern int print_length;	   

extern PBoolean be_quiet;
extern PBoolean register_to_server;
extern PBoolean register_to_mp;
extern List_Envar locked_local_var;
extern List_Envar copy_var_uniquely;

#ifdef SPRINT_RETURN_STRING
#define NUM_CHAR_SPRINT(x) (strlen((char *)(x))) /* I have to cast to (char *) for the DEC Station */
#else
#define NUM_CHAR_SPRINT(x) (x)
#endif

#include "lang.h"

/* Type of object to draw */
typedef enum {
     DT_NODE, DT_IF_NODE, DT_THEN_NODE, DT_ELSE_NODE, DT_EDGE, DT_EDGE_TEXT, 
     DT_TEXT, DT_KNOT, DT_INST}  Draw_Type; 

typedef enum {INODE, IEDGE} IDraw_Type;

typedef enum {QET_NONE, QET_QUOTE, QET_EVAL} Quote_Eval_Type;

#endif /* INCLUDE_oprs_type */
