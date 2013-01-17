/*                               -*- Mode: C -*- 
 * oprs-sprint_f-pub.h -- 
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

#ifdef  __cplusplus
extern "C"  {
#endif

Sprinter *make_sprinter(int size);
void free_sprinter(Sprinter *sp);
void reset_sprinter(Sprinter *sp);
char *sprinter_string(Sprinter *sp);
int sprinter_size(Sprinter *sp);
int sprinter_remaining_size(Sprinter *sp);
char *sprinter_cur_pos(Sprinter *sp);
void enlarge_sprinter(Sprinter *sp, unsigned int minimum);
void add_sprinter_size(Sprinter *sp, int i);

#define USER_SPRINT(_sp,eval,_sprintf) \
do {char *f; int _real_size, _other_size;\
	if (sprinter_remaining_size(_sp) < (int)((eval)+1))\
	     enlarge_sprinter(_sp,eval+1);\
	f = sprinter_cur_pos(_sp);\
	_real_size = (int)num_char_sprint(_sprintf);\
	_other_size = strlen(f);\
	if (_real_size != _other_size)\
	     fprintf(stderr,"ERROR: sprintf lie to you (%d != %d) in the printer:\t%s.\n",\
		     _real_size, _other_size, sprinter_string(_sp));\
	if (_real_size > (int)(eval))\
	     fprintf(stderr,"ERROR: you did not reserve enough space (%d < %d) in the printer:\t%s.\n",\
		     eval, _real_size, sprinter_string(_sp));\
	add_sprinter_size(_sp, _other_size);\
} while (0)

#ifdef __cplusplus
}
#endif
