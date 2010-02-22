static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * shary-user-ev-function.c -- contains user defined evaluable functions.
 * 
 * Copyright (c) 1991-2010 Francois Felix Ingrand.
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

#ifdef VXWORKS
#include "vxWorks.h"
#include "stdioLib.h"
#else
#include <stdio.h>
#endif

#include <math.h>

#include "macro-pub.h"
#include "opaque-pub.h"
#include "constant-pub.h"
#include "oprs-type-pub.h"

#include "shary-user-ev-function.h"

#include "oprs-type_f-pub.h"
#include "lisp-list_f-pub.h"
#include "oprs-rerror_f-pub.h"
#include "oprs-error.h"
#include "oprs-rerror.h"
#include "oprs-rerror_f.h"
#include "oprs-error_f.h"

Term *distance_eval_func(TermList terms)
{

  Term *pos1x, *pos1y, *pos2x, *pos2y, *res;  
  
  pos1x = (Term *)sl_get_slist_pos(terms, 1);
  pos1y = (Term *)sl_get_slist_pos(terms, 2);
  pos2x = (Term *)sl_get_slist_pos(terms, 3);
  pos2y = (Term *)sl_get_slist_pos(terms, 4);
  
  if ((pos1x->type != TT_FLOAT) || (pos1y->type != TT_FLOAT) || (pos2x->type != TT_FLOAT) || (pos2y->type != TT_FLOAT))  {
	 fprintf(stderr,"Expecting a  position \n");
	 return res;
  }
  
  float x1 = *pos1x->u.doubleptr;
  float y1 = *pos1y->u.doubleptr;
  float x2 = *pos2x->u.doubleptr;
  float y2 = *pos2y->u.doubleptr;

  float distance=0.0;

  distance = sqrt(pow(x1-x2,2)+pow(y1-y2,2));

  res=build_float(distance);
  
  return res;
}

double compute_theta(double robot_x, double robot_y, double human_x, double human_y);

Term *compute_human_theta_toward_robot_eval_func(TermList terms)
{

  Term *robotx, *roboty, *humanx, *humany, *res;  
  
  robotx = (Term *)sl_get_slist_pos(terms, 1);
  roboty = (Term *)sl_get_slist_pos(terms, 2);
  humanx = (Term *)sl_get_slist_pos(terms, 3);
  humany = (Term *)sl_get_slist_pos(terms, 4);
  
  if ((robotx->type != TT_FLOAT) || (roboty->type != TT_FLOAT) || (humanx->type != TT_FLOAT) || (humany->type != TT_FLOAT))  {
	 fprintf(stderr,"Expecting a  position \n");
	 res=build_float(0.0);
	 return res;
  }
  
  double robot_x = *robotx->u.doubleptr;
  double robot_y = -*roboty->u.doubleptr;
  double human_x = *humanx->u.doubleptr;
  double human_y = -*humany->u.doubleptr;

  double human_th = -compute_theta(robot_x,robot_y,human_x,human_y);

  res=build_float(human_th);
  
  return res;
}

double compute_theta(double robot_x, double robot_y, double human_x, double human_y) 
{

  double result;
  ;;
  double  coefDir = (robot_y-human_y)/(robot_x-human_x);
  double  angle = atan(coefDir);

  if ((human_x == robot_x) && (human_y != robot_y)) {
	 if (human_y > robot_y) {
		result=-1.57;
	 } else {
		result=1.57;
	 }
  } else if ((human_y == robot_y) && (human_x != robot_x)) {
	 if (human_x > robot_x) {
		result=3.14;
	 } else {
		result=0.0;
	 }	 
  } else if ((human_x == robot_x) && (human_y == robot_y)) {
	 result=0.0;
  } if ((human_x > robot_x) && (human_y > robot_y)) {
	 result=-3.14+angle;
  } else if ((human_x < robot_x) && (human_y > robot_y)) {
	 result=angle;
  } else if ((human_x > robot_x) && (human_y < robot_y)) {
	 result=3.14+angle;
  } else if ((human_x < robot_x) && (human_y < robot_y)) {
	 result=angle;
  } 

  result=-result;

  return result;
}

void declare_shary_user_eval_funct()
{
     make_and_declare_eval_funct("DISTANCE",distance_eval_func, 4);

     make_and_declare_eval_funct("HUMAN-THETA-TOWARD-ROBOT",compute_human_theta_toward_robot_eval_func, 4);


     return;
}
