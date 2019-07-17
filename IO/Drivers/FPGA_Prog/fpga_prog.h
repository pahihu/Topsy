/******************************************************************************/
/* Programmingroutine for the XILINX XC4008 FPGA on add-on FPGA-BOARD of RISC */
/* RX3053-Evaluation BOARD 7RS385                                             */
/*                                                                            */
/* Author            : M. Eisenring                                           */
/*                                                                            */
/* File              : fpga_prog.h                                            */
/* Last modification : 12.5.97, Bernard Stauffer                              */
/******************************************************************************/

#include "Configuration.h"


#ifndef _fpga_prog
#define _fpga_prog

/* exported function */
void InitLinkBOARD();


#define FPGADATASIZE_0    16384
#define FPGADATASIZE_1    2060
#define NRFPGADATABLOCKS  2

extern unsigned char FPGA_10[FPGADATASIZE_0];   /* program data for FPGA ... */       
extern unsigned char FPGA_11[FPGADATASIZE_1];   /* ... defined in fpgadata.c */   

#define RESETdelay   5000                       /* pulsewidth of RESET */
#define CHECKdelay   5000                       /* time after programming check is done */


#define Ereg0_Addr   (void*) 0xBFA00000            
#define Ereg1_Addr   (void*) 0xBFA40000
#define Ereg2_Addr   (void*) 0xBFA80000
#define Ereg3_Addr   (void*) 0xBFAC0000         /* programming addresses of FPGA */


#define EPLD_LED0    0x01                       /* important programming constants */
#define EPLD_LED1    0x02
#define EPLD_LED2    0x04
#define EPLD_LED3    0x08
#define EPLD_LED4    0x10
#define mask_Init    0x08                        
#define mask_DONE    0x04
#define mask_READY   0x80
#define mask_RESET   0x02
#define mask_PROGRAM 0x01

unsigned long *Ereg0;                           /* Pointer for FPGA programming */
unsigned long *Ereg1;
unsigned long *Ereg2;
unsigned long *Ereg3;

unsigned long LED;                  /* memory of LEDs */
unsigned long *pLED;                /* pointer to LEDs */

#endif






































