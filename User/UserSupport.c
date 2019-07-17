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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/User/RCS/UserSupport.c,v $
 	Author(s):             C. Conrad 
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.10 $
 	Creation Date:         21.3.97
 	Last Date of Change:   $Date: 1998/04/09 12:16:18 $      by: $Author: fiedler $
	
	
*/

#include "UserSupport.h"


void byteCopy( Address targetAddress,
               Address sourceAddress,
               unsigned long int nbBytes)
{
    char* src = (char*)sourceAddress;
    char* tar = (char*)targetAddress;
    unsigned long int i;
    
    for (i = 0; i < nbBytes; i++) {
        *tar++ = *src++;
    }
}
 
void zeroOut(Address target, unsigned long int size)
{
    char* tar = (char*)target;
    unsigned long int i;
    
    for (i = 0; i < size; i++) *tar++ = 0;
}

/* schneller als zeroOut */
void initmem(Address target, unsigned long int size, char c)
{
    char* tar = (char*)target;
    unsigned long int i;
    
    for (i=size; i>0; i--) *tar++ = c;
}
 
void stringCopy( char* target,
                 char* source)
{
    while ((*target++ = *source++) != '\0')
      ;
}
 
void stringNCopy(char* target, char* source, unsigned long int size)
{
    while (((*target++ = *source++) != '\0') && ((--size) > 1)) ;
    *target = '\0';
}

int stringLength( char* s)
{
    int len=0;

    if (s!=NULL)
      while (*s++ != '\0')
	len++;

    return len;
}



void stringConcat(char *dest, char *source1, char *source2)
{
    char *ptr;
    ptr = dest;
    stringCopy(ptr, source1);
    ptr+=stringLength(source1);
    stringCopy(ptr,source2);
}

void int2string(char *str, int i)
{
    int j,nrc;
    
    j=i;
    nrc=0;
    while(j>0) {
        nrc++;
        j = (j - (j%10))/10;
    }
    if (i < 0) {
        nrc++;
        str[0] = '-';
    }
    if (i == 0) str[0] = '0';
    str[nrc+1] = 0;
    while(nrc >0) {
        str[--nrc] = (i%10) + 48;
        i = (i - (i%10))/10;
    }
}
void display( ThreadId tty, char* s)
{
    char comment[MAXCOMMENTSIZE];
    unsigned long int size = stringLength( s);

    ioWrite(tty, s, &size);
}
    
int stringCompare(char* a, char* b) {
  int i = 0;
  if (a != NULL && b != NULL) {
    while ((a[i] != '\0') && (b[i] != '\0')) {
      if (a[i] < b[i]) return BEFORE;
      if (a[i] > b[i]) return AFTER;
      i++;
    }
    /* a contains b or vice versa */
    if (a[i] == '\0' && b[i] == '\0') return EQUAL;
    if (a[i] == '\0') return BEFORE;
    return AFTER;
  }
  return UNDEFINED;
}

int stringNCompare(const char *s1, const char *s2, int n) {

  if (n == 0 || s1 == NULL || s2 == NULL) return UNDEFINED;
  do {
    if (*s1 != *s2) {
      if (*s1 < *s2) return BEFORE;
      else return AFTER;
    }
    if (*s1 == 0 || *s2 == 0) break;
    s1++; s2++;
  } while (--n != 0 );
  if ((n == 0) || (*s1 == 0 && *s2 == 0))  return EQUAL;
  if (*s1 == 0) return BEFORE;
  return AFTER;
}

int power(int base, int n)
{
    int p;
    for (p=1;n>0;--n)
      p*=base;
    return p;
}


int atoi(int* intValue, char* string)
{
    char* currentPtr = string;
    int counter=0;
    int nbChars=1;
    
    *intValue=0;

    /* cur positioned on last byte not NULL */
    while (*(currentPtr+1) != '\0') {
	counter++;
	currentPtr++;
	nbChars++;
    }
    
    while (counter>=0) {
	if ( *currentPtr=='-') {
	    if (counter==0) {
		*intValue *= (-1);
	    } else {
		return -1;
	    }
	} else {
	    if ( *currentPtr-'0' < 0 || *currentPtr-'0' > 9) {
		return -1;
	    }
	    *intValue+=(*currentPtr-'0')*(power(10,nbChars-counter-1));
	}
	counter--;
	currentPtr--;
    }
    return 1;
}

void reverse(char s[])
{
    int c,i,j;
    for(i=0,j=stringLength(s)-1;i<j;i++,j--) {
	c=s[i];
	s[i]=s[j];
	s[j]=c;
    }
}
void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)
      n = -n;
    i=0;
    do {
	s[i++]=n%10+'0';
    } while ((n/=10)>0);
    if (sign<0)
      s[i++]='-';
    s[i]='\0';
    reverse(s);
}

/*void byteCopy( Address targetAddress,
               Address sourceAddress,
               unsigned long int nbBytes)
{
    char* src = (char*)sourceAddress;
    char* tar = (char*)targetAddress;
    unsigned long int i;
    
    for (i = 0; i < nbBytes; i++) {
        *tar++ = *src++;
    }
}*/
