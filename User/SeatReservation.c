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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/User/RCS/SeatReservation.c,v $
 	Author(s):             Marcus Brunner 
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.9 $
 	Creation Date:         8.4.97
 	Last Date of Change:   $Date: 1998/03/26 20:38:36 $      by: $Author: gfa $
	
	
*/

#include "../Topsy/Syscall.h"
#include "UserSupport.h"
 
#define MAX_SEATS 80
#define NR_THREADS  8

int seats;                   /* Anzahl reservierter Sitze */
ThreadId tty;                /* ThreadId des I/O Thread */
int reisebuero[NR_THREADS];  /* Zwischenergebnisse der Buchungen */
int k;   

void Res_Thread(int i)
{
    int myreserved,tmp,j;
    char str[10],output[40],output2[40],idstr[7];
    ThreadId myid, parent;
    Message reply;
    Boolean full;
    
    /* Hole eigene ThreadId */
    if (tmGetInfo(SELF, &myid, &parent) < 0) {
        display(tty,"tmGetInfo failed\n");
        /* exit(-1); */
    }    
    /* initalisieren der Variabeln */
    myreserved = 0;
    itoa(myid,idstr);
    stringConcat(idstr,idstr,": ");
    full =FALSE;
    
    /* Falls noch Sitze vorhanden sind, reservere einen Sitz */
    while (!full) {
        /* lesen der Anzahl bis jetzt reservierten Sitze */
        tmp = seats;
        
        /* for demonstration only */
        for (j=0; j<400; j++) {
             k--;
        }
        
        /* Sind noch Sitze frei ? */
        if (tmp >= MAX_SEATS) {
            /* Nein, ausgebucht */
            itoa(myreserved,str);
            stringConcat(output,idstr,str);
            stringConcat(output2,output," Sitze total erhalten\n");
            display (tty,output2);

	    /* Abbruch */            
            full = TRUE;
        }
        else {
            /* Ja, es hat noch freie */
            /* erhoehe die Anzahl der Sitze, welche dieses Reisebuero
               reserviert hat
            */
            myreserved++;
            
            /* Schreibe die neue Anzahl der Sitze zurueck */
            seats = tmp+1;
            
            /* print */
            itoa(tmp,str);
            stringConcat(output,idstr,str);
            stringConcat(output2,output," Sitze sind schon reserviert\n");
            display (tty,output2);
        }
    }
    /* Speichere die Sitze, die diese Reisebuero reserviert hat */
    reisebuero[i] = myreserved;
}

void Start_Reservation()
{
    ThreadId child[NR_THREADS];
    Message reply;
    int i,sum;
    char str[10];
    
    ioOpen(IO_CONSOLE, &tty);
    ioInit(tty);    
    
    seats = 0;
    
    /* Reisebueros werden kreiert */
    for (i=0; i< NR_THREADS; i++){
        if (tmStart( &child[i], (ThreadMainFunction)Res_Thread, (ThreadArg)i ,"Res_Thread") == TM_STARTFAILED) {
             display(tty,"thread start failed");
        }  
    }

    /* Wir warten, bis alle Reisebueros fertig gebucht haben */
    for (i=0; i<  NR_THREADS; i++){
        if (tmMsgRecv(&(child[i]), ANYMSGTYPE, &reply, INFINITY) == TM_MSGRECVFAILED) {
            /* Receive failed, weil der Thread child[i] nicht mehr existiert */
        }
    }
    
    display(tty,"Alle Reisebueros haben geschlossen\n");    
    
    for (i=0; i<  NR_THREADS; i++){
        sum +=reisebuero[i];
    }
            
    /* Alle Reisebueros haben geschlossen */
    itoa(sum,str);
    display(tty,"Die Reisebuero's haben zusammen ");
    display(tty,str);
    display(tty," Sitze reserviert \n(Es stehen 80 zur Verfuegung)\n");
}
