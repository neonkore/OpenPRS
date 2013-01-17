/*                               -*- Mode: C -*- 
 * macro-pub.h -- Public macros used in OPRS.
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

#ifndef INCLUDE_macro_pub
#define INCLUDE_macro_pub

#include <sys/types.h>

/* 
 * This macro should be used whenever you want to create a new 
 * structure of type "type".
 */

void *malloc1(unsigned int nBytes);
void free1(void *free);

#define MAKE_OBJECT(type) (type *)malloc1(sizeof(type))

#define MAKE_OBJECT_STD(type) (type *)malloc(sizeof(type))

#define FREE_OBJECT(ptr) (free1(ptr))

#define FREE_OBJECT_STD(ptr) (free(ptr))

#define MALLOC_STD(x) (malloc(x))
#define FREE_STD(ptr) (free(ptr))

#define OPRS_MALLOC(x) (malloc1(x))
#define OPRS_REALLOC(x,y) (realloc1(x,y))
#define OPRS_FREE(ptr) (free1(ptr))

/* #define BITFIELDS(x) x */
#define BITFIELDS(x)

#endif /* INCLUDE_macro_pub */

#ifdef __cplusplus
}
#endif
