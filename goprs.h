/*                               -*- Mode: C -*- 
 * xoprs.h -- 
 * 
 * $Id$
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

#ifndef INCLUDE_xoprs
#define INCLUDE_xoprs

extern Widget xp_quitQuestion;
extern Widget xpTraceDialog;
extern Widget xpOptionDialog;
extern Widget addFactGoalDialog;
extern Widget consultOPDialog;
extern Widget consultAOPDialog;
extern Widget consultDBDialog;
extern Widget concludeDBDialog;
extern Widget deleteDBDialog;

extern Widget addfactGoalEntry;
extern Widget consultOPEntry;
extern Widget consultAOPEntry;
extern Widget consultDBEntry;
extern Widget concludeDBEntry;
extern Widget deleteDBEntry;
 
extern Widget xp_includeFilesel, xp_loaddbFilesel, xp_loadopFilesel, xp_reloadopFilesel;
extern Widget xp_unloadopFilesel, xp_savedbFilesel;
extern Widget xp_loadddbFilesel, xp_loaddopFilesel, xp_loadkrnFilesel, xp_dumpdbFilesel, 
     xp_dumpopFilesel, xp_dumpkrnFilesel;

#endif /* INCLUDE_xoprs */


