/*                               -*- Mode: C -*- 
 * pu-genom.c -- 
 * 
 * Copyright (C) 1999-2012 LAAS/CNRS
 * 
 * $Id$
 */

#ifndef __CEXTRACT__

#include <config.h>

#ifdef VXWORKS
#include "vxWorks.h"
#endif

/* include system */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* pour OPRS */
#include "slistPack.h"
#include "slistPack_f.h"

#include "constant-pub.h"
#include "oprs-type-pub.h"
#include "oprs-type.h"
#include "macro-pub.h"

#include "oprs-type_f-pub.h"
#include "oprs-pred-func_f.h"
#include "lisp-list_f-pub.h"

#include "pu-parse-tl_f.h"
#include "pu-mk-term_f.h"
#include "pu-genom_f.h"
#include "pu-enum_f.h"
#endif

PBoolean pu_check_ttc_name(Expression *tc, char *name, char *type_name)
{
     if (strcasecmp(PFR_NAME(tc->pfr), (name ? name : type_name)) != 0) {
	  fprintf(stderr,"pu_check_ttc_name: bad name, %s should be %s\n",
		   (name ? name : type_name), PFR_NAME(tc->pfr));
	  return FALSE;
     } else 
	  return TRUE;
}

PBoolean pu_check_ttc_name_alter(Expression *tc, char *name)
{
     if (strcasecmp(PFR_NAME(tc->pfr), name) != 0) {
	  return FALSE;
     } else 
	  return TRUE;
}

PBoolean pu_check_ttc_name_strict(Expression *tc, char *name)
{
     if (!pu_check_ttc_name_alter(tc, name)) {
	  fprintf(stderr,"pu_check_ttc_name_strict: bad name, %s should be %s\n",
		  PFR_NAME(tc->pfr), name);
	  return FALSE;
     } else 
	  return TRUE;
}

/* PBoolean pu_check_array_size(int *i) */
/* { */
/*      if (*i < 0) { */
/* 	  fprintf(stderr,"pu_check_array_size: buggy Genom code... array: %d\n", *i); */
/* 	  return FALSE; */
/*      } */
/*      if (!*i) *i=1; */
/*      return TRUE; */
/* } */

/* This macro is the one which defines encoding function (from Oprs to C struct.
   As they are the sames, no need to write code, let the preprocessor do it for you.
   The args are:
   - type_gen_name is the suffix to give to the function name,
   - type_gen is the name of the Genom C type,
   - type_oprs is the Oprs C type,
   - type_term is the enum for the type of the Oprs TERM.
*/

#define define_pu_encode_genom(type_gen_name,type_gen,type_oprs,type_term) \
PBoolean pu_encode_genom_ ## type_gen_name (char *name, Expression *tc, type_gen *val_addr, int size)\
{\
     if (!pu_check_ttc_name(tc, name, #type_term))\
	  return FALSE;\
     else {\
       int j;\
       type_oprs val;\
       for (j=1;j<=size;j++) {\
       	    if (!PUGetOprsTermCompSpecArg(tc, j, type_term, &val))\
		 return FALSE;\
	    *(val_addr+j-1) = (type_gen)val;\
           }\
       return TRUE;\
     }\
}

#define define_pu_encode_genom3(type_gen_name,type_gen,type_oprs,type_term) \
PBoolean pu_encode_genom3_ ## type_gen_name (char *name, Expression *tc, type_gen *val_addr)\
{\
     if (!pu_check_ttc_name(tc, name, #type_term))\
	  return FALSE;\
     else {\
       type_oprs val;\
       if (!PUGetOprsTermCompSpecArg(tc, 1, type_term, &val))\
	 return FALSE;\
       *(val_addr) = (type_gen)val;\
       return TRUE;\
     }\
}


/* pu_encode_genom_double */
define_pu_encode_genom(double, double, double, FLOAT)
/* pu_encode_genom_float */
define_pu_encode_genom(float, float, double, FLOAT)
/* pu_encode_genom_int */
define_pu_encode_genom(long_long_int, long long int, long long int, LONG_LONG)
define_pu_encode_genom(unsigned_long_long_int, unsigned long long int, long long int, LONG_LONG)

define_pu_encode_genom(int, int, int, INTEGER)
define_pu_encode_genom(unsigned_int, unsigned int, int, INTEGER)

define_pu_encode_genom(short, short, int, INTEGER)
define_pu_encode_genom(short_int, short int, int, INTEGER)
define_pu_encode_genom(unsigned_short_int, unsigned short int, int, INTEGER)

define_pu_encode_genom(long_int, long int, int, INTEGER)
define_pu_encode_genom(unsigned_long_int, unsigned long int, int, INTEGER)

define_pu_encode_genom(char, char, int, INTEGER)
define_pu_encode_genom(unsigned_char, unsigned char, int, INTEGER)

define_pu_encode_genom(addr, void *, void *, U_POINTER)

/* pu_encode_genom_double */
define_pu_encode_genom3(double, double, double, FLOAT)
/* pu_encode_genom_float */
define_pu_encode_genom3(float, float, double, FLOAT)
/* pu_encode_genom_int */
define_pu_encode_genom3(long_long_int, long long int, long long int, LONG_LONG)
define_pu_encode_genom3(unsigned_long_long_int, unsigned long long int, long long int, LONG_LONG)

define_pu_encode_genom3(int, int, int, INTEGER)
define_pu_encode_genom3(unsigned_int, unsigned int, int, INTEGER)

define_pu_encode_genom3(short, short, int, INTEGER)
define_pu_encode_genom3(short_int, short int, int, INTEGER)
define_pu_encode_genom3(unsigned_short_int, unsigned short int, int, INTEGER)

define_pu_encode_genom3(long_int, long int, int, INTEGER)
define_pu_encode_genom3(unsigned_long_int, unsigned long int, int, INTEGER)

define_pu_encode_genom3(char, char, int, INTEGER)
define_pu_encode_genom3(unsigned_char, unsigned char, int, INTEGER)

define_pu_encode_genom3(addr, void *, void *, U_POINTER)

/* Strings are particular... */
PBoolean pu_encode_genom_string(char *name, Expression *tc, char *val_addr, int size, int max_size)
{
     if (!pu_check_ttc_name(tc, name, "string"))
	  return FALSE;
     else {
	  int j;
	  char *val;
	  for (j=1;j<=size;j++) {
	       if (!PUGetOprsTermCompSpecArg(tc, j, STRING, &val))
		    return FALSE;
	       if ((strlen(val)<max_size)) {
		    memcpy(val_addr+(j-1)*max_size,val, strlen(val)+1);
	       } else {
		    fprintf(stderr,"pu_encode_genom_string: string to long: %u > %u\n",
			    strlen(val), (unsigned int)max_size);
		    *(val_addr +(j-1)*max_size) = '\0';
		    return FALSE;
	       }
	  }
	  return TRUE;
     }
}



/***************************************************************************/
/***************************************************************************/
/* Decodage */
/***************************************************************************/
/***************************************************************************/

/* This macro is the one which defines decoding function (from C Struct to Oprs).
   As they are the sames, no need to write code, let the preprocessor do it for you.
   The args are:
   - type_gen_name is the suffix to give to the function name,
   - type_gen is the name of the Genom C type,
   - oprs_fct is the Oprs function to build proper term,
   - type_term is the C type of the expected object by the term building function above.
*/

#define define_pu_decode_genom(type_gen_name,type_gen,oprs_fct,type_term,prop_type) \
Term *pu_decode_genom_ ## type_gen_name(char *name, type_gen *addr, int size) \
{\
     Pred_Func_Rec *fr = find_or_create_pred_func(declare_atom(name?name:#prop_type));\
     TermList tl = sl_make_slist();\
     int j;\
     for (j=0; j<size; j++) {\
	  sl_add_to_tail(tl, oprs_fct((type_term)*(addr+j)));\
     }\
     return build_term_expr(build_expr_pfr_terms(fr, tl));\
}

#define define_pu_decode_genom3(type_gen_name,type_gen,oprs_fct,type_term,prop_type) \
Term *pu_decode_genom3_ ## type_gen_name(char *name, type_gen *addr) \
{\
     Pred_Func_Rec *fr = find_or_create_pred_func(declare_atom(name?name:#prop_type));\
     TermList tl = sl_make_slist();\
     sl_add_to_tail(tl, oprs_fct((type_term)*(addr)));\
     return build_term_expr(build_expr_pfr_terms(fr, tl));\
}

define_pu_decode_genom(int, int, PUMakeTermInteger, int, INTEGER)
define_pu_decode_genom(long_long_int, long long int, PUMakeTermLongLong, long long int, LONG_LONG)
define_pu_decode_genom(unsigned_long_long_int, unsigned long long int, PUMakeTermLongLong, long long int, LONG_LONG)
define_pu_decode_genom(unsigned_int, unsigned int, PUMakeTermInteger, int, INTEGER)
define_pu_decode_genom(unsigned_char, unsigned char, PUMakeTermInteger, int, INTEGER)

define_pu_decode_genom(short, short, PUMakeTermInteger, int, INTEGER)
define_pu_decode_genom(short_int, short int, PUMakeTermInteger, int, INTEGER)
define_pu_decode_genom(unsigned_short_int, unsigned short int, PUMakeTermInteger, int, INTEGER)

define_pu_decode_genom(long_int, long int, PUMakeTermInteger, int, INTEGER)
define_pu_decode_genom(unsigned_long_int, unsigned long int, PUMakeTermInteger, int, INTEGER)

define_pu_decode_genom(addr, void *, PUMakeTermUPointer, void *, U_POINTER)
define_pu_decode_genom(char, char, PUMakeTermInteger, int, INTEGER)
define_pu_decode_genom(float, float, PUMakeTermFloat, double, FLOAT)
define_pu_decode_genom(double, double, PUMakeTermFloat, double, FLOAT)

define_pu_decode_genom3(int, int, PUMakeTermInteger, int, INTEGER)
define_pu_decode_genom3(long_long_int, long long int, PUMakeTermLongLong, long long int, LONG_LONG)
define_pu_decode_genom3(unsigned_long_long_int, unsigned long long int, PUMakeTermLongLong, long long int, LONG_LONG)
define_pu_decode_genom3(unsigned_int, unsigned int, PUMakeTermInteger, int, INTEGER)
define_pu_decode_genom3(unsigned_char, unsigned char, PUMakeTermInteger, int, INTEGER)

define_pu_decode_genom3(short, short, PUMakeTermInteger, int, INTEGER)
define_pu_decode_genom3(short_int, short int, PUMakeTermInteger, int, INTEGER)
define_pu_decode_genom3(unsigned_short_int, unsigned short int, PUMakeTermInteger, int, INTEGER)

define_pu_decode_genom3(long_int, long int, PUMakeTermInteger, int, INTEGER)
define_pu_decode_genom3(unsigned_long_int, unsigned long int, PUMakeTermInteger, int, INTEGER)

define_pu_decode_genom3(addr, void *, PUMakeTermUPointer, void *, U_POINTER)
define_pu_decode_genom3(char, char, PUMakeTermInteger, int, INTEGER)
define_pu_decode_genom3(float, float, PUMakeTermFloat, double, FLOAT)
define_pu_decode_genom3(double, double, PUMakeTermFloat, double, FLOAT)

Term *pu_decode_genom_string(char *name, char *addr, int size, int max_size)
{
     Pred_Func_Rec *fr = find_or_create_pred_func(declare_atom(name?name:"STRING"));
     TermList tl = sl_make_slist();
     int j;
     for (j=0; j<size; j++) {
	  sl_add_to_tail(tl, PUMakeTermString((addr+(j*max_size))));
     }
     return(build_term_expr(build_expr_pfr_terms(fr, tl)));
}

/* Same as above, but just take one object, no need to pass a pointer */
#define define_pu_simple_decode(type_gen_name,type_gen,oprs_fct,type_term,prop_type) \
Term *pu_simple_decode_ ## type_gen_name(char *name, type_gen val) \
{\
     Pred_Func_Rec *fr = find_or_create_pred_func(declare_atom(name?name:#prop_type));\
     TermList tl = sl_make_slist();\
     sl_add_to_tail(tl, oprs_fct((type_term)val));\
     return build_term_expr(build_expr_pfr_terms(fr, tl));\
}

define_pu_simple_decode(int, int, PUMakeTermInteger, int, INTEGER)
define_pu_simple_decode(long_long_int, long long int, PUMakeTermLongLong, long long int, LONG_LONG)
define_pu_simple_decode(unsigned_long_long_int, unsigned long long int, PUMakeTermLongLong, long long int, LONG_LONG)
define_pu_simple_decode(unsigned_int, unsigned int, PUMakeTermInteger, int, INTEGER)
define_pu_simple_decode(unsigned_char, unsigned char, PUMakeTermInteger, int, INTEGER)

define_pu_simple_decode(short, short, PUMakeTermInteger, int, INTEGER)
define_pu_simple_decode(short_int, short int, PUMakeTermInteger, int, INTEGER)
define_pu_simple_decode(unsigned_short_int, unsigned short int, PUMakeTermInteger, int, INTEGER)

define_pu_simple_decode(long_int, long int, PUMakeTermInteger, int, INTEGER)
define_pu_simple_decode(unsigned_long_int, unsigned long int, PUMakeTermInteger, int, INTEGER)

define_pu_simple_decode(addr, void *, PUMakeTermUPointer, void *, U_POINTER)
define_pu_simple_decode(char, char, PUMakeTermInteger, int, INTEGER)
define_pu_simple_decode(float, float, PUMakeTermFloat, double, FLOAT)
define_pu_simple_decode(double, double, PUMakeTermFloat, double, FLOAT)

Term *pu_simple_decode_atom(char *key, char *value)
{
  Pred_Func_Rec *fr = find_or_create_pred_func(declare_atom(key?key:"ATOM"));
  TermList tl = sl_make_slist();
  sl_add_to_tail(tl, PUMakeTermAtom(value));
  
  return(build_term_expr(build_expr_pfr_terms(fr, tl)));
}

Term *pu_simple_decode_string(char *key, char *value)
{
  Pred_Func_Rec *fr = find_or_create_pred_func(declare_atom(key?key:"STRING"));
  TermList tl = sl_make_slist();
  sl_add_to_tail(tl, PUMakeTermString(value));
  
  return(build_term_expr(build_expr_pfr_terms(fr, tl)));
}
