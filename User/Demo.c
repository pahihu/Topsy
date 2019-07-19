
#include "../Topsy/Topsy.h"
#include "../Topsy/Syscall.h"
#include "UserSupport.h"

#include "Demo.h"


void Testback1(ThreadArg arg) {
  int      i,arglen,len=0;
  char*    argument;
  char     outputline[80];
  ThreadId tty;

  ioOpen(IO_CONSOLE, &tty);
  ioInit(tty);

  for (i=0; i<1000; i++) { /* NOP */ }

  display(tty,"testback1 found args: \n");
  for (i=0; i<5; i++) {
    argument=((char**)arg)[i];
    if (argument==NULL) break;
    arglen=stringLength(argument);
    byteCopy((Address)(&outputline[len]),(Address)argument,arglen);
    len+=arglen;
    byteCopy((Address)(&outputline[len]),(Address)"|",1);
    len++;
  }
  byteCopy((Address)(&outputline[len]),(Address)"\n\0",2);

  display(tty,outputline);
  ioClose(tty);
}

void trap () 
{
  int* i = (int *)(5);
  
  *i = 1;
}

