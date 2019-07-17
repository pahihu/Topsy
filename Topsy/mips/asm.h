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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/mips/RCS/asm.h,v $
 	Author(s):             
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.8 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/21 08:04:39 $      by: $Author: gfa $
	
	
*/

/* register names */

#define zero $0	/* wired zero */
#define AT	$1	/* assembler temp (uprcase, because ".set at") */
#define v0	$2	/* return value */
#define v1	$3
#define a0	$4	/* argument registers */
#define a1	$5
#define a2	$6
#define a3	$7
#define t0	$8	/* caller saved */
#define t1	$9
#define t2	$10
#define t3	$11
#define t4	$12
#define t5	$13
#define t6	$14
#define t7	$15
#define s0	$16	/* callee saved */
#define s1	$17
#define s2	$18
#define s3	$19
#define s4	$20
#define s5	$21
#define s6	$22
#define s7	$23
#define t8	$24	/* caller saved */
#define t9	$25
#define k0	$26	/* kernel scratch */
#define k1	$27
#define gp	$28	/* global pointer */
#define sp	$29	/* stack pointer */
#define fp	$30	/* frame pointer */
#define ra	$31	/* return address */

/* short hands */

#define	FRAME(_name_)	\
	.globl	_name_;		\
	.ent	_name_,0;	\
_name_:;			\
	.frame	sp,0,ra

#define	END(_name_)		\
	j		ra;	\
	.end	_name_
	
/* define a symbolic label */
#define LABEL(_name_)   \
.globl  _name_;         \
_name_:


#define	NESTED(_name_, _framesize_, _saved_pc_)	\
	.globl	_name_;				\
	.ent	_name_,0;			\
_name_:;					\
	.frame	sp,_framesize_, _saved_pc_;	\
	subu	sp, sp, _framesize_;		\
	sw		ra, 28(sp);		\
	sw		fp, 24(sp);    		\
        move            fp, sp;        		\
	

#define	ENDNESTED(_name_, _framesize_)		\
	lw		ra, 28(sp);		\
	lw		fp, 24(sp);		\
	addu	sp, sp, _framesize_;		\
	j		ra;			\
	.end	_name_
	

/* pseudo opcodes */


