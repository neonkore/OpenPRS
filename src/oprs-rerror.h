/*                               -*- Mode: C -*- 
 * oprs-rerror.h -- 
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


#ifndef INCLUDE_oprs_rerror
#define INCLUDE_oprs_rerror

#include <setjmp.h>

typedef enum {ET_NONE, EVAL_FUNCTION, ACTION, EVAL_PREDICATE, INVALID_BINDING} Error_Type;

typedef enum {EC_NONE, EC_GOAL_POSTING, EC_CONSULT_DATABASE, EC_EXECUTE_INTENTION, EC_CHECK_STDIN,
	      EC_FIND_SOAK_GOAL, EC_FIND_SOAK_FACT, EC_EXECUTE_ACTION} Error_Ctxt;

extern Symbol current_eval_funct;
extern Symbol current_eval_pred;
extern Symbol current_ac;

extern Error_Ctxt current_error_ctxt;

extern Error_Type current_error_type;

extern jmp_buf global_error_jmp;
extern jmp_buf check_stdin_jmp;
extern jmp_buf find_soak_goal_jmp;
extern jmp_buf find_soak_fact_jmp;
extern jmp_buf goal_posting_jmp;
extern jmp_buf execute_intention_jmp;
extern jmp_buf consult_database_jmp;
extern jmp_buf execute_action_jmp;

#endif /* INCLUDE_oprs_rerror */
