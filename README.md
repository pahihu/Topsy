# TOPSY
TOPSY - A Teachable Operating System. Swiss Federal Institute of Technology, Computer Engineering and Networks Laboratory

The code is tested on macOS 10.13.6 "High Sierra".  The GCC toolchain is patched to compile cleanly.
The LCC toolchain uses the mips-idt-elf binutils from the GCC toolchain, patched the mips/irix target 
to produce a cross-compiler.

To set up either the GCC or the LCC toolchain run `mkcfg gcc` or `mkcfg lcc`.
The default is to use the GCC toolchain.

Changes:

  * GCC mips-idt-elf port
  * LCC 4.2 port

**NOTE**: The LCC produced topsy.srec does not work yet.
