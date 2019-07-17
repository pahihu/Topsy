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
#	File:                  $Source: /proj/topsy/ss98/Topsy/RCS/Makefile,v $
# 	Author(s):             
# 	Affiliation:           ETH Zuerich, TIK
# 	Version:               $Revision: 1.54 $
# 	Creation Date:         
# 	Last Date of Change:   $Date: 1998/04/07 16:50:58 $      by: $Author: fiedler $

# host platform
include Makefile.platform
# all architecture dependent information is here
include Makefile.mips

# several defines to be reused later
OBJ=$(ARCH).obj
DEBUG=-g
CCDEBUG=$(DEBUG) -ggdb
CFLAGS=$(MACHINE_CFLAGS) $(CCDEBUG) -Wall
DIRS=Topsy Memory IO IO/Drivers IO/Drivers/FPGA_Prog Threads Startup
ALLDIRS=$(DIRS) $(addsuffix /${ARCH}, $(DIRS)) 
INC=$(addprefix -I, $(ALLDIRS))
DEFS = -DDEBUG_LEVEL=2
OPTIMIZE = -O2

# here come the source files by module
ST=Startup
STFILES=$(ST)/Startup.c $(ST)/$(ARCH)/start.S
TO=Topsy
TOFILES=$(TO)/HashList.c $(TO)/List.c $(TO)/Support.c $(TO)/Syscall.c \
	$(TO)/$(ARCH)/SyscallMsg.S $(TO)/Error.c $(TO)/Lock.c \
	$(TO)/$(ARCH)/SupportAsm.S $(TO)/$(ARCH)/Exception.c
MM=Memory
MMFILES=$(MM)/MMInit.c $(MM)/$(ARCH)/MMDirectMapping.c \
	$(MM)/$(ARCH)/MMError.c $(MM)/MMHeapMemory.c $(MM)/MMMain.c \
	$(MM)/MMVirtualMemory.c $(MM)/$(ARCH)/tlb.S
TM=Threads
TMFILES=$(TM)/TMInit.c $(TM)/TMIPC.c $(TM)/TMMain.c \
	$(TM)/TMScheduler.c $(TM)/TMThread.c $(TM)/$(ARCH)/TMHal.c \
	$(TM)/$(ARCH)/TMError.c $(TM)/$(ARCH)/TMHalAsm.S $(TM)/$(ARCH)/TMClock.c
IO=IO
IOFILES=$(IO)/IOMain.c $(IO)/IODevice.c $(IO)/$(ARCH)/IOHal.c \
	$(IO)/IOConsole.c $(IO)/Drivers/SCN2681_DUART.c \
	$(IO)/Drivers/FPGA_Comm.c $(IO)/Drivers/FPGA_Prog/fpga_prog.c \
	$(IO)/Drivers/FPGA_Prog/fpgadata.c $(IO)/Drivers/Loopback.c \
	$(IO)/Drivers/intel8254.c

# here we define lists of all source and all object filenames (with path)
KERNELFILES=$(STFILES) $(TOFILES) $(MMFILES) $(TMFILES) $(IOFILES)
KERNELOBJS=$(KERNELFILES:.c=.o) $(KERNELFILES:.S=.o)
REALKERNELOBJS=$(addsuffix .o, $(addprefix $(OBJ)/, $(basename $(KERNELFILES))))


all: kernel user bootlinker bootlink

# here are the main rules for optimized C, non-optimized C, Assembler, and Java code
$(OBJ)/%.o: %.c $(OBJ)
	$(CC) $(CFLAGS) $(INC) $(DEFS) $(OPTIMIZE) -DSEGMAP=$(HEXSEGMAP) -c $< -o $@
${OBJ}/$(TM)/$(ARCH)/TMClock.o: $(OBJ)/%.o: %.c $(OBJ)
	$(CC) $(CFLAGS) $(INC) $(DEFS) -DSEGMAP=$(HEXSEGMAP) -c $< -o $@
$(OBJ)/%.o: %.S $(OBJ)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@
%.class: %.java
	javac -O $<

# this rule makes a tree of directories under mips.obj
$(OBJ):
	mkdir $(OBJ) $(addprefix $(OBJ)/, $(ALLDIRS))

# this builds the Topsy kernel
kernel: kernel.ecoff kernel.bin
kernel.ecoff: $(REALKERNELOBJS) link.scr
	$(LD) $(DEBUG) -o kernel.ecoff $(REALKERNELOBJS) -T link.scr
kernel.bin: $(REALKERNELOBJS) link.scr
	$(LD) $(DEBUG) -oformat binary -o kernel.bin $(REALKERNELOBJS) -T link.scr

# the user process has its own makefile
user:
	$(MAKE) -C User

# user and kernel need to be joined for download
bootlink:	kernel user
	$(SIZE) -Ax kernel.ecoff > kSize
	$(SIZE) -Ax user.ecoff > uSize
	(cd BootLinker; \
	java BootLinker ../kSize ../uSize ../user.srec $(SEGMAP) $(USERINKERNELAT); \
	mv segmap.bin .. )
	$(OBJCOPY) kernel.ecoff \
	--adjust-section-vma=".user"=+0x$(USERINKERNELAT) \
	--add-section=".user"=user.bin \
	--adjust-section-vma=".segmap"=+0x$(SEGMAP) \
	--add-section=".segmap"=segmap.bin \
	--target=ecoff-bigmips topsy.ecoff
	$(OBJCOPY) --target=srec topsy.ecoff topsy.srec	

# this is the tool which generates the segmap
bootlinker:	BootLinker/BootLinker.class

# use this to remove temporary files
clean:
	(cd User; $(MAKE) clean)
	$(RM) -rf $(OBJ)
	$(RM) -f kernel.bin kernel.ecoff kernel.srec \
		 user.ecoff user.bin user.srec \
		 segmap.bin segmap.srec \
		 topsy.ecoff topsy.srec \
		 kSize uSize segmap *~ *.bak 
	$(RM) -f Startup/*.bak Startup/*~ Startup/$(ARCH)/*.bak Startup/$(ARCH)/*~  Memory/*.bak Memory/*~ Memory/$(ARCH)/*.bak Memory/$(ARCH)/*~ Threads/*.bak Threads/*~ Threads/$(ARCH)/*.bak Threads/$(ARCH)/*~ IO/*.bak IO/*~ IO/$(ARCH)/*.bak IO/$(ARCH)/*~ Topsy/*~ Topsy/*.bak Topsy/$(ARCH)/*~ Topsy/$(ARCH)/*.bak BootLinker/*~ BootLinker/*.bak BootLinker/*.class IO/Drivers/*.bak IO/Drivers/*~

# this is a practical command to show some statistics on source and object size
size:
	wc -l Topsy/*.[ch] Startup/*.[ch] Startup/$(ARCH)/*.[chS] Memory/*.[ch] Memory/$(ARCH)/*.[chS] Threads/*.[ch] Threads/$(ARCH)/*.[chS] IO/*.[ch] IO/Drivers/*.[chS] IO/$(ARCH)/*.[chS]
	$(SIZE) kernel.ecoff $(REALKERNELOBJS) | sort -n -r
	$(SIZE) user.ecoff User/mips.obj/* | sort -n -r
	$(SIZE) -Ax topsy.ecoff

