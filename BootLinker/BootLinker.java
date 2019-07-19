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
	    parseSizeFile("KERNEL", kSizeFile, segmapFile); // size/start info kernel	
	    parseSizeFile("  INIT", iSizeFile, segmapFile); // size/start info init	
	    try {
		while (true) {
		    s = userSrec.readLine();
		    if ((s.charAt(0) == 'S') && (s.charAt(1) == '7')) {
			int s7;
			//segmapFile.writeBytes(s.substring(4,12));
			s7 = Integer.parseInt(s.substring(4,12), 16);
			// XXX s7 += 0x80040000;
			segmapFile.writeInt(s7);
			System.out.println("       STARTAT: 0x" + toHexString(s7));
		    }
		}
	    }
	    catch (Exception e) {
	    }
	    // write the user-in-kernel-address at the end
	    segmapFile.writeInt((int)Long.parseLong(argv[4], 16));
	    System.out.println("USERINKERNELAT: 0x" + argv[4]);

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

    static String toHexString(int val) {
        return String.format("%1$08x", val);
    }
    
    static void parseSizeFile(String msg, RandomAccessFile sizeFile, RandomAccessFile out) 
							    throws IOException{
	boolean dbg = false;
	int addr = 0, size = 0;
	int seg_size, seg_addr;
	// expect text
	if (expect(sizeFile, ".text")) {
	    size = readHexInt(sizeFile); addr = readHexInt(sizeFile);
	    // XXX if ((addr & 0x80000000) == 0) addr += 0x80040000;
	}
//	writeHexInt(out, size); writeHexInt(out, addr); 	
	out.writeInt( size); out.writeInt(addr); 	
	System.out.println(msg + "TEXTSIZE: 0x" + toHexString(size));
	System.out.println(msg + "TEXTADDR: 0x" + toHexString(addr));

	// expect data, add sizes up and find start address by minimizing
	size = 0; addr = 0;
	if (expect(sizeFile, ".data")) {
	    seg_size = readHexInt(sizeFile);
	    seg_addr = readHexInt(sizeFile);
	    if (dbg)
	    System.out.println("\t.data\t0x" + toHexString(seg_size) + "\t0x" + toHexString(seg_addr));
	    size = seg_size;
	    addr = seg_addr;
	}
	if (expect(sizeFile, ".rodata.str1.4 ")) {
	    seg_size = readHexInt(sizeFile);
	    seg_addr = readHexInt(sizeFile);
	    if (dbg)
	    System.out.println("\t.rodata.str1.4\t0x" + toHexString(seg_size) + "\t0x" + toHexString(seg_addr));
	    size += seg_size;
	    addr = Math.min(addr, seg_addr);
	}
	if (expect(sizeFile, ".rodata ")) {
	    seg_size = readHexInt(sizeFile);
	    seg_addr = readHexInt(sizeFile);
	    if (dbg)
	    System.out.println("\t.rodata\t0x" + toHexString(seg_size) + "\t0x" + toHexString(seg_addr));
	    size += seg_size;
	    addr = Math.min(addr, seg_addr);
	}
	if (expect(sizeFile, ".sdata")) {
	    seg_size = readHexInt(sizeFile);
	    seg_addr = readHexInt(sizeFile);
	    if (dbg)
	    System.out.println("\t.sdata\t0x" + toHexString(seg_size) + "\t0x" + toHexString(seg_addr));
	    size += seg_size;
	    addr = Math.min(addr, seg_addr);
	}
	if (expect(sizeFile, ".sbss")) {
	    seg_size = readHexInt(sizeFile);
	    seg_addr = readHexInt(sizeFile);
	    if (dbg)
	    System.out.println("\t.sbss\t0x" + toHexString(seg_size) + "\t0x" + toHexString(seg_addr));
	    size += seg_size;
	    addr = Math.min(addr, seg_addr);
	}
	if (expect(sizeFile, ".bss")) {
	    seg_size = readHexInt(sizeFile);
	    seg_addr = readHexInt(sizeFile);
	    if (dbg)
	    System.out.println("\t.bss\t0x" + toHexString(seg_size) + "\t0x" + toHexString(seg_addr));
	    size += seg_size;
	    addr = Math.min(addr, seg_addr);
	}
//	writeHexInt(out, size); writeHexInt(out, addr);
	// XXX if ((addr & 0x80000000) == 0) addr += 0x80020000;
	out.writeInt( size); out.writeInt(addr); 	
	System.out.println(msg + "DATASIZE: 0x" + toHexString(size));
	System.out.println(msg + "DATAADDR: 0x" + toHexString(addr));
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
	    // System.out.print("readHexInt: ");
	    // skip white space
	    byte c = f.readByte();
	    while ((c == ' ') || (c == '\t')) {
		    c = f.readByte();
	    }
	    while (isHex(c)) {
		    tmp = readHexByte(c);
		    value = (value << 4) + (tmp & 0xf);
		    c = f.readByte();
	    }
	    return value;
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
	    return ((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f')) || (c == 'x');
    }		
}


class BootLinkerSizeFormatException extends Exception {
}
	
