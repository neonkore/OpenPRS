static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * oprs-main.c -- 
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

#include "config.h"

#ifdef VXWORKS
#include "vxWorks.h"
#include "stdioLib.h"
#else
#include <stdio.h>
#endif

#include <stdlib.h>

#include "constant.h"
#include "oprs-main.h"

#include "oprs-type.h"
#include "oprs.h"
#include "parser-funct.h"
#include "macro.h"

#include "top-lev.h"
#include "oprs-type_f.h"
#include "oprs_f.h"
#include "top-lev_f.h"
#include "oprs-profiling_f.h"
#include "oprs-init_f.h"
#include "oprs-socket_f.h"
#include "user-external_f.h"

PBoolean install_user_trace;

#ifdef VXWORKS
#endif

#ifdef VXWORKS
int oprs_main(char *name_arg, char *server_hostname_arg, int server_port_arg,
	     char *mp_hostname_arg, int mp_port_arg, char *include_filename_arg)
#elif defined C_PLUS_PLUS_RELOCATABLE
int oprs_main(int argc, char **argv, char ** envp)
#else
int main(int argc, char **argv, char ** envp)
#endif
{
     char *server_hostn;
     int server_port;

     char *mp_hostn;
     int mp_port;

     char *name;

     socket_startup();

#if SETBUF_UNDEFINED
     setbuffer(stdout,NULL,0);
#else
     setbuf(stdout,NULL);
#endif

#ifdef VXWORKS
     name = name_arg;
     server_hostn = server_hostname_arg;
     mp_hostn = mp_hostname_arg;
     server_port = server_port_arg;
     register_to_server = ! (server_port_arg == 0);
     mp_port = mp_port_arg;
     register_to_mp = ! (mp_port_arg == 0);
     progname = "vxoprs";
     oprs_init_env();
     init_hash_tables();
     if (include_filename_arg && include_filename_arg[0])
	  add_initial_include_command(include_filename_arg);
#else
     progname = argv[0];
     oprs_init_env();
     name = oprs_init_arg(argc, argv, &mp_hostn, &mp_port, &server_hostn, &server_port);
#endif
     oprs_init_main(name, FALSE, mp_hostn, mp_port, server_hostn, server_port);

#ifndef VXWORKS
     FREE(mp_hostn);
     FREE(server_hostn);
#endif

     start_kernel_user_hook(name);

     run_initial_commands();

     client_oprs_top_level_loop(current_oprs); /* Just execute the main loop for the child */
     
     return(0);
}
