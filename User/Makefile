#
#	Copyright (c) 1996-1997 Swiss Federal Institute of Technology, 
#	Computer Engineering and Networks Laboratory. All rights reserved.
#
#	TOPSY - A Teachable Operating System. 
#		Implementation of a tiny and simple micro kernel for
#		teaching purposes.
#
#	Permission to use, copy, modify, and distribute this software and its
#	documentation for any purpose, without fee, and without written 
#	agreement is hereby granted, provided that the above copyright notice 
#	and the following two paragraphs appear in all copies of this software.
#
#	File:                  $Source: /proj/topsy/ss98/Topsy/User/RCS/Makefile,v $
# 	Author(s):             
# 	Affiliation:           ETH Zuerich, TIK
# 	Version:               $Revision: 1.37 $
# 	Creation Date:         
# 	Last Date of Change:   $Date: 1998/04/08 15:22:54 $      by: $Author: fiedler $
#	

# host platform
include ../Makefile.platform
# target
include ../Makefile.mips

# the usual options and defines, includes for the syscall library
DEBUG=-g
CCDEBUG=-ggdb
OBJ=$(ARCH).obj
CFLAGS=-G -Wall $(CCDEBUG) 
LDFLAGS=$(DEBUG)
INC=-I../Topsy -I../Topsy/$(ARCH) -I../Memory -I../IO -I../Threads
OPTIMIZE = -O2

# syscall library objects; the same code as produced for the kernel is reused here
SYSCALL=../$(OBJ)/Topsy/Syscall.o ../$(OBJ)/Topsy/$(ARCH)/SyscallMsg.o

# all the user files (shell plus your stuff)
SHELLFILES= Shell.c 
USERFILES=  UserSupport.c \
	    SeatReservation.c \
	    testtimer.c Demo.c HelloWorld.c ThreadsDemo.c CrashMe.c
#	PacketDriverTest.c datafile.c IRQControl.c \
#	PacketStackControl.c Packetizer.c PacketFraming.c

# list of all object files
SHELLOBJS=$(SHELLFILES:.c=.o) 
USEROBJS=$(USERFILES:.c=.o)
ALLOBJS=$(SHELLOBJS) $(USEROBJS)
REALOBJS=$(addprefix $(OBJ)/, $(notdir $(ALLOBJS)))


all: initMake user

initMake: $(OBJ)

# main rules for C programs
$(OBJ)/%.o: %.c $(OBJ)
	$(CC) $(CFLAGS) $(INC) $(OPTIMIZE) -c $< -o $@

# shell and user targets (simulator, debugger, and linker need different obj formats)
user:   $(REALOBJS) $(SYSCALL) link.scr
	$(LD) $(LDFLAGS) -o ../user.srec $(OBJ)/*.o $(SYSCALL) -T link.scr
	$(LD) $(LDFLAGS) -o ../user.bin -oformat binary  $(OBJ)/*.o $(SYSCALL) -T link.scr
	$(LD) $(LDFLAGS) -o ../user.ecoff -oformat ecoff-bigmips $(OBJ)/*.o $(SYSCALL) -T link.scr

# this rule makes mips.obj/
$(OBJ):
	mkdir $(OBJ)

# remove temporary stuff
clean:	
	$(RM) -f ../user ../user.srec *.bak *~
	$(RM) -rf $(OBJ)










