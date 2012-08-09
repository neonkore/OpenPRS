/*                               -*- Mode: C -*- 
 * oprs-dump_f.h -- 
 * 
 * $Id$
 * 
 * Copyright (c) 1991-2012 Francois Felix Ingrand.
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


void add_object_to_dump(Dump_Type dt, void *addr);
int dump_object_list(void);
int start_dump_session(char *filename);
int start_load_session(char *filename);
void end_dump_session(void);
void end_load_session(void);
void flush_dump_session(void);
void flush_load_session(void);
PBoolean load_all_objects(void);
void *load_object(Dump_Type *return_type);
char load_char(void);
void *load_list_expr(void);
void *load_list_type(void);
void dump_list_type(TypeList types);
void *load_list_op(void);
Intention_Graph *load_ig(void);
unsigned char load_uchar(void);
void *load_ptr(void);
void reference_object(void *old_addr, void **addr_ref, Dump_Type dt);

#define load_boolean load_char

#define LOAD_ADDR_AND_REF_LOC_ADDR(type,loc_addr)	\
do {void *addr = load_ptr(); \
     if (addr) reference_object(addr,(void **)loc_addr,type);	\
    else *(loc_addr) = addr; \
} while(0)

#define LOAD_ADDR_AND_REF_LOC(type,loc) LOAD_ADDR_AND_REF_LOC_ADDR(type,&(loc))

void *load_list_reloc_elt(Dump_Type dt);
void ntohd(u_char *buf, double *dbl);
void htond(double *dbl, u_char *buf);
void ntohll(u_char *buf, long long *ll);
void htonll(long long *ll, u_char *buf);
void ntoh64(u_char *buf, void *ll);
void hton64(void *ll, u_char *buf);

#ifdef _LP64
void htonptr(void *ptr, uchar buf[8]);
void ntohptr(u_char *buf, uchar ptr[8]);
#else
void htonptr(void *ptr, void **buf);
void ntohptr(void *buf, void **ptr);
#endif

int dump_ptr(void *ptr);
int dump_char(char c);
void dump_list_expr(ExprList exprs_to_dump);
void dump_list_op(Op_List opl);
void dump_ig(Intention_Graph *ig);
int dump_short(short s);
int dump_ushort(unsigned short s);

#define dump_boolean(bool) dump_char(bool)

#define DUMP_LIST_RELOC_ELT(list,type,dt_type) \
	do { \
            type object; \
            dump_ushort(list_length(list)); \
            dump_short(sl_slist_type(list)); \
	    sl_loop_through_slist(list,object,type) { \
               WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(dt_type,object); \
	    } \
	} while (0)

#define WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(type,addr) \
	do {dump_ptr(addr); \
            add_object_to_dump(type,addr); \
	} while(0)
