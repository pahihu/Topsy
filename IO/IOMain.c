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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/IO/RCS/IOMain.c,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.30 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/04/08 15:10:08 $      by: $Author: gfa $
	
	
*/

#include "IO.h"
#include "IOMain.h"
#include "IODevice.h"
#include "Syscall.h"
#include "Threads.h"
#include "Exception.h"

static void ioLoadDriver(int number, IODevice dev);

/* static device table with all the driver entry points
 * (similar to Unix' device switching table)
 * this could be replaced/enhanced later by a dynamic 
 * driver loader mechanism
 */ 
static IODeviceDesc ioDeviceTable[IO_DEVCOUNT] = {
	{ "ttya", (Address)UART_A_BASE, 
	    NULL, /* buffer must be allocated by driver */
	    DUART, /* interrupt */
	    (InterruptHandler)devSCN2681_interruptHandler, /* ttya and b */
	    devSCN2681_read, devSCN2681_write, 
	    devSCN2681_close, devSCN2681_init, 
	    NULL /* private msg handler */,
	    FALSE, NULL /* data extension */ },
		
	{ "ttyb", (Address)UART_B_BASE, NULL, 
	    -1, NULL, 
	    devSCN2681_read, devSCN2681_write, 
	    devSCN2681_close, devSCN2681_init, 
	    NULL /* private msg handler */,
	    FALSE, NULL /* data extension */ },

	{ "fpga", (Address)FPGA_BASE,
	    NULL, /* buffer must be allocated by driver */
	    FPGA_CARD, /* interrupt */
	    (InterruptHandler)devFPGA_interruptHandler,
	    devFPGA_read, devFPGA_write, 
	    devFPGA_close, devFPGA_init, 
	    NULL /* private msg handler */,
	    FALSE, NULL /* data extension */ },

	{ "loopback", (Address)0, NULL, 
	    -1, NULL, 
	    devLoopback_read, devLoopback_write, 
	    devLoopback_close, devLoopback_init, 
	    NULL /* private msg handler */,
	    FALSE, NULL /* data extension */ } /*,

        { "timer1", (Address)COUNTER1,
	        NULL,*/ /* no buffer */
	/*CLOCKINT_1,*/ /* interrupt */
	                /* count down timer interrupt handler */
	                /* read */ 
	                /* write */
	                /* close */ 
	                /* init */
	                /* private msg handler */
	     /*FALSE,*/ /* init */ 
	       /*NULL*/ /* data extension */ /* } */
};

static HashList ioDevices;


/* IO thread main procedure */
void ioMain(ThreadArg arg) {

    int i;
    Error error;
    ThreadId t;
    ThreadId expectedFrom;
    Message msg, reply;
    DriverMessage driverMsg;
        
    ioDevices = hashListNew();


    /* using ioConsolePutString() here can possibly generate "garabage" 
       on screen while interrupt-handler table entries are entered
       concurrently in ioLoadDriver(), 03/31/98 Gries */

    ioConsolePutString("ioThread: loading ");
    ioConsolePutInt(IO_DEVCOUNT);
    ioConsolePutString(" drivers...\n");

    /* due to the centralized interrupt handling in certain UARTs we
     * need to know each other
     */
    ioDeviceTable[IO_SERIAL_A].extension = &(ioDeviceTable[IO_SERIAL_B]);

    for (i = 0; i < IO_DEVCOUNT; i++) {
        //ioConsolePutString("[");
        //ioConsolePutString(ioDeviceTable[i].name);
        //ioConsolePutString("] ");
        //ioDelayAtLeastCycles(1000);
        //ioConsolePutString("\n");
	if (ioDeviceTable[i].init != NULL) {
		ioDeviceTable[i].init( &(ioDeviceTable[i]) );
	}
    }
    for (i = 0; i < IO_DEVCOUNT; i++) {
	ioLoadDriver(i, &(ioDeviceTable[i]));
    }  
    
    while (TRUE) {
	expectedFrom = ANY;
	tmMsgRecv(&expectedFrom, ANYMSGTYPE, &msg, INFINITY);
	switch (msg.id) {
	case IO_OPEN:
	    error = hashListGet(ioDevices,  
			    	(void**)(&t), msg.msg.ioOpen.deviceNumber);
	    reply.id = IO_OPENREPLY;
	    if (error == HASHNOTFOUND) {
		reply.msg.ioOpenReply.errorCode = IO_OPENFAILED;
	    }
	    else {
		reply.msg.ioOpenReply.errorCode = IO_OPENOK;
	    }					
	    reply.msg.ioOpenReply.deviceThreadId = t;
	    tmMsgSend(msg.from, &reply);
	    break;
	case IO_CLOSE:
	    /* msg is forwarded to driver thread or fails if the id is wrong */
	    driverMsg.id = IO_DRIVERCLOSE;
	    driverMsg.clientThreadId = msg.from;
	    error = tmMsgSend(msg.msg.ioClose.deviceThreadId, 
						    (Message*)&driverMsg);
	    if (error != TM_MSGSENDOK) {
		reply.id = IO_CLOSEREPLY;
		reply.msg.ioCloseReply.errorCode = IO_CLOSEFAILED;
		tmMsgSend(msg.from, &reply);
	    }
	    /* drivers actions: send reply to syscaller, clean up */
	    break;
	case UNKNOWN_SYSCALL:
	    /* possible reply on garbage IO_CLOSE forward */
	    break;
	default:
	    /* return error */
	    reply.id = UNKNOWN_SYSCALL;
	    tmMsgSend(msg.from, &reply);
	}
    }
}

static void ioLoadDriver(int number, IODevice dev) {

    ThreadId t;

    if (dev->interruptHandler != NULL) {
	(void)tmSetInterruptHandler(dev->interrupt, dev->interruptHandler,dev);
    }
    
    /* main function is for all devices the same */
    if (tmStart( &t, (ThreadMainFunction)ioDeviceMain,
		    dev, dev->name) != TM_STARTOK) {
	ERROR("Driver could not be started\n");
    }
    /* track the numbers to be able to open drivers later */
    hashListAdd(ioDevices, (void*)t, number);
}
