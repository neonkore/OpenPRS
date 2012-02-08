/*                               -*- Mode: C -*- 
 * intention_f.h -- Intention external functions declarations.
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

#include "intention_f-pub.h"

Intention *make_intention(Op_Instance *op_inst, Intention_Graph *ig, int prio);
Intention *make_intention_for_goal(Op_Instance *op_inst,Intention_Graph *ig, int prio, Goal *goal);
Thread_Intention_Block *make_thread_intention_block(Op_Instance *op_inst);

void delete_intention(Intention *in);
void free_intention(Intention *in);
void free_thread_intention_block(Thread_Intention_Block *t_int_bl);

Intention *dup_intention(Intention *in);

void fprint_intention(FILE *f,Intention *in);
void print_intention(Intention *in);
void sprint_intention(Sprinter *sp, Intention *in);

ListLines pretty_print_intention(int width, Intention *in);
Op_Instance *intention_bottom_op_instance(Intention *in);
Op_Instance *ib_op_instance(Thread_Intention_Block *tib);

Thread_Intention_Block *fork_thread_intention_block(Thread_Intention_Block *tib);
Thread_Intention_Block *split_thread_intention_block(Thread_Intention_Block *tib);
void begin_critical_section(Thread_Intention_Block *tib);
void end_critical_section(Thread_Intention_Block *tib);
void propagate_critical_section(Thread_Intention_Block *tib);
void mark_critical_section(Thread_Intention_Block *tib);
void unmark_critical_section(Thread_Intention_Block *tib);
Thread_Intention_Block *dup_thread_intention_block(Thread_Intention_Block *tib);

void fprint_tib(FILE *f,Thread_Intention_Block *tib);
void print_tib(Thread_Intention_Block *tib);

void set_intention_priority(Intention *in, int priority);
void set_intention_id(Intention *in, PString id);

void show_tib_list(Thread_Intention_Block_List tib_list, int indent);
void sprint_show_tib_list(Sprinter *sp, Thread_Intention_Block_List tib_list, int indent);
void sprint_tib(Sprinter *sp, Thread_Intention_Block *tib);
