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


        
        File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/mips/RCS/limits.h,v $
        Author(s):             
        Affiliation:           ETH Zuerich, TIK
        Version:               $Revision: 1.2 $
        Creation Date:         
        Last Date of Change:   $Date: 1997/04/12 16:01:46 $      by: $Author: gfa $
        
        
*/

/* compiler and target machine dependent limits */

#define CHAR_BIT 8
#define SCHAR_MIN (-128)
#define SCHAR_MAX 127
#define UCHAR_MAX 255U

#define CHAR_MIN 0
#define CHAR_MAX UCHAR_MAX

#define SHORT_MIN (-32768)
#define SHORT_MAX 32767
#define UNSIGNED_SHORT_MAX 65535U

#define INT_MAX 2147483647
#define INT_MIN (-INT_MAX-1)
#define UNSIGNED_INT_MAX 4294967295U

#define LONG_MIN INT_MIN
#define LONG_MAX INT_MAX
#define UNSIGNED_LONG_MAX UNSIGNED_INT_MAX
