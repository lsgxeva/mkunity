#
# Copyright 1991-1998 by Open Software Foundation, Inc. 
#              All Rights Reserved 
#  
# Permission to use, copy, modify, and distribute this software and 
# its documentation for any purpose and without fee is hereby granted, 
# provided that the above copyright notice appears in all copies and 
# that both the copyright notice and this permission notice appear in 
# supporting documentation. 
#  
# OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE 
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
# FOR A PARTICULAR PURPOSE. 
#  
# IN NO EVENT SHALL OSF BE LIABLE FOR ANY SPECIAL, INDIRECT, OR 
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
# LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT, 
# NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
# WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
#
# MkLinux

PURE_MACH		= 1

VPATH	= ${MAKETOP}mach_services/include/servers:${${TARGET_MACHINE}_VPATH}

PROGRAMS 		= bootstrap

ILIST			= bootstrap bootstrap.template
IDIR			= ${MACH3_ROOT_SERVERS_IDIR}

NO_STARTFILES		=
USE_STATIC_LIBRARIES	=

LIBS			= -lsa_fs -lcthreads -lmach -lsa_mach -lmach \
				${${TARGET_MACHINE}_LIBS}
LDFLAGS			= ${SA_MACH_LDFLAGS} ${${TARGET_MACHINE}_LDFLAGS}

CFLAGS			= ${${OBJECT_FORMAT}_EXTRA_WARNINGS}

serviceServer.o_CFLAGS	=

INCFLAGS		= -I. -I${MACHINE}

MIG_DEFS		= service.defs

OFILES			= bootstrap.o service.o serviceServer.o \
			  load.o \
			  ${${TARGET_MACHINE}_OFILES}

DATAFILES		= bootstrap.template

DEPENDENCIES		=

.include "${TARGET_MACHINE}/machdep.mk"
.include <${RULES_MK}>

.if exists(depend.mk)
.include "depend.mk"
.endif
