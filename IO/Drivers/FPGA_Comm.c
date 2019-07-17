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


  File:                  $Source: /proj/topsy/ss98/Topsy/IO/Drivers/RCS/FPGA_Comm.c,v $
  Author(s):             Bernard Stauffer
  Affiliation:           ETH Zuerich, TIK
  Version:               $Revision: 1.3 $
  Creation Date:         1997/05/02 20:34:00
  Last Date of Change:   $Date: 1997/05/19 22:42:49 $      by: $Author: stauffer $
*/


#include "Messages.h"
#include "IODevice.h"
#include "IOConsole.h"
#include "FPGA_Comm.h"
#include "FPGA_Prog/fpga_prog.h"


volatile char *ptr_fpga;


/*----------------------------------------------------------------*/        
/* Driver's private functions */

char fpga_readStatusGet(void) {
  ptr_fpga = (char*) FPGA_STATUSGET;
  return *ptr_fpga;
}

char fpga_readStatusPut(void) {
  ptr_fpga = (char*) FPGA_STATUSPUT;
  return *ptr_fpga;
}

void fpga_putData(char c) {
  ptr_fpga = (char*) FPGA_DATAPUT;
  *ptr_fpga = c;
  ioDelayAtLeastCycles(50);         /* THIS WAIT IS VERY IMPORTANT */
}

char fpga_getData(void) {
  ptr_fpga = (char*) FPGA_DATAGET;
  return *ptr_fpga;
}


/*----------------------------------------------------------------*/        
void devFPGA_interruptHandler(IODevice this) {

  devFPGA   fpga_comm;
  char      lost;

  fpga_comm = (devFPGA) this->extension;
  
  while (fpga_readStatusGet() != 0) {     /* Read as long data are present */

    if (((fpga_comm->in + 1) % FPGA_BUFSIZE) == fpga_comm->out) {
      lost = fpga_getData();              /* We have to get the data, even if full */
      WARNING("\nFPGA COMM receive overflow\n");
    }
    else {                                /* If buffer not full, copy data */
      this->buffer[fpga_comm->in] = fpga_getData();
      fpga_comm->in = ((fpga_comm->in + 1) % FPGA_BUFSIZE);
    }
    
  }
}


/*----------------------------------------------------------------*/        
Error devFPGA_read(IODevice this, ThreadId threadId, char* buffer, 
                   long int* size) 
{
  devFPGA fpga_comm;
  int     i = 0;


  fpga_comm = (devFPGA) this->extension;

  while (TRUE) {  
          
                        /* Stop, if no more characters available (buffer empty) */
    if (fpga_comm->in == fpga_comm->out) {
      break;
    }
                        /* Stop, if user is satisfied */
    if (i >= *size) {
      break;
    }
                        /* Copy data and increment data pointer */
    buffer[i++] = this->buffer[fpga_comm->out];
    fpga_comm->out = ((fpga_comm->out + 1) % FPGA_BUFSIZE);

  }

  *size = i;            /* Set the actual value of read chars */

  return IO_READOK;
}


        
/*----------------------------------------------------------------*/        
Error devFPGA_write(IODevice this, ThreadId threadId, char* buffer,
                    long int* size) 
{
  int i;

  for (i = 0; i < *size; i++) {
    while(fpga_readStatusPut() == 0) {};  /* Wait for FPGA board to become ready */

    fpga_putData(buffer[i]);              /* Send it */
  }
  
  return IO_WRITEOK;
}


        
/*----------------------------------------------------------------*/        
Error devFPGA_init(IODevice this) { 

  devFPGA fpga_comm;
  
 
  if (!this->isInitialised) {

    hmAlloc((Address*)&(this->buffer), FPGA_BUFSIZE);           /* Allocate buffer and */
    hmAlloc((Address*)&(this->extension), sizeof(devFPGADesc)); /* ... descriptor space */
 
    InitLinkBOARD();                /* Programs and initializes the FPGA comm interface */  

    this->isInitialised = TRUE;     /* Set Initialized flag */
  }

  fpga_comm = (devFPGA)this->extension;   /* Hook drivers vars in descriptor */
  fpga_comm->in = 0;                      /* Reset buffer pointers to initial value */
  fpga_comm->out = 0;

  return IO_INITOK;

}


        
/*----------------------------------------------------------------*/        
Error devFPGA_close(IODevice this) {
 
  /* this message is forwarded from ioThread */
  /* release buffers if any */
    

  if (this->isInitialised) {

    hmFree((Address)this->buffer);      /* Deallocate buffer space */
    hmFree((Address)this->extension);   /* Deallocate descriptor space */

    this->isInitialised = FALSE;        /* Set Initialized flag */
  }

  return IO_CLOSEOK;
}



/*----------------------------------------------------------------*/        
