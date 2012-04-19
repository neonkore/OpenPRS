/*                               -*- Mode: C -*- 
 * fact-goal.h -- Definition of goals and facts.
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


#ifndef INCLUDE_fact_goal
#define INCLUDE_fact_goal

#include "slistPack.h"
#include "slistPack_f.h"

#include "oprs-type.h"
#include "oprs-profiling.h"
#include "op-instance.h"
#include "intention.h"

struct fact {
     Expression * statement;
     PBoolean pris_en_compte;	/* A t'il deja ete pris en compte */
     int executed_ops;
     PString message_sender;
#ifdef OPRS_PROFILING
     PDate creation;		/* Date de creation, reception, soak et reponse */
     PDate reception;
     PDate soak;
     PDate reponse;
#endif
};

/* NOT_FAILED has to be FALSE... */
typedef enum {NOT_FAILED = 0, NORMAL_FAILURE, PASS_MAINT_FAILURE, ACT_MAINT_FAILURE, 
	      FORBIDDEN_SYNTAX, ERROR_HANDLER} Echec_Type;

struct goal {
     Expression *statement;
     PBoolean pris_en_compte;	/* A t'il deja ete pris en compte */
     FramePtr frame;
     PBoolean etat;		/* Satisfait par la base de donnees? */
     Intention *interne;	/* De quelle intention vient il, si NULL, externe au systeme */
     Thread_Intention_Block *tib;
     Op_Instance *succes;	/* Quelle op instance l'a satisfait? */
     Op_Instance_List echec;	/* Quelles sont celles qui n'y sont pas arrive? */
     Echec_Type echoue;		/*  Est-il echouee ou pas? */
     Edge *edge;
     Control_Point *cp;		/* Where do we go next... */
#ifdef OPRS_PROFILING
     PDate creation;		/* Date de creation, reception, soak et reponse */
     PDate reception;
     PDate soak;
     PDate reponse;
#endif
};

#endif /* INCLUDE_fact_goal */
