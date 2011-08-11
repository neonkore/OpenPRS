/*                               -*- Mode: C -*- 
 * int-graph.h -- Definition du graph d'intentions.
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

#ifndef INCLUDE_intention_graph
#define INCLUDE_intention_graph

#include "opaque.h"
#include "intention.h"

struct intention_graph {
     Intention_List list_intentions;
     Intention_List list_first;
     Intention_List list_runnable;
     Intention_List list_condamned;
     Intention_Paire_List list_pairs;
     Intention_List current_intentions;
     Intention_List sleeping;             /* All inactive intentions */
     Intention_List sleeping_on_cond;     /*  */
     PFB sort_predicat;
#ifdef GRAPHIX
     List_IOG list_inode;
     List_IOG list_iedge;
#endif
};

typedef struct intention_paire {
     Intention *first;
     Intention *second;
#ifdef GRAPHIX
     IOG *iog;
#endif
} Intention_Paire;

#endif /* INCLUDE_intention_graph */
