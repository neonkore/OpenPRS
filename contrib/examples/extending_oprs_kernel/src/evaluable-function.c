/*                               -*- Mode: C -*- 
 * evaluable-function.c -- contains user defined evaluable functions.
 * 
 * Copyright (c) 2013 LAAS/CNRS
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

#include <stdio.h>
#include <math.h>

#include "macro-pub.h"
#include "opaque-pub.h"
#include "constant-pub.h"
#include "oprs-type-pub.h"
#include "oprs-type_f-pub.h"
#include "ev-function_f-pub.h"

#include "evaluable-function_f.h"


#include "pu-parse-tl_f.h"
#include "pu-mk-term_f.h"
#include "pu-genom_f.h"

/* This evaluable function will return the square root of its argument as a FLOAT.*/
Term *sqrt_eval_funct(TermList terms)
{

  double a;

  if (PUGetOprsParameters(terms, 1,
			  FLOAT, &a)) {

    return PUMakeTermFloat(sqrt(a));
  } else {
    return build_nil();
  }  
}

Term *bar2pt_eval_funct(TermList terms)
{
  double x1,y1,x2,y2,xb,yb;

  if (PUGetOprsParameters(terms, 4,
			  FLOAT, &x1,
			  FLOAT, &y1,
			  FLOAT, &x2,
			  FLOAT, &y2)) {
    
    Pred_Func_Rec *fr = find_or_create_pred_func(declare_atom("point2d"));
    TermList tl = sl_make_slist();    
    
    xb = (x1+x2)/2;
    yb = (y1+y2)/2;
    
    
    sl_add_to_tail(tl, pu_simple_decode_double("x",xb)); //build (x <val xb>)
    sl_add_to_tail(tl, pu_simple_decode_double("y", yb)); //build (y <val yb>)
    sl_add_to_tail(tl, PUMakeTermFloat(xb));		     // Add the float value directly
    sl_add_to_tail(tl, PUMakeTermFloat(yb));
				  
    return build_term_expr(build_expr_pfr_terms(fr, tl)); // this will create something like:
  } else {						  // (point2d (x 13.0) (y 24.0) 13.0 24.0) you get the idea
    return build_nil();					  // this will return the Term nil.
  }
}


#define D2D(x1,y1,x2,y2) (sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)))

Term *two_points_distance_eval_funct(TermList terms)
{
  double xa,ya,xb,yb;

  if (PUGetOprsParameters(terms, 4,  
			  FLOAT, &xa,
			  FLOAT, &ya,
			  FLOAT, &xb,
			  FLOAT, &yb)) {
    return PUMakeTermFloat(sqrt(D2D(xa,ya,xb,yb)));
  } else {
    return build_nil();
  }
}


void declare_myoprs_eval_funct(void)
{
  make_and_declare_eval_funct("SQRT", sqrt_eval_funct, 1);
  make_and_declare_eval_funct("BARI", bar2pt_eval_funct, 4);
  make_and_declare_eval_funct("TwoPointDist" , two_points_distance_eval_funct, 4);
  return;
}

