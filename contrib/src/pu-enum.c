/*                               -*- Mode: C -*- 
 * pu-enum.c -- 
 * 
 * Copyright (C) 1998-2003 LAAS/CNRS
 * 
 * $Id$
 */


#include <stdio.h>

#include <hashPack.h>

#include "macro-pub.h"

#include "opaque-pub.h"
#include "constant-pub.h"
#include "oprs-type-pub.h"

#include "user-ev-predicate.h"
#include "user-ev-function.h"

#include "ev-function_f-pub.h"
#include "ev-predicate_f-pub.h"

#include "oprs-type_f-pub.h"
#include "pu-mk-term_f.h"
#include "pu-parse-tl_f.h"
#include "oprs-rerror_f-pub.h"

#include "pu-parse-tl_f.h"

#include "pu-enum.h"
#include "pu-enum_f.h"

#define PU_ATOM_ENUM_HASH_SIZE 64		/* has to be a power of two */
static int PU_atom_enum_hashtable_size = PU_ATOM_ENUM_HASH_SIZE;
static int PU_atom_enum_hashtable_mod = PU_ATOM_ENUM_HASH_SIZE - 1;
HASH PU_atom_enum_hash;


typedef struct {	
     char *atom;
     int enm;
} PU_Atom_Enum;

/* Hash functions. */

static int PU_atom_enum_hash_a_string(PString name)
{
     return (hash_a_string(name) & PU_atom_enum_hashtable_mod);
}
static PBoolean PU_atom_enum_match(PString s1, PU_Atom_Enum *en)
{
     return ( s1 == en->atom);
}

void PU_init_atom_enum_hash() 
{
    PU_atom_enum_hash = make_hashtable(PU_atom_enum_hashtable_size, PU_atom_enum_hash_a_string, PU_atom_enum_match);
}

void PU_declare_enum_atom(char *id, int enm)
{
     char *atom;
     PU_Atom_Enum *atom_enum;

     atom = make_atom(id);
     
     if ((atom_enum = (PU_Atom_Enum *)(get_from_hashtable(PU_atom_enum_hash,atom)))) {
	  if (enm != atom_enum->enm)
	       fprintf(stderr, "RS:redeclaring atom/enum:%s.\n",atom); 
	  else
	       return;
     } else {
	  atom_enum = MAKE_OBJECT(PU_Atom_Enum);
	  atom_enum->atom = atom;
     }
     atom_enum->enm = enm;
     add_to_hashtable(PU_atom_enum_hash, atom_enum, atom); /* redeclaring it anyway... */
}

PBoolean PU_enum_atom(char *atom, int *enm)
{
     PU_Atom_Enum *atom_enum;

     if ((atom_enum = (PU_Atom_Enum *)(get_from_hashtable(PU_atom_enum_hash,atom)))) {
	  *enm = atom_enum->enm;
	  return TRUE;
     } else return FALSE;
}


Term *PU_ef_atom_enum(TermList terms)
{
     char *atom;

     if (! (PUGetOprsParameters(terms, 1, ATOM, &atom))) {
	  report_recoverable_external_error("Expecting an ATOM in ef_atom_enum.");
	  return build_nil();
     } else {
	  int enm;

	  if (PU_enum_atom(atom, &enm))
	       return build_integer(enm);
	  else 
	       return build_nil();
     }
}

void PU_enum_declare_user_eval_funct(void)
{
     make_and_declare_eval_funct("ENUM", PU_ef_atom_enum, 1);
}


PBoolean PU_ep_report_equal(TermList terms)
{
     char *atom;
     int report;

     if (! (PUGetOprsParameters(terms, 2, INTEGER, &report, ATOM, &atom))) {
	  report_recoverable_external_error("Expecting an INTEGER and an ATOM in ep_report_equal.");
	  return FALSE;
     } else {
	  int enm;

	  if (PU_enum_atom(atom, &enm))
	       return (enm == report);
	  else {
	       fprintf(stderr, "PU_ep_report_equal:warning:no enum defined for: %s.\n",atom);
	       return FALSE;
	  }
       }
}

void PU_enum_declare_user_eval_pred(void)
{
     make_and_declare_eval_pred("EQUAL-REPORT", PU_ep_report_equal, 2, TRUE);
     return;
}
