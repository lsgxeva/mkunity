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
#
# MkLinux

VPATH	= ${MAKETOP}mach_kernel/i386/${TARGET_MACHINE}/boot

DATAFILES		= ${BOOT_DATAFILES}

INCLUDES		= ${DATAFILES}

EXPDIR			= /mach_kernel/i386/boot/

.if     exists(../${TARGET_MACHINE}/boot/boot.mk)
.include "../${TARGET_MACHINE}/boot/boot.mk"
.endif
.include <${RULES_MK}>
