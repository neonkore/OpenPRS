/*                               -*- Mode: C -*-
 * ope-graphic.h -- Defines some constants and structure for the op-editor.
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

#ifndef INCLUDE_ope_graphic
#define INCLUDE_ope_graphic

#include "constant.h"
#include "opaque.h"
#include "op-structure.h"

/* Some values and fraction of Pi. */
#define M_PI	3.14159265358979323846
#define M_PI_2	1.57079632679489661923
#define M_PI_4	0.78539816339744830962

#define MAX_NUMBER_LIGNES 64 /* I do not know... */

#define OP_TITLE_X 2;
#define OP_TITLE_Y 2;

/* Default size of the work area */
#define WORK_HEIGHT 1000
#define WORK_WIDTH 1500

/* Default Work font */
#define WORK_FONT "fixed"

/* Length in pixel of the arrow */
#define ARR_LENGTH 20

/* Angle of the arrow */
#define ARR_ANGLE (M_PI/12)

/* Maximum number of knots on one edge... */
#define MAX_KNOT 32

/* Size in pixel of the bounding box around a knot */
#define KNOT_SIZE 16

#define MAX3(a,b,c)     (((a) > (b)) ? (MAX(a,c)) : (MAX(b,c)))

#define EMPTY_STRING(s) ((s)[0]=='\0')

#define NOT_EMPTY_STRING(s) (!(EMPTY_STRING(s)))

typedef enum {XWD, XPM} DumpFormat;

/* Enumeration of the Drawing mode */
typedef enum {
     NOTHING = FALSE, DRAW_NODE, DRAW_EDGE, DRAW_KNOT, EDGE_SELECTED, NODE_SELECTED, EDIT_OG, DESTROY_OG, MOVE_OG,
     MOVING_OG, MOVING_CANVAS, CONVERT_END,  CONVERT_START, ALIGN_OG, ALIGNING_OG, DRAW_IFNODE,
     ALIGN_OG_V, ALIGNING_OG_V, ALIGN_OG_H, ALIGNING_OG_H, DUPLICATE_OBJECTS, DUPLICATING_NODE,
     DUPLICATING_EDGE, DUPLICATING_EDGE2, MERGE_NODE, MERGING_NODE, FLIP_SPLIT, FLIP_JOIN,
     OPEN_NODE, DM_RELEVANT_OP,
     MAX_DRAW_MODE
}    Draw_Mode;

PString mode_help[MAX_DRAW_MODE];

/* Enumeration of the Text Field type */
typedef enum {
     TT_INVOCATION, TT_CONTEXT, TT_CALL, TT_SETTING, TT_PROPERTIES, TT_DOCUMENTATION, TT_EFFECTS, TT_ACTION, TT_BODY, TT_TEXT_NONE
}    Text_Type;

typedef struct gnode {
     Node *node;
     XmString xmstring;
     Dimension swidth, sheight;
} Gnode; /* Graphic Node structure */

typedef struct gknot {
     Position x, y;
     OG *edge;
} Gknot; /* Graphic Knot structure */

typedef Slist *List_Knot;

typedef struct gtext_string {
     Dimension off_x, off_y;
     XmString xmstring;
} Gtext_String;
     
typedef Slist *List_Gtext_String;

typedef struct gedge_text {
     Position dx, dy;
     PString log_string;
     List_Gtext_String lgt_log_string;
     Dimension text_width;
     PBoolean fill_lines;
     OG *edge;
} Gedge_text; /* Graphic Edge Text structure */

typedef struct gtext {
     PString string;
     List_Gtext_String lgt_string;
     Dimension text_width;
     PBoolean fill_lines;
     PBoolean visible;
     Text_Type text_type;
     List_OG      	list_og_inst;
} Gtext; /* Graphic Text structure */

typedef struct gedge {
     Edge *edge;
     OG *text;
     Position x1, y1, x2, y2;
     Position fx1, fx2, fy1, fy2;
     List_Knot list_knot;
} Gedge; /*Graphic Edge structure */

typedef struct ginst {
     Instruction *inst;
     Edge *edge;        /* For X-Oprs */
     int num_line, nb_lines, indent, width; /* Temporay informations saved 
					       while pretty-printing the text body */
} Ginst; /* Graphic Instruction structure */

struct og {
     Draw_Type type;
     Position x, y;
     Dimension width, height;
     Region region;
     PBoolean selected;
     union {
	  Gnode *gnode;
	  Gedge *gedge;
	  Gedge_text *gedge_text;
	  Gtext *gtext;
	  Gknot *gknot;
	  Ginst *ginst;
     }     u;
}; /* Object Graphic structure */

typedef struct copy_area_index {
     int top, left;
} Copy_Area_Index;

struct draw_data {
     int start_x, start_y, last_x, last_y, dx, dy;
     GC gc;				  /* default GC       */
     GC sgc;				  /* selected GC       */
     GC xorgc;				  /* xor GC       */
     PBoolean just_compiling;
     Window window;
     Widget vscrollbar;
     Widget hscrollbar;
     Widget canvas;
     Dimension canvas_height;		  /* canvas dimensions     */
     Dimension canvas_width;
     Dimension work_height;
     Dimension work_width;
     int top;				  /* top */
     int left;				  /* left */
     XFontStruct *font;			  /* The font struct       */
     XmFontList fontlist;
     Draw_Mode mode, old_mode;
     OG *edited_og;
     OG *node_selected;
     OG *second_node_selected;
     OG *edge_selected;
     OG *og_moving;
     OG *og_align;
     OG *og_aligning;
     OG *og_selected_on_press;
     OG *sensitive_og;
     Region expose_region;
     Op_Structure *op;
     List_Knot list_knot;
     unsigned int pressed_button;
     Squeue *copy_area_index_queue;
};

typedef Slist *ListOPFile;

typedef Slist *ListOP;

typedef Slist *ListSymbol;

typedef enum {
     GRASPER_GRAPH, SUN_GRAPH, ACS_GRAPH
}    Opf_Type;

typedef struct {
     PString name;
     ListOP list_op;
     PBoolean modified;
     PBoolean filed;
     PBoolean backed_up;
     Opf_Type type;
     ListSymbol list_sym;
} OPFile;

typedef Slist *ListLastSelectedOP;

extern int node_x, node_y, visible , text_dx, text_dy, string_width, fill_lines;
extern Slist *edge_location;
extern Draw_Data *global_draw_data;

#endif
