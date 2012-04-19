/*                               -*- Mode: C -*- 
 * lang.c -- 
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

#include "stdio.h"

#include "macro.h"
#include "constant-pub.h"
#include "opaque-pub.h"
#include "lang.h"

Language_Type oprs_language = LANGUAGE;

#ifndef VXWORKS
void select_language(char *language_str, PBoolean resource)
{
     static PBoolean res_inited = FALSE;
     static Language_Type res_value = ENGLISH;
     if ( !language_str || ! strcasecmp(language_str,"en")){
	  oprs_language = ENGLISH;
/*	  fprintf(stderr, "Locale set from oprs to %s.\n",setlocale(LC_CTYPE,"en_US")); */
     } else if (!strcasecmp(language_str,"fr")){
	  oprs_language = FRENCH;
/* 	  fprintf(stderr, "Locale set from oprs to %s.\n",setlocale(LC_CTYPE,"fr")); */
     } else {
	  fprintf(stderr,LG_STR("Unknown language '%s', only 'en' and 'fr' are currently allowed.\n",
				"Langage '%s', seulement 'en' et 'fr' sont actuellement autorisé.\n"),language_str);
	  oprs_language = ENGLISH;
     }

     if (res_inited && (oprs_language != res_value)) {
	  fprintf(stderr,LG_STR("Language declaration '%s' is different from the one declared in the resource file: '%s'.\n",
				"Déclaration de langage '%s' est différent de celui déclaré dans le fichier de resource: '%s'.\n"),language_str, (res_value == FRENCH?"fr":"en"));
	  fprintf(stderr,LG_STR("you will get mixed language.\n",
				"vous allez avoir des mélanges de langues.\n"));
     }

     

     if (resource) {
	  res_inited = TRUE;
	  res_value = oprs_language;
     }

}
#endif
