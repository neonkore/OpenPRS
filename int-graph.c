/*                               -*- Mode: C -*- 
 * int-graph.c -- fonctions lies a l'utilisation du graph des intentions.
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

#include <stdlib.h>

/*	For timeval definition. */
#ifdef WIN95
#include <winsock.h>
#endif


#ifdef GRAPHIX
#ifdef GTK
#include <gtk/gtk.h>
#include "xm2gtk.h"
#else
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#endif
#endif

#include "macro.h"
#include "constant.h"


#ifdef OPRS_PROFILING
#include "user-external.h"
#endif

#include "oprs-profiling.h"

#include "oprs-type.h"
#include "intention.h"
#include "int-graph.h"
#include "fact-goal.h"
#include "oprs-sprint.h"

#include "oprs-error.h"

#include "oprs-error_f.h"
#include "int-graph_f.h"
#include "oprs.h"
#include "intention_f.h"
#include "intention_f-pub.h"

#ifdef GRAPHIX
#ifdef GTK
#include "op-structure.h"
#include "gope-graphic.h"
#include "goprs-intention.h"
#include "goprs-intention_f.h"
#include "goprs-main.h"
#else
#include "xoprs-main.h"
#include "xoprs-intention.h"
#include "xoprs-intention_f.h"
#endif
#endif

#include "lisp-list_f.h"
#include "oprs-print_f.h"
#include "oprs-sprint_f.h"
#include "activate_f.h"
#include "oprs-profiling_f.h"

void free_intention_graph(Intention_Graph *ig)
{
     FREE_SLIST(ig->list_intentions);
     FREE_SLIST(ig->list_first);
     FREE_SLIST(ig->list_runnable);
     FREE_SLIST(ig->list_condamned);
     FREE_SLIST(ig->list_pairs);
     FREE_SLIST(ig->current_intentions);
     FREE_SLIST(ig->sleeping);
     FREE_SLIST(ig->sleeping_on_cond);
#ifdef GRAPHIX
     FREE_SLIST(ig->list_inode);
     FREE_SLIST(ig->list_iedge);
#endif     
     FREE(ig);
}

Intention_Graph *make_intention_graph(void)
{
     Intention_Graph *ig =  MAKE_OBJECT(Intention_Graph);

     ig->list_intentions = sl_make_slist();
     ig->list_first = sl_make_slist();
     ig->list_runnable = sl_make_slist();
     ig->list_condamned = sl_make_slist();
     ig->list_pairs = sl_make_slist();
     ig->current_intentions = sl_make_slist();
     ig->sleeping =  sl_make_slist();
     ig->sleeping_on_cond =  sl_make_slist();
     ig->sort_predicat = (PFB)NULL;
#ifdef GRAPHIX
     ig->list_inode =  sl_make_slist();
     ig->list_iedge =  sl_make_slist();
#endif

     return ig;
}

Intention_List intention_graph_intention_list(Intention_Graph *ig)
{
     return ig->list_intentions;
}

PBoolean eq_intention(Intention *i1, Intention *i2)
{
     return (i1 == i2);
}

PBoolean valid_intention(Intention_Graph *ig, Intention *in)
{
     /*      return (sl_search_slist(ig->list_intentions,in,eq_intention) != NULL);  */
     return sl_in_slist(ig->list_intentions,in); 
}

Intention_List make_and_check_c_list_from_l_list_of_intentions (Intention_Graph *ig, L_List l)
{
     Intention *in;
     Slist *res = sl_make_slist();

     while (l != l_nil) {
	  in = CAR(l)->u.in;
	  if (valid_intention(ig, in))
	      sl_add_to_tail(res, in);
	  else
	       oprs_perror("make_and_check_c_list_from_l_list_of_intentions", PE_DEAD_INTENTION);
	  l = CDR(l);
     }
     
     return res;	  
}

void rebuilt_intention_graph_graphic(Intention_Graph *ig)
{
#ifdef GRAPHIX
     Intention *in;
     Intention_Paire *ip;

     sl_loop_through_slist(ig->list_intentions,in,Intention *){
       if (! (in->iog)) {
#ifdef GTK
	 in->iog =  create_ginode(global_int_draw_data, inCGCsp, in);
#else
	 in->iog =  create_ginode(global_int_draw_data, in);
#endif
       }
     }
     
     sl_loop_through_slist(ig->list_pairs, ip, Intention_Paire *) {
       if (! (ip->iog)) {
#ifdef GTK
	 ip->iog = create_giedge(global_int_draw_data, ip->first, ip->second);
#else
	 ip->iog = create_giedge(global_int_draw_data, ip->first, ip->second);
#endif
       }
     }
     global_int_draw_data->reposition_all = TRUE;
     draw_intention_graph(global_int_draw_data); /* We've got to do it... */
#endif
}

void free_intention_paire(Intention_Paire *ip)
{
     /* do not need to free them it is done by the node if (ip->iog) destroy_giedge(global_int_draw_data, ip->iog); */
     FREE(ip);
}

Intention_Paire *make_pair(Intention *first, Intention *second)
{
     Intention_Paire *ip = MAKE_OBJECT(Intention_Paire);

     ip->first = first;
     ip->second = second;
#ifdef GRAPHIX
     ip->iog = (debug_trace[GRAPHIC_INTEND] && global_int_draw_data ? create_giedge(global_int_draw_data, first, second) : NULL);
#endif
     return ip;
}

void insert_intention_in_ig(Intention *i, Intention_Graph *ig, Intention_List after, Intention_List before) 
{
     Intention *itmp;

     sl_add_to_head(ig->list_intentions,i);

     if (sl_slist_empty(after)) {
	  sl_add_to_tail(ig->list_first,i);
	  sl_add_to_tail(ig->list_runnable,i);
     } else 
	  sl_loop_through_slist(after,itmp,Intention *){
	       sl_add_to_tail(ig->list_pairs,make_pair(itmp,i));
	  }
     sl_loop_through_slist(before,itmp,Intention *){
	  sl_add_to_tail(ig->list_pairs,make_pair(i,itmp));
	  sl_delete_slist_node(ig->list_first,itmp); /* Just in case itmp was a root. */
	  sl_delete_slist_node(ig->list_runnable,itmp); /* Just in case itmp was a root. */
     }
#ifdef GRAPHIX
     if (debug_trace[GRAPHIC_INTEND] && global_int_draw_data) {
	  global_int_draw_data->reposition_all = TRUE;
	  draw_intention_graph(global_int_draw_data);
     }
#endif
}

Intention_List predecessor_intention(Intention_Graph *ig, Intention *i)
/* 
 * predecessor_intention - Return a list of the intention before i in ig. the result will 
 *                       have to be freed by the caller... please.
 */
{
     Intention_Paire * ip;
     Intention_List res = sl_make_slist();

     sl_loop_through_slist(ig->list_pairs, ip, Intention_Paire *) {
	  if (i == ip->second)
	       sl_add_to_tail(res,ip->first);
     }
     
     return res;
}

Intention_List successor_intention(Intention_Graph *ig, Intention *i)
/* 
 * successor_intention - Return a list of the intention after i in ig. the result will 
 *                       have to be freed by the caller... please.
 */
{
     Intention_Paire * ip;
     Intention_List res = sl_make_slist();

     sl_loop_through_slist(ig->list_pairs, ip, Intention_Paire *) {
	  if (i == ip->first)
	       sl_add_to_tail(res,ip->second);
     }
     
     return res;
}

void delete_intention_from_ig(Intention *i, Intention_Graph *ig)
{
     Intention_Paire * ip;
     Intention_Paire_List ipl;
     Intention_List il_a, il_b;
     Intention *i1, *i2;
     PBoolean was_first, is_first;

     sl_delete_slist_node(ig->list_intentions,i); /* All the following are overdoing it a little bit, but it is not more expensive */
     was_first = (sl_delete_slist_node(ig->list_first,i) != NULL);
     sl_delete_slist_node(ig->list_runnable,i);
     sl_delete_slist_node(ig->sleeping,i);
     sl_delete_slist_node(ig->sleeping_on_cond,i);
     ipl = sl_make_slist();
     il_a = sl_make_slist();
     il_b = sl_make_slist();
     sl_delete_slist_node(ig->current_intentions, i);
     sl_loop_through_slist(ig->list_pairs, ip, Intention_Paire *) {
	  if (i == ip->first) {
	       sl_add_to_tail(il_a,ip->second);
	       free_intention_paire(ip);
	  } else if (i == ip->second) {
	       sl_add_to_tail(il_b,ip->first);
	       free_intention_paire(ip);
	  } else sl_add_to_tail(ipl,ip);
     }

     sl_loop_through_slist(il_b, i1, Intention *) {
	  sl_loop_through_slist(il_a, i2, Intention *) {
	       sl_add_to_tail(ipl,make_pair(i1,i2));
	  }
     }
     if (was_first)		/* or if il_b is empty */
	  sl_loop_through_slist(il_a, i1, Intention *) {
	       is_first = TRUE;
	       sl_loop_through_slist(ipl, ip, Intention_Paire *) {
		    if (i1 == ip->second) is_first = FALSE;
	       }
 
	       if (is_first) {
		    sl_add_to_tail(ig->list_first,i1);
		    if (! sl_in_slist(ig->sleeping,i1))
			 sl_add_to_tail(ig->list_runnable,i1);
	       }
	  }
     FREE_SLIST(il_a);
     FREE_SLIST(il_b);
     FREE_SLIST(ig->list_pairs);
     ig->list_pairs=ipl;
	
#ifdef  OPRS_PROFILING
	  TIME_STOP(&i->creation,T_INTENTION_IN_GRAPH);
#endif /* OPRS_PROFILING */

     delete_intention(i);

#ifdef GRAPHIX
     if (debug_trace[GRAPHIC_INTEND] && global_int_draw_data) {
	  global_int_draw_data->reposition_all = TRUE;
	  draw_intention_graph(global_int_draw_data);
}
#endif

}

PFB intention_list_sort_predicate = NULL;
PFPL intention_scheduler = NULL;
PFPL intention_par_scheduler = NULL;

Intention_List apply_scheduler(Intention_List il)
{
     if (sl_slist_empty(il)) return il;

     if (intention_scheduler) {
	  return (* (intention_scheduler))(il);
     } else if (intention_list_sort_predicate) {
	  return (Intention_List)sl_sort_slist_func(il,intention_list_sort_predicate);
     } else return il;
     
}

Intention_List apply_par_scheduler(Intention_List il)
{
     if (sl_slist_empty(il)) return il;

     if (intention_par_scheduler) {
	  return (* (intention_par_scheduler))(il);
     } else 
	  return il;
}

Intention_List intention_scheduler_time_sharing_with_sort_predicate(Intention_List l)
{
     static int loop = 0;

     if (loop++ == 6) {		/* This scheduler is not too smart has it does not take into account the number of threads. */
	  Intention *i =  (Intention *)sl_get_from_head(l);
	  sl_add_to_tail(l,i);
	  loop = 0;
	  if (intention_list_sort_predicate)
	       l = (Intention_List)sl_sort_slist_func(l,intention_list_sort_predicate);
     }
     return l;
}

Intention_List intention_scheduler_time_sharing(Intention_List l)
{
     static int loop = 0;

     if (loop++ == 6) {		/* This scheduler is not too smart has it does not take into account the number of threads. */
	  Intention *i =  (Intention *)sl_get_from_head(l);
	  sl_add_to_tail(l,i);
	  loop = 0;
     }
     return l;
}

PBoolean intention_list_sort_by_priority(Intention *i1, Intention *i2)
{
     return (i1->priority > i2->priority);
}

PBoolean intention_list_sort_by_time(Intention *i1, Intention *i2)
{
     return (i1->creation.tv_sec < i2->creation.tv_sec);
}

PBoolean intention_list_sort_by_priority_time(Intention *i1, Intention *i2)
{
     if (i1->priority != i2->priority)
	  return (i1->priority > i2->priority);
     else
	  return (i1->creation.tv_sec < i2->creation.tv_sec);
}

Intention_List list_in_modified(Intention_List list_old, Intention_List list_new, Intention_List list_intentions)
{
     Intention_List res = NULL;
     Intention *intention;
     
     sl_loop_through_slist(list_intentions, intention, Intention *)
	  if ((sl_in_slist(list_new, intention) == 0) ^ (sl_in_slist(list_old, intention) == 0))
	       SAFE_SL_ADD_TO_HEAD(res,intention);
          
     return res;
}

void find_current_intentions(Intention_Graph *ig)
{
     Intention *previous_current;
     Intention *intention;
#ifdef GRAPHIX
     Intention_List list_modified = NULL;
#endif

     if (current_oprs->critical_section) {
	  if (sl_slist_empty(ig->current_intentions)) {
	       printf (LG_STR("WARNING: Critical section without current intention.\n\tForce exit of the critical section.\n",
			      "ATTENTION: Section critique sans intention courante.\n\tSortie forcée de la section critique.\n"));
	       current_oprs->critical_section = FALSE;
	  }
	  return;
     }

     if (run_option[PAR_EXEC]) {
	  Intention_List list_new;

	  list_new = COPY_SLIST(ig->list_runnable);

	  list_new = apply_par_scheduler(list_new);

#ifdef GRAPHIX
	  if (global_int_draw_data && 
	      debug_trace[GRAPHIC_INTEND]) {
	       list_modified = list_in_modified(ig->current_intentions, list_new, ig->list_intentions);
	       SAFE_SL_LOOP_THROUGH_SLIST(list_modified, intention, Intention *)
		    touch_intention_ginode(intention); /* Touch the new one and the one which are removed */
	  }
#endif

	  FREE_SLIST(ig->current_intentions);
	  ig->current_intentions = list_new;

#ifdef GRAPHIX
	  if (global_int_draw_data && 
	      debug_trace[GRAPHIC_INTEND]) {
	       if (! (SAFE_SL_SLIST_EMPTY(list_modified)))
		    draw_intention_graph(global_int_draw_data); /* We've got to do it... */
	       SAFE_SL_FREE_SLIST(list_modified);
	  }
#endif

     } else {			/* Execution in mono intention mode. */
	  previous_current = (Intention *)sl_get_slist_head(ig->current_intentions);

#ifdef GRAPHIX
	  if (previous_current && /* There is a current intention */
	      (!(sl_in_slist(ig->list_runnable,previous_current)))) /* or it is not runnable */
	       if (debug_trace[GRAPHIC_INTEND] && global_int_draw_data) touch_intention_ginode(previous_current);
#endif
	  FLUSH_SLIST(ig->current_intentions);

	  if (! (sl_slist_empty(ig->list_runnable))) {
	       Intention_List runnable = sl_make_slist();

	       sl_loop_through_slist(ig->list_runnable, intention, Intention *) 
		    if (RANDOM() & 16) 
			 sl_add_to_head(runnable,intention);
		    else
			 sl_add_to_tail(runnable,intention);
	       
	       runnable = apply_scheduler(runnable);

	       if (! sl_slist_empty(runnable)) {
		    intention = (Intention *)sl_get_slist_head(runnable);
		    sl_add_to_head(ig->current_intentions, intention); /* H've got one. Get out of here. */
#ifdef GRAPHIX
		    if (debug_trace[GRAPHIC_INTEND] && global_int_draw_data) touch_intention_ginode(intention);
#endif
	       }
	       
	       FREE_SLIST(runnable);
	  }

#ifdef GRAPHIX
	  if ((previous_current != (Intention *)sl_get_slist_head(ig->current_intentions)) && 
	      global_int_draw_data && 
	      debug_trace[GRAPHIC_INTEND]) { /* We change of current intention */
	       if (previous_current)
		    touch_intention_ginode(previous_current); 
	       
	       draw_intention_graph(global_int_draw_data); /* We've got to do it... */
	  }
#endif
     }
     return;
}

void wake_up_thread(Thread_Intention_Block *tib)
{
     if (tib->status == TIBS_SLEEPING || tib->status == TIBS_ACTION_WAIT) {
	  tib->status = TIBS_ACTIVE;
	  add_in_active_tib(tib);
	  continue_intention(tib->intention);
     } else if (tib->status == TIBS_SUSP_SLEEPING) {
	  tib->status = TIBS_SUSP_ACTIVE;
     }
}

void wake_up_intention(Intention *in)
{
     if (in->status == IS_SLEEPING) {
	  sl_delete_slist_node(current_oprs->intention_graph->sleeping_on_cond,in);
	  sl_delete_slist_node(current_oprs->intention_graph->sleeping,in);
	  if (sl_in_slist(current_oprs->intention_graph->list_first,in))
	       sl_add_to_head(current_oprs->intention_graph->list_runnable, in);
	  in->status = IS_ACTIVE;

#ifdef GRAPHIX
	  if (debug_trace[GRAPHIC_INTEND] && global_int_draw_data) touch_intention_ginode(in);
#endif
     } else if (in->status == IS_SUSP_SLEEPING) {
	  sl_delete_slist_node(current_oprs->intention_graph->sleeping_on_cond,in);
	  in->status = IS_SUSP_ACTIVE;

#ifdef GRAPHIX
	  if (debug_trace[GRAPHIC_INTEND] && global_int_draw_data) touch_intention_ginode(in);
#endif
     }

}

void force_asleep_intention(Intention_Graph *ig, Intention *in)
{
     if ((current_oprs->critical_section) &&
	 (current_oprs->critical_section->intention == in)) {
	  printf (LG_STR("WARNING: You are putting the current intention to sleep in a critical section.\n",
			 "ATTENTION: Vous mettez l'intention courante à dormir alors qu'elle est en section critique.\n"));
     }
     
     if (!sl_in_slist(ig->sleeping_on_cond, in)) {
	  /* We add it if it was not already sleeping */
	  sl_add_to_head(ig->sleeping_on_cond, in);
	  switch (in->status) {
	  case IS_SUSP_ACTIVE:
	       in->status = IS_SUSP_SLEEPING;
	       break;
	  case IS_ACTIVE:
	       sl_add_to_head(ig->sleeping, in);
	       sl_delete_slist_node(ig->list_runnable, in);
	       in->status = IS_SLEEPING;
	       break;
	  case IS_SUSP_SLEEPING:
	  case IS_SLEEPING:
	       fprintf(stderr, LG_STR("WARNING: Intention already sleeping.\n",
				      "ATTENTION: Intention déjà endormie.\n"));
	       break;
	  default:
	       fprintf(stderr, LG_STR("ERROR: force_asleep_intention: Unknown intention status.\n",
				      "ERREUR: force_asleep_intention: Etat d'intention inconnu.\n"));
	       break;
	  }
#ifdef GRAPHIX	  
	  if (debug_trace[GRAPHIC_INTEND] && global_int_draw_data) 
	       touch_intention_ginode(in); /* Will check for draw_data. */
#endif
     }
}

/* continue the intention, when one of its thread is again active */

void continue_intention(Intention *in)
{
     switch (in->status) {
     case IS_ACTIVE:
     case IS_SLEEPING:
	  /* already active ( not suspended ) */
	  break;
     case IS_SUSP_ACTIVE:
	  sl_delete_slist_node(current_oprs->intention_graph->sleeping,in);
	  if (sl_in_slist(current_oprs->intention_graph->list_first,in))
	       sl_add_to_head(current_oprs->intention_graph->list_runnable, in);
	  in->status = IS_ACTIVE;
	  break;
     case IS_SUSP_SLEEPING:
	  in->status = IS_SLEEPING;
	  break;
     default:
	       fprintf(stderr, LG_STR("ERROR: continue_intention: Unknown intention status.\n",
				      "ERREUR: continue_intention: Etat d'intention inconnu.\n"));
	  break;
     }
#ifdef GRAPHIX	  
     if (debug_trace[GRAPHIC_INTEND] && global_int_draw_data) touch_intention_ginode(in);
#endif
}

/* Suspend the intention, when there is no more active threads */

void suspend_intention(Intention *in) 
{
     Intention_Graph *ig = current_oprs->intention_graph;
     if ((current_oprs->critical_section) &&
	 (current_oprs->critical_section->intention == in)) {
	  printf (LG_STR("WARNING: The current intention has no more active threads in a critical section.\n",
			 "ATTENTION: L'intention courante est en section critique sans thread actif.\n"));
     }
     
     switch (in->status) {
     case IS_ACTIVE:
	  sl_add_to_head(ig->sleeping, in);
	  sl_delete_slist_node(ig->list_runnable, in);
	  in->status = IS_SUSP_ACTIVE;
	  break;
     case IS_SLEEPING:
	  in->status = IS_SUSP_SLEEPING;
     case IS_SUSP_ACTIVE:
     case IS_SUSP_SLEEPING:
	  fprintf(stderr, LG_STR("WARNING: Suspending an inactive intention.\n",
				 "ATTENTION: Tentative de suspendre une intention inactive.\n"));
	  break;
     default:
	  fprintf(stderr, LG_STR("ERROR: suspend_intention: Unknown intention status.\n",
				 "ERREUR: suspend_intention: Etat d'intention inconnu.\n"));
	  break;
     }
#ifdef GRAPHIX	  
     if (debug_trace[GRAPHIC_INTEND] && global_int_draw_data) touch_intention_ginode(in);
#endif
}

Intention_List intention_graph_roots(Intention_Graph *ig)
{
	return ig->list_first;	
}
	
void sprint_show_intention(Sprinter *sp, Intention *in,  Intention_Graph *ig)
{
     sprint_intention(sp,in); 

     if (sl_in_slist(ig->current_intentions, in))
	  SPRINT(sp,32,sprintf(f,LG_STR(" (current intention)",
					" (intention courante)")));
	       
     if (in->goal) {
	  SPRINT(sp,32,sprintf(f,LG_STR(" achieving the goal: ",
					" réalisant le but: ")));
	  sprint_expr(sp,in->goal->statement);
	  SPRINT(sp,1,sprintf(f,"\n"));
     } else if (in->fact) {
	  SPRINT(sp,32,sprintf(f,LG_STR(" in response to the fact: ",
					" en réponse au fait: ")));
	  sprint_expr(sp,in->fact->statement);
	  SPRINT(sp,1,sprintf(f,"\n"));
     }
     SPRINT(sp,64,sprintf(f,LG_STR("composed of the following thread(s):\n",
				   "composé des threads suivants:\n")));
     sprint_show_tib_list(sp,in->fils, 4);
}

void sprint_intention_graph(Sprinter *sp, Intention_Graph *ig)
{
     if (sl_slist_empty (ig->list_intentions)) {
	  SPRINT(sp,64,sprintf(f,LG_STR("There are no intention in the intention graph.\n",
					"Il n'y a pas d'intention dans le graphe d'intentions.\n")));
     } else {
	  Intention *in;

	  SPRINT(sp,32,sprintf(f,LG_STR("The intentions are:\n",
					"Les intentions sont:\n")));
	  sl_loop_through_slist(ig->list_intentions, in, Intention *)
	       sprint_show_intention(sp, in, ig);
	  SPRINT(sp,10,sprintf(f,LG_STR("Done.\n",
					"Terminé.\n")));
     }
}
	
void show_intentions(Intention_Graph *ig)
{
     static Sprinter *sp = NULL;

     if (! sp)
       sp = make_sprinter(0);
     else
       reset_sprinter(sp);

     sprint_intention_graph(sp,ig);
     printf("%s",sprinter_string(sp));
}
