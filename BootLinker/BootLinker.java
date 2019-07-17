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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/BootLinker/RCS/BootLinker.java,v $
 	Author(s):             G. Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.1 $
 	Creation Date:         January 1997
 	Last Date of Change:   $Date: 1997/05/09 14:28:28 $      by: $Author: gfa $
	
	
*/

import java.lang.*;
import java.io.*;

public class BootLinker extends Object {

    public final static String OUTFILE = "segmap.bin";
    
    public static void main(String argv[]) throws Exception {
	String a, b, s = null;
	File f;
	
	if (argv.length != 5) {
	    System.out.println(
	    "usage: java BootLinker <kSizeFile> <iSizeFile> <user.srec> SEGMAPADDR USERINKERNELAT"
	    );
	    System.exit(0);
	}
	try {
	    f = new File(OUTFILE);
	    f.delete();
	}
	catch (Exception e) { 
	}
	RandomAccessFile segmapFile = new RandomAccessFile(OUTFILE, "rw");
	RandomAccessFile kSizeFile = new RandomAccessFile(argv[0], "r");
	RandomAccessFile iSizeFile = new RandomAccessFile(argv[1], "r");
	RandomAccessFile userSrec = new RandomAccessFile(argv[2], "r");

	// write segmentMap (sizes and start addresses of kernel and 
	// user segments). we assume that the sizeFile was generated 
	// by gnu-size with options -Ax (addresses, hex)
	// ex.: size -Ax
	try {
	    //// write segment map
//	    segmapFile.writeBytes("S3" + // record type
//				  "29" + // const length 41 bytes
//				argv[3]);	// where it goes (segmap addr)	
	    segmapFile.seek(0);						   
	    parseSizeFile(kSizeFile, segmapFile); // size/start info kernel	
	    parseSizeFile(iSizeFile, segmapFile); // size/start info init	
	    try {
		while (true) {
		    s = userSrec.readLine();
		    if ((s.charAt(0) == 'S') && (s.charAt(1) == '7')) {
			//segmapFile.writeBytes(s.substring(4,12));
			segmapFile.writeInt(
			    Integer.parseInt(s.substring(4,12), 16)
			);						
		    }
		}
	    }
	    catch (Exception e) {
	    }
	    // write the user-in-kernel-address at the end
	    segmapFile.writeInt((int)Long.parseLong(argv[4], 16));

//	    if (segmapFile.getFilePointer() != 84) {
//		System.out.print("BootLinker: couldn't write segmap\n");
//		System.exit(-1);
//	    }
	    // data is in, now write the checksum
	    //segmapFile.writeBytes("00");			
	    //segmapFile.writeBytes("\n");
	}
	catch (Exception e) {
		throw e;
	}	
    }
    
    static void parseSizeFile(RandomAccessFile sizeFile, RandomAccessFile out) 
							    throws IOException{
	int addr = 0, size = 0;
	// expect text
	if (expect(sizeFile, ".text")) {
	    size = readHexInt(sizeFile); addr = readHexInt(sizeFile);
	}
//	writeHexInt(out, size); writeHexInt(out, addr); 	
	out.writeInt( size); out.writeInt(addr); 	

	// expect data, add sizes up and find start address by minimizing
	size = 0; addr = 0;
	if (expect(sizeFile, ".data")) {
	    size = readHexInt(sizeFile);
	    addr = readHexInt(sizeFile);
	}
	if (expect(sizeFile, ".rdata")) {
	    size += readHexInt(sizeFile); 
	    addr = Math.min(addr, readHexInt(sizeFile));
	}
	if (expect(sizeFile, ".bss")) {
	    size += readHexInt(sizeFile); 
	    addr = Math.min(addr, readHexInt(sizeFile));
	}
	if (expect(sizeFile, ".sbss")) {
	    size += readHexInt(sizeFile); 
	    addr = Math.min(addr, readHexInt(sizeFile));
	}
//	writeHexInt(out, size); writeHexInt(out, addr);
	out.writeInt( size); out.writeInt(addr); 	
    }
    
    static void writeHexInt(RandomAccessFile segmapFile, int hex) 
							    throws IOException{
	byte hexArray[] =  {'0','1','2','3','4','5','6','7',
			    '8','9','A','B','C','D','E','F'};
	for (int i = 0; i < 8; i++) {
		segmapFile.writeByte(hexArray[(hex >> 28) & 0xf]);
		hex = hex << 4;
	}
    }
    
    static boolean expect(RandomAccessFile f, String exp) throws IOException {
	String s;
	long pos;
	
	f.seek(0);
	while (true) {
	    try {
		pos = f.getFilePointer();
		s = f.readLine();
		if (s.length() < exp.length()) continue;
		if (exp.equals(s.substring(0, exp.length()))) {
			f.seek(pos+exp.length());
			return true;
		}
	    }
	    catch (Exception e) {
		    return false;
	    }
	}
    }
	    
    static int readHexInt(RandomAccessFile f) throws IOException {
	    int value = 0, tmp;
	    // skip white space
	    byte c = f.readByte();
	    while ((c == ' ') || (c == '\t')) {
		    c = f.readByte();
	    }
	    int i = 28;
	    while (isHex(c)) {
		    tmp = readHexByte(c);
		    value |= ((tmp & 0xf) << i);
		    i-=4; c = f.readByte();
	    }
	    return (value >>> (i+4));
    }
	    
    static byte readHexByte(byte b) {
	    char aChar = 'a';
	    char nullChar = '0';
	    if ((b >= nullChar) && (b < (nullChar+10))) {
		    return (byte)(b - nullChar);
	    }
	    else if ((b >= aChar) && (b < (aChar+6))) {
		    return (byte)(b - aChar + 10);
	    }
	    else {
		    return (byte)0;
	    }	
    }
	    
    static boolean isHex(byte c) {		 
	    return ((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f'));
    }		
}


class BootLinkerSizeFormatException extends Exception {
}
	
