#	$LAAS: robots.m4,v 1.5 2004/07/22 08:23:14 mallet Exp $

#
# Copyright (c) 2002-2003 LAAS/CNRS                   --  Fri Mar 15 2002
# All rights reserved.
#
# Redistribution  and  use in source   and binary forms,  with or without
# modification, are permitted provided that  the following conditions are
# met:
#
#   1. Redistributions  of  source code must  retain  the above copyright
#      notice, this list of conditions and the following disclaimer.
#   2. Redistributions in binary form must  reproduce the above copyright
#      notice,  this list of  conditions and  the following disclaimer in
#      the  documentation   and/or  other  materials   provided with  the
#      distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE  AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY  EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES   OF MERCHANTABILITY AND  FITNESS  FOR  A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO  EVENT SHALL THE AUTHOR OR  CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING,  BUT  NOT LIMITED TO, PROCUREMENT  OF
# SUBSTITUTE  GOODS OR SERVICES;  LOSS   OF  USE,  DATA, OR PROFITS;   OR
# BUSINESS  INTERRUPTION) HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE  USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

dnl --- Look for the laas mkdep executable ------------------------------
dnl --- I grabbed it from robots.m4 in Genom


AC_DEFUN([ROBOT_PROG_MKDEP],
[
   AC_PATH_PROG(MKDEP, mkdep, no, $exec_prefix/bin:$prefix/bin:$PATH)
   if test "$MKDEP" = "no"; then
      AC_MSG_ERROR([You need the mkdep package])
   fi

   AC_CACHE_CHECK(
      [whether mkdep accepts -I/-D options], ac_cv_robot_mkdep,
      [
         if $MKDEP -I foo -D bar 1>/dev/null 2>&1; then
	    ac_cv_robot_mkdep=yes;
	 else
	    ac_cv_robot_mkdep=no;
         fi
      ])
   if test x"${ac_cv_robot_mkdep}" = xno; then
      AC_MSG_ERROR([You need the mkdep package])
   fi 
])
