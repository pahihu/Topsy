/*
  Copyright (c) 1996-1997 Swiss Federal Institute of Technology, 
  Computer Engineering and Networks Laboratory. All rights reserved.

  TOPSY - A Teachable Operating System. 
    Implementation of a tiny and simple micro kernel for
    teaching purposes.

  Permission to use, copy, modify, and distribute this software and its
  documentation for any purpose, without fee, and without written 
  agreement is hereby granted, provided that the above copyright notice 
  and the following two paragraphs appear in all copies of this software.

  
  File:                  $Source: /proj/topsy/ss98/Topsy/IO/Drivers/RCS/FPGA_Comm.h,v $
  Author(s):             Bernard Stauffer
  Affiliation:           ETH Zuerich, TIK
  Version:               $Revision: 1.3 $
  Creation Date:         1997/05/02 20:34:00
  Last Date of Change:   $Date: 1997/05/19 22:42:32 $      by: $Author: stauffer $
*/
  
  
#include "Topsy.h"
#include "MMHeapMemory.h"
#include "IODevice.h"
#include "IDT385_IOMap.h"
#include "Configuration.h"


/*----------------------------------------------------------------*/        

#define FPGA_DATAPUT    FPGA_BASE + 0x0   /* FPGA board register layout */
#define FPGA_STATUSPUT  FPGA_BASE + 0x4
#define FPGA_DATAGET    FPGA_BASE + 0x8
#define FPGA_STATUSGET  FPGA_BASE + 0x0c

#define FPGA_BUFSIZE 1024                 /* Size of receiving buffer */


typedef struct devFPGADesc_t {
  unsigned long in, out;
} devFPGADesc;


typedef devFPGADesc* devFPGA;


void devFPGA_interruptHandler(IODevice this);
Error devFPGA_init(IODevice this);
Error devFPGA_read(IODevice this, ThreadId threadId, char* buffer, long int* size);
Error devFPGA_write(IODevice this, ThreadId threadId, char* buffer, long int* size);
Error devFPGA_close(IODevice this);
