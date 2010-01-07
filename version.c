static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * version.c -- 
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

#include "config.h"

#include <stdio.h>

#include "lang.h"
#include "constant.h"
#include "version.h"


char *host = HOST;
char *date = COMPIL_DATE;
char *major_version = "1";
char *status = "Beta";
char *minor_version = "0";
char *patchlevel = "5";

void print_version()
{
     printf(LG_STR("\n\
\t%s\n\
\n\
\tVersion     : %s.%s\n\
\tCompiled on : %s\n\
\tDate        : %s\n\
\tPatch Level : %s\n\
\tStatus      : %s\n\
",
		   "\n\
\t%s\n\
\n\
\tVersion     : %s.%s\n\
\tCompil� sur : %s\n\
\tDate        : %s\n\
\tNiveau Patch: %s\n\
\tEtat        : %s\n\
"), COPYRIGHT_STRING, major_version, minor_version, host, date, patchlevel, status);
}



