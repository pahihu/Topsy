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
  
  File:                  $Source: /proj/topsy/ss98/Topsy/User/RCS/PacketDriverTest.c,v $
  Author(s):             Bernard Stauffer 
  Affiliation:           ETH Zuerich, TIK
  Version:               $Revision: 1.5 $
  Creation Date:         1.5.97
  Last Date of Change:   $Date: 1998/04/08 15:34:25 $      by: $Author: fiedler $
  
*/


#include "../Topsy/Topsy.h"
#include "../Topsy/Syscall.h"
#include "../IO/IOConsole.h"
#include "UserSupport.h"
#include "PacketDriverTest.h"


/* ---------------------------------------------------------------------------- */
/* A simple test programm, which helps you to debug your driver. A string is    */
/* sent to the driver and expected to be received. Some 'display'-functions     */
/* are included to enabled fast debugging.                                      */

void PacketDriverTest() {

  char                fromNet[25], s[5];
  ThreadId            tty, driver;
  unsigned long int   sizelen1, sizelen2, read_bytes, total_bytes; 

  char hello1[] = "\nTest PacketDriver\n";
  char hello2[] = "\nHello World over FPGA\n";


  sizelen1 = stringLength(hello1);
  sizelen2 = stringLength(hello2);

  ioOpen(IO_CONSOLE, &tty);
  ioInit(tty);
  ioOpen(IO_FPGA_COMM, &driver);
  ioInit(driver);

                      /* Writing the welcome message to console */
  ioWrite(tty, hello1, &sizelen1);

                      /* Writing the message to the net */
  if (ioWrite(driver, hello2, &sizelen2) == IO_WRITEFAILED) {
    display(tty,"ioWrite failed");
  }
  else {
    display(tty,"\nMessage sent to FPGA. Waiting to receive ...\n");
    read_bytes = 25;
    total_bytes = 0;
    while (total_bytes != sizelen2) {       /* Receiving the message from the net */
      if (ioRead(driver, fromNet, &read_bytes) == IO_READFAILED) {
        display(tty,"ioRead failed");
        read_bytes = 0;
      }
      else {
        total_bytes = total_bytes + read_bytes;   /* Count the received bytes */
        itoa(read_bytes, s);
        display(tty,s);
        display(tty," bytes received.\n");
      }
    }
                      /* Writing the received message to console */
    ioWrite(tty, fromNet, &total_bytes);

  }

}
