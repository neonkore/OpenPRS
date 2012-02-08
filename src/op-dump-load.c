static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * op-dump-load.c -- 
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

#include "config.h"

#include "constant.h"
#include "macro.h"

#ifdef OPRS_PROFILING
#include "user-external.h"
#endif

#include "oprs-profiling.h"

#ifndef NO_GRAPHIX
#include "xoprs-main.h"
#endif

#include "op-structure.h"
#include "relevant-op.h"
#include "oprs.h"
#include "oprs-dump.h"
#include "oprs-pred-func.h"
#include "database.h"
#include "oprs-error.h"

#include "op-structure_f.h"
#include "op-compiler_f.h"
#include "relevant-op_f.h"
#include "oprs-type_f.h"
#include "oprs-print.h"
#include "oprs-print_f.h"
#include "oprs-profiling_f.h"
#include "oprs-dump_f.h"
#include "oprs-error_f.h"
#include "parser-funct_f.h"


void clear_op_database(Relevant_Op *rk, PBoolean graphic)
{
#ifndef NO_GRAPHIX
     if (graphic) {
	  clear_op_graphic(global_draw_data);
     }
#endif
     clear_relevant_op(rk);
}

void dump_op_file(Relevant_Op *rk, PString internal_file_name, PString output_file_name)
{
     if (start_dump_session(output_file_name) < 0) {
	  fprintf(stderr, LG_STR("ERROR: dump_op_database_from_parser: failed start_dump_session: %s\n",
				 "ERREUR: dump_op_database_from_parser: echec start_dump_session: %s\n"), output_file_name);
     } else {
	  Op_List ops = sl_make_slist();
	  short i = 0;
	  Op_Structure *op;

	  printf(LG_STR("Dumping %s:",
			"Dump de %s:"),output_file_name);
	  
	  sl_loop_through_slist(rk->op_list, op, Op_Structure *) {
	       if (equal_op_fname(internal_file_name,op)) {
		    sl_add_to_tail(ops, op);
		    
		    if (i++ == MAX_OP_PER_DUMP) {
			 i = 0;
			 dump_list_op(ops);
			 dump_object_list();
			 flush_dump_session();
			 FLUSH_SLIST(ops);
		    }
	       }
	  }
	  dump_list_op(ops);
	  dump_object_list();
	  end_dump_session();
	  FREE_SLIST(ops);

	  printf(LG_STR("\nDumping %s done.\n",
			"\nDump de %s terminé.\n"),output_file_name);
     }
}

void dump_all_opfs(Relevant_Op *rk)
{   
     PString opf;
     PString output_file_name = NULL;

     sl_loop_through_slist(rk->opf_list, opf, PString) {
	  output_file_name = compilable_opf(opf);

	  if (output_file_name) {
	       dump_op_file(rk, opf, output_file_name);
	       FREE(output_file_name);
	  }
     }
}

void dump_op_database_from_parser(Relevant_Op *rk, PString file_name)
{
     if (start_dump_session(file_name) < 0) {
	  fprintf(stderr, LG_STR("ERROR: dump_op_database_from_parser: failed start_dump_session: %s\n",
				 "ERREUR: dump_op_database_from_parser: echec start_dump_session: %s\n"), file_name);
     } else {
	  Op_List ops = sl_make_slist();
	  short i = 0;
	  Op_Structure *op;

	  sl_loop_through_slist(rk->op_list, op, Op_Structure *) {
	       sl_add_to_tail(ops, op);
	       
	       if (i++ == MAX_OP_PER_DUMP) {
		    i = 0;
		    dump_list_op(ops);
		    dump_object_list();
		    flush_dump_session();
		    FLUSH_SLIST(ops);
	       }
	  }

	  dump_list_op(ops);
	  dump_object_list();
	  end_dump_session();
	  FREE_SLIST(ops);
     }
}

void load_op_database_from_parser(Relevant_Op *rk, PString file_name)
{
     if (start_load_session(file_name) < 0) {
	  fprintf(stderr, LG_STR("ERROR: load_op_database_from_parser: failed start_load_session: %s\n",
				 "ERREUR: load_op_database_from_parser: echec start_load_session: %s\n"), file_name);
     } else {
	  PBoolean res;

	  do {
	       Op_Structure *op;
	       Op_List ops;

	       ops = load_list_op();

	       if ((res = load_all_objects()))
		    flush_load_session();
	       else
		    end_load_session();
						  
	       sl_loop_through_slist(ops, op, Op_Structure *)
		    add_op_to_relevant_op_internal(op,rk);

	       FREE_SLIST(ops);
	  } while (res);
     }
}
