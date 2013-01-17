/*                               -*- Mode: C -*- 
 * int-graph_f.h -- 
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
#include "int-graph_f-pub.h"

Intention_Graph *make_intention_graph(void);
void insert_intention_in_ig(Intention *i, Intention_Graph *ig, Intention_List after, Intention_List before);
void find_current_intentions(Intention_Graph *ig);
Intention_List successor_intention(Intention_Graph *ig, Intention *i);
Intention_List predecessor_intention(Intention_Graph *ig, Intention *i);
void wake_up_intention(Intention *in);
void wake_up_thread(Thread_Intention_Block *tib);
void asleep_intention(Intention_Graph *ig, Intention *in);
void delete_intention_from_ig(Intention *i, Intention_Graph *ig);
Intention_List intention_graph_intention_list(Intention_Graph *ig);
void rebuilt_intention_graph_graphic(Intention_Graph *ig);
void force_asleep_intention(Intention_Graph *ig, Intention *in);
PBoolean valid_intention(Intention_Graph *ig, Intention *in);
Intention_List make_and_check_c_list_from_l_list_of_intentions (Intention_Graph *ig, L_List l);
void show_sleeping_intention(Intention_Graph *ig);
void continue_intention(Intention *in);
void suspend_intention(Intention *in);
void free_intention_graph(Intention_Graph *ig);
void sprint_show_intention(Sprinter *sp, Intention *in,  Intention_Graph *ig);
void show_intentions(Intention_Graph *ig);
