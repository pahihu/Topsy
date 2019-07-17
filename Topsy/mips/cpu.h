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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/mips/RCS/cpu.h,v $
 	Author(s):             
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.7 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/04/08 15:09:20 $      by: $Author: gfa $
	
	
*/

#define PHYS_BASE		0
#define	KUSEG_BASE		0
#define	KUSEG_SIZE		0x80000000
#define	K0SEG_BASE		0x80000000
#define	K0SEG_SIZE		0x20000000
#define	K1SEG_BASE		0xa0000000
#define	K1SEG_SIZE		0x20000000
#define	K2SEG_BASE		0xc0000000
#define	K2SEG_SIZE		0x40000000

#define	ISA_KUSEG(x)		((unsigned)(x)  <  K0SEG_BASE)
#define	ISA_K0SEG(x)		(((unsigned)(x) >= K0SEG_BASE) && \
				 ((unsigned)(x) <  K1SEG_BASE))
#define	ISA_K1SEG(x)		(((unsigned)(x) >= K1SEG_BASE) && \
				 ((unsigned)(x) <  K2SEG_BASE))
#define	ISA_K2SEG(x)		((unsigned)(x)  >= K2SEG_BASE)

#define	K0SEG_TO_PHYS(x)	((unsigned)(x) & 0x1fffffff)
#define	PHYS_TO_K0SEG(x)	((unsigned)(x) | K0SEG_BASE)

#define	K1SEG_TO_PHYS(x)	((unsigned)(x) & 0x1fffffff)
#define	PHYS_TO_K1SEG(x)	((unsigned)(x) | K1SEG_BASE)

#define	K0SEG_TO_K1SEG(x)	((unsigned)(x) | K1SEG_BASE)
#define	K1SEG_TO_K0SEG(x)	((unsigned)(x) & 0x9fffffff)

#define	VEC_TLB_UMISS		K0SEG_BASE
#define	VEC_EXCEPTION		(K0SEG_BASE+0x80)
#define	VEC_RESET		(K1SEG_BASE+0x1fc00000)

#define	TLB_BASE		0
#define	TLB_SIZE		64
#define TLB_SAFE_BASE		0
#define	TLB_SAFE_SIZE		8
#define	TLB_USER_BASE		8
#define	TLB_USER_SIZE		56


#define	c0_tlbhi		$10		/* EntryHi Register */
#define	TLB_HI_VPN_MASK		0xfffff000	/* 20 high bits */
#define	TLB_HI_VPN_SHIFT	12
#define	TLB_HI_PID_MASK		0xfc0		/* 6 bits */
#define	TLB_HI_PID_SHIFT	6
#define	TLB_HI_NPID		64		/* tlb context ids */

#define	c0_tlblo		$2		/* Entry Lo Register */
#define	TLB_LO_PFN_MASK		0xfffff000	/* 20 high bits */
#define	TLB_LO_PFN_SHIFT	12
#define	TLB_NOCACHE		0x0800		/* No cache */
#define	TLB_DIRTY		0x0400		/* Dirty */
#define	TLB_VALID		0x0200		/* Valid */
#define	TLB_GLOBAL		0x0100		/* Global */

#define	c0_tlbind		$0		/* Index Register */
#define	TLB_IND_P		0x80000000	/* Probe */
#define	TLB_IND_INX_MASK	0x00003f00	/* Index */
#define	TLB_IND_INX_SHIFT	8

#define c0_tlbrnd		$1		/* Random Register */
#define	TLB_RND_MASK		0x00003f00	/* Index */
#define	TLB_RND_SHIFT		8

#define c0_tlbcxt		$4		/* Context Register */
#define	TLB_CXT_PTE_MASK	0xffe00000	/* PTE base */
#define	TLB_CXT_PTE_SHIFT	21
#define	TLB_CXT_VPN_MASK	0x001ffffc	/* Failed VPN */
#define	TLB_CXT_VPN_SHIFT	2

#define	c0_badvm		$8		/* BadVAddr Register */

#define	c0_status		$12		/* Status Register */
#define	SR_CU_MASK		0xf0000000	/* Coproc Usable */
#define	SR_CU3		0x80000000	/* Coprocessor 3 usable */
#define	SR_CU2		0x40000000	/* Coprocessor 2 usable */
#define	SR_CU1		0x20000000	/* Coprocessor 1 usable */
#define	SR_CU0		0x10000000	/* Coprocessor 0 usable */
#define	SR_BEV			0x00400000	/* Boot Exception Vectors */
#define	SR_TS			0x00200000	/* TLB shutdown */
#define	SR_PE			0x00100000	/* Parity Error (cache) */
#define	SR_CM			0x00080000	/* Cache Miss */
#define	SR_PZ			0x00040000	/* Parity Zero (cache) */
#define	SR_SwC			0x00020000	/* Swap Caches */
#define	SR_IsC			0x00010000	/* Isolate (data) Cache */
#define	SR_INT_MASK		0x0000ff00	/* Interrupt Mask (1=enable)*/
#define	INT_LEV8	0x00000000	/* Interrupt level 8 */
#define	INT_LEV7	0x00008000	/* Interrupt level 7 */
#define	INT_LEV6	0x0000c000	/* Interrupt level 6 */
#define	INT_LEV5	0x0000e000	/* Interrupt level 5 */
#define	INT_LEV4	0x0000f000	/* Interrupt level 4 */
#define	INT_LEV3	0x0000f800	/* Interrupt level 3 */
#define	INT_LEV2	0x0000fc00	/* Interrupt level 2 */
#define	INT_LEV1	0x0000fe00	/* Interrupt level 1 */
#define	INT_LEV0	0x0000ff00	/* Interrupt level 0 */
#define INTMASK_SHIFT           8
#define	SR_KUo			0x00000020	/* old  mode (1 => user) */
#define	SR_KUp			0x00000008	/* previous mode */
#define SR_KUo_SHIFT	5
#define SR_KUp_SHIFT	3
#define	SR_KUc			0x00000002	/* currrent mode */
#define	SR_IEo			0x00000010	/* old IntEn (1 => enable) */
#define	SR_IEp			0x00000004	/* previous IntEn */
#define	SR_IEc			0x00000001	/* Current IntEn */

#define	c0_cause		$13		/* Cause Register */
#define	CAUSE_BD		0x80000000	/* Branch Delay */
#define	CAUSE_CE_MASK		0x30000000	/* Coprocessor Error */
#define	CAUSE_CE_SHIFT		28
#define	CAUSE_IP_MASK		0x0000ff00	/* Interrupts Pending */
#define	CAUSE_IP_SHIFT		8
#define	IP_NLEV		8		/* No. of Interrupt Levels */
#define	IP_LEV7		0x00008000	/* Hardware level 7 pending */
#define	IP_LEV6		0x00004000	/* Hardware level 6 pending */
#define	IP_LEV5		0x00002000	/* Hardware level 5 pending */
#define	IP_LEV4		0x00001000	/* Hardware level 4 pending */
#define	IP_LEV3		0x00000800	/* Hardware level 3 pending */
#define	IP_LEV2		0x00000400	/* Hardware level 2 pending */
#define	IP_LEV1		0x00000200	/* Software level 1 pending */
#define	IP_LEV0		0x00000100	/* Software level 0 pending */
#define	CAUSE_EXC_MASK		0x0000003c	/* Exc Code */
#define	CAUSE_EXC_SHIFT		2
#define	EXC_INT		0x00000000	/* Interrupt */
#define	EXC_MOD		0x00000004	/* TLB mod */
#define	EXC_TLBL	0x00000008	/* TLB Miss (Load) */
#define	EXC_TLBS	0x0000000c	/* TLB Miss (Store) */
#define	EXC_ADEL	0x00000010	/* Address Error (Load) */
#define	EXC_ADES	0x00000014	/* Address Error (Store) */
#define	EXC_IBE		0x00000018	/* Bus Error (Instruction) */
#define	EXC_DBE		0x0000001c	/* Bus Error (Data) */
#define	EXC_SYS		0x00000020	/* Syscall */
#define	EXC_BP		0x00000024	/* Breakpoint */
#define	EXC_RI		0x00000028	/* Reserved Instruction */
#define	EXC_CU		0x0000002c	/* Coprocessor Unusable */
#define	EXC_OVF		0x00000030	/* Arithmetic Overflow */

#define	c0_epc			$14		/* ExceptionPC Register */
#define	c0_prid			$15		/* Processor Id Register */

