/*                               -*- Mode: C -*- 
 * oprs-sprint_f.h -- 
 * 
 * $Id$
 * 
 * Copyright (c) 1991-2009 Francois Felix Ingrand.
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

#include "opaque.h"
#include "oprs-type.h"
#include "oprs-sprint_f-pub.h"

void sprint_int_array(Sprinter *sp, Int_array *ia);
void sprint_float_array(Sprinter *sp, Float_array *fa);

void sprint_property(Sprinter *sp, Property *p);
void sprint_list_property(Sprinter *sp, PropertyList pl);
void sprint_frame(Sprinter *sp, FramePtr fp);
void sprint_lenv(Sprinter *sp, List_Envar le);
void sprint_envar(Sprinter *sp, Envar *ev);
void sprint_undo_list(Sprinter *sp, List_Undo lu);
void sprint_undo(Sprinter *sp, Undo *ud);
void sprint_expr(Sprinter *sp, Expression *expr);
void sprint_expr_list(Sprinter *sp,ExprList tl);
void sprint_expr_frame(Sprinter *sp, ExprFrame *ef);
void sprint_tl(Sprinter *sp, TermList tl);
void sprint_term(Sprinter *sp, Term *term);
void sprint_unknown_lisp_object(Sprinter *sp, L_List l);
void sprint_l_list(Sprinter *sp, L_List l);
void sprint_backslash_string(Sprinter *sp, PString string);
void sprint_action_field(Sprinter *sp,Action_Field *ac);


#define SPRINTER_STRING(sp) ((sp)->str)
#define SPRINTER_CUR_POS(sp) ((sp)->str+(sp)->size)
#define SPRINTER_SIZE(sp) ((sp)->size)
#define SPRINTER_REMAINING_SIZE(sp) ((sp)->tsize - (sp)->size)
#define SPRINTER_TSIZE(sp) ((sp)->tsize)

#define SPRINT(_sp,_eval,_sprintf) \
     do {char *f; int _real_size;unsigned int eval = _eval;		\
    if (SPRINTER_REMAINING_SIZE(_sp) < (unsigned)(eval+1))\
	enlarge_sprinter(_sp,eval+1);\
    f = SPRINTER_CUR_POS(_sp);\
    _real_size = NUM_CHAR_SPRINT(_sprintf);\
    if (_real_size < 0) {\
	fprintf(stderr,LG_STR("ERROR: In the sprintf of the sprinter:\t%s.\n",\
			      "ERREUR: Dans le sprintf du sprinter:\t%s.\n"),\
		sprinter_string(_sp));\
	_real_size = strlen(f);\
    }\
    if ((unsigned)_real_size > (unsigned)(eval)) \
	fprintf(stderr,LG_STR("ERROR: Not enough space reserved (%d < %d) in the sprinter:\t%s.\n",\
			      "ERREUR: Pas assez de place réservé (%d < %d) dans le sprinter:\t%s.\n"),\
		eval, _real_size, sprinter_string(_sp));\
    SPRINTER_SIZE(_sp) += _real_size;\
} while (0)
