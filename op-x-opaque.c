/*                               -*- Mode: C -*- 
 * op-x-opaque.c -- Contains opaque functions to prevent the link with X libraries.
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

/* This file contains a number of function with no definition which are not supposed to be called in any case... They are
   just here to please the linker... */

#include "config.h"
#include "oprs-type.h"
#include <stdio.h>

int XmStringCreate()
{
     return 0;
}

int XmStringExtent()
{
     return 0;
}

int ope_string_to_xmstring()
{
     return 0;
}

int ope_string_to_lgt_string()
{
     return 0;
}

int free_lgt_string()
{
     return 0;
}

void position_edge()
{
}

void position_then_else()
{
}

int make_op_title()
{
     return 0;
}

void update_canvas_size()
{
}

int XUnionRectWithRegion()
{
     return 0;
}

int XCreateRegion()
{
     return 0;
}

int XDestroyRegion()
{
     return 0;
}

int XmStringFree()
{
     return 0;
}

void clear_op_graphic()
{
}

void clear_specified_op_graphic()
{
}

void report_syntax_error()
{
}

void build_field_graphic()
{
}

void build_edge_graphic()
{
}

int make_og_text_field()
{
     return 0;
}

void make_node_graphic()
{
}

void make_inst_graphic()
{
}

int make_og_edge()
{
     return 0;
}

void *make_xoprs_ut_window(void)
{
     return NULL;
}

void manage_ut_window(void)
{
}

void unmanage_ut_window(void)
{
}

void delete_ut_window(void)
{
}

void AppendTextWindow(void *w, char *s, PBoolean p)
{
 printf("%s",s);
}

void rename_ut_window(void *w, char *title)
{
}
