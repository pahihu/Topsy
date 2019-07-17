/*****************************************************************************
 Programmingroutine for the XILINX XC4005 FPGA on add-on FPGA-BOARD on
 R3052-Evaluation BOARD 7RS385             
                                   
 Author            : M. Eisenring                    
 File              : fpga_prog.c          
 Last modification : 13.5.97, Bernard Stauffer     
******************************************************************************/

#include "IOConsole.h"
#include "fpga_prog.h"


/****************************************************************************/
/* wait for some time                                                         */
/****************************************************************************/
void wait(unsigned int TIME) {
  unsigned long i;

  for (i=0;i<TIME*10;i++);
}


/****************************************************************************/
/* Initialisation sequence for LED 0 ... 4 of EPLD                            */
/****************************************************************************/
void eInitLED() {

  pLED  = (void*)Ereg0_Addr;

  LED   = 0;              /* LEDs on */
  *pLED = LED;

  wait(500);              

  LED   = 32767;          /* LEDs off */
  *pLED = LED;
}


/****************************************************************************/
/* Set LED x on                                                               */
/****************************************************************************/
void eSetLED(unsigned long localLED) {
  LED   = LED & ~localLED;
  *pLED = LED;
}


/****************************************************************************/
/* Set LED x off                                                              */
/****************************************************************************/
void eClearLED(unsigned long localLED) {
  LED   = LED | localLED;
  *pLED = LED;
}


/****************************************************************************/
/* VooDoo Code                                                                */
/****************************************************************************/
void VooDoo_fpgaprog() {

	wait(1);            
}


/****************************************************************************/
/* load configuration data into FPGA                                          */
/****************************************************************************/
unsigned int ProgramFPGA() {

    unsigned char *ConfigurationData_Addr;    /* Data source for FPGA data */
    unsigned long *FPGA_Programming_Addr;     /* programming address */
    volatile unsigned char FPGA_STATUS;       /* Statusword of FPGA */
    unsigned int blocknr, datasize;           /* FPGA data Counter */
    unsigned int err = 0;               	    /* Error var */
    unsigned int i;                           /* Data Counter */
    unsigned int j=0;                         /* temporary counter */
    
    /* set pointers */
    ConfigurationData_Addr = (void*)0 ;     /* Dummy initialization ... */
    FPGA_Programming_Addr  = Ereg3;    
    
    /* waiting for FPGA ready to program */
    do {
    FPGA_STATUS = *Ereg2;
    FPGA_STATUS = FPGA_STATUS & mask_Init;
    } while (FPGA_STATUS==0);
    
    /* program FPGA  */
    eSetLED(EPLD_LED2);
    for (blocknr = 0;  blocknr < NRFPGADATABLOCKS; blocknr++) {
	switch (blocknr) {
	    case 0:
	    ConfigurationData_Addr = FPGA_10; /* Set ptr to FPGA data block */
	    datasize = FPGADATASIZE_0;
	    break;
	    case 1:
	    ConfigurationData_Addr = FPGA_11; /* Set ptr to FPGA data block */
	    datasize = FPGADATASIZE_1;
	    break;
	    default:
	    datasize = 0;
	    err = 1;                          /* Set Error 1 */
	    break;
	}
    
	for (i=0; i<datasize; i++, ConfigurationData_Addr++) {
	    *FPGA_Programming_Addr = *ConfigurationData_Addr;  /* write byte */
	
	VooDoo_fpgaprog();
	
	    /* wait until FPGA may accept next byte */
	    if ((blocknr == 0) || (i < (datasize -1))) {    
		do {
		    FPGA_STATUS = *FPGA_Programming_Addr;    /* read Status */
		/*ioConsolePutHexInt(FPGA_STATUS);*/
		/*if (i == 2059) ioConsolePutHexInt(FPGA_STATUS); */
		    FPGA_STATUS = FPGA_STATUS & mask_READY;
		    j++;
		    if (j>1000) {
			err = 1;                           /* Set Error 1 */
			eSetLED(EPLD_LED3);           /* FPGA never READY */
		    }
		} while (FPGA_STATUS==0);
	    }
	    j = 0;
	    
	    /* check if no error occured */
	    FPGA_STATUS = *Ereg2;
	    FPGA_STATUS = FPGA_STATUS & mask_Init;
	    if (FPGA_STATUS==0) {
		err = 2;                                   /* Set Error 2 */
		break;
	    }
	}
    }
    eClearLED(EPLD_LED2);
    
    /* check error 1 */
    if (err != 0)
    eSetLED(EPLD_LED0);                                   /* signal error 1 */
    else 
    {
    for (i=0;i<CHECKdelay;i++) ;                          /* wait some time */
	FPGA_STATUS = *Ereg2;                             /* read Status */
	FPGA_STATUS = FPGA_STATUS & mask_DONE;
	if (FPGA_STATUS==0) {
	    eSetLED(EPLD_LED1);  
	    err = 3;                                    /* Set Error 3 */
	}
    }
    return err;
}


/******************************************************************************/
/* reset FPGA without reloading configuration data                            */
/******************************************************************************/
void ResetFPGA() {
  int i;
  unsigned long mask;

  mask   = mask_PROGRAM;
  *Ereg2 = mask;                            /* force RESET down */

  for (i=0;i<RESETdelay;i++) ;              /* wait some time */

  mask   = mask_PROGRAM + mask_RESET;       /* force RESET and PROGRAM high */
  *Ereg2 = mask;
}


/******************************************************************************/
/* clear FPGA configuration data -> FPGA goes in configuration mode           */
/******************************************************************************/
void ClearFPGA() {
  int i;
  unsigned long mask;

  mask   = 0;
  *Ereg2 = mask;                        /* force RESET and PROGRAM down */

  for (i=0;i<RESETdelay;i++) ;          /* wait some time */

  mask   = mask_PROGRAM + mask_RESET;   /* force RESET and PROGRAM high */
  *Ereg2 = mask;
}


/******************************************************************************/
/* initialisation of LinkBOARD                                                */
/******************************************************************************/
void InitLinkBOARD() {

    unsigned int error;                     	/* error variable */
    unsigned long fpga_register;

    /* set pointer */
    Ereg0   = (void*)Ereg0_Addr;
    Ereg1   = (void*)Ereg1_Addr;
    Ereg2   = (void*)Ereg2_Addr;
    Ereg3   = (void*)Ereg3_Addr;

    fpga_register = *Ereg3;
    
    if (fpga_register == 0xffffffff) {
	ioConsolePutString("\nProbably no real FPGA... aborting init\n"); 
	return;   
    }

	ioDelayAtLeastCycles(10000);
	ioConsolePutString("\nProgramming FPGA ... ");
	ioDelayAtLeastCycles(10000);


    eInitLED();						/* clear LEDs */
    
    ClearFPGA();                      /* clear configuration data */
    error = ProgramFPGA();            /* programming of FPGA */
    ResetFPGA();                      /* reset FPGA */

    if (error == 0) {
	    ioConsolePutString("done.\n");
    }
    else {
	    ioConsolePutString("Error occured!!!\n");
    }
//ioDelayAtLeastCycles(10000);
}
