/*                               -*- Mode: C -*- 
 * xoprs-intention.h -- 
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


#ifndef INCLUDE_xp_intention
#define INCLUDE_xp_intention

#include "opaque.h"

#define IDD_WORK_HEIGHT 500
#define IDD_WORK_WIDTH 1000

#define I_ARR_LENGTH 12
#define I_ARR_ANGLE (M_PI/8)

#define INT_WIDTH 40

typedef struct ginode Ginode;
typedef struct giedge Giedge;

struct iog {
     IDraw_Type type;
     Position x,y;
     Dimension width,height;
     Region region;
     PBoolean placed,text_changed;
     union {
	  Ginode *ginode;
	  Giedge *giedge;
     }u;
};

struct ginode {
     Intention *intention;
     List_Gtext_String lgt_string;
     List_IOG connect;
};

struct giedge {
     Intention *in, *out;
     Position x1,y1,x2,y2;
     Position fx1,fx2,fy1,fy2;
};

struct int_draw_data {
     Intention_Graph *ig;
     int          start_x, start_y;
     GC            gc;		/* default GC       */
     GC            sgc;		/* reversed GC       */
     Widget        vscrollbar;
     Widget        hscrollbar;
     Widget        canvas;
     Dimension     canvas_height; /* canvas dimensions     */
     Dimension     canvas_width;
     Dimension     work_height;
     Dimension     work_width;
     Region	expose_region;
     int           top;		/* top */
     int           left;	/* left */
     XFontStruct  	*font;	/* The font struct       */
     XmFontList    fontlist;
     Squeue *copy_area_index_queue;
     PBoolean reposition_all;
};

#endif /* INCLUDE_xp_intention */
