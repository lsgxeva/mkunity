# Copyright (c) 1991-1998 Open Software Foundation, Inc. 
#  
# 
# 
# MkLinux
#

#
# Makefile for the linux server on top of the OSF/Mach3 micro-kernel.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definitions are now in the main makefile...

O_TARGET := chr_dev.o
O_OBJS   := adb.o char_dev.o atixlmouse.o busmouse.o console.o lp.o mem.o \
	misc.o mouse.o \
	msbusmouse.o psaux.o serial.o sunmouse.o \
	st.o tpqic02.o

include $(TOPDIR)/Rules.make
