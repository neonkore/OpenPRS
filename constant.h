/*                               -*- Mode: C -*-
 * constant.h -- Defines some constants system-wise.
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

#ifndef INCLUDE_constant
#define INCLUDE_constant

#include "constant-pub.h"

#if defined(VXWORKS)
#define MAXPATHLEN MAX_FILENAME_LENGTH
#endif

#define LINSIZ 256

#ifndef HAS_BUFSIZ
#define BUFSIZ 1024
#endif

#define MAXARGS 20

#define OPRS_NAME_MAX_LENGTH 128

#define KILL_MP_NAME "kill-mp"
#define OPRS_SERVER_MP_NAME "oprs-server"
#define OP_EDITOR_MP_NAME "ope"

#define OPRS_PROGNAME "oprs"
#define OPRS_SERVER_PROGNAME "oprs-server"
#define X_OPRS_PROGNAME "xoprs"
#define MP_OPRS_PROGNAME "mp-oprs"

typedef enum {MP_PG, SERV_PG, OPE_PG, XP_PG, OPRS_PG, KILL_PG, OPC_PG} Prog_Type;

#define ASK_NAME_MESSAGE "who are you?"

#define MAX_HOST_NAME 128	/* Maximum length for an host name */

#ifndef MP_PORT
#define MP_PORT 3300
#endif

#ifdef ALLOWED_PORT
#undef MP_PORT
#define MP_PORT ALLOWED_PORT
#endif

#ifndef SERVER_PORT
#define SERVER_PORT 3400
#endif

#define COPYRIGHT_STRING "Copyright (c) 1991-2003 Francois Felix Ingrand."

#endif	/* INCLUDE_constant */

