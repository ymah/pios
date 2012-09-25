.section .init
.globl	_start

/*!
 * @brief Entry point for our program.  At the moment the memory map has 
 * .init = 0x00000000
 * ATAGS = 0x00000100
 * Stack = ATAGS top - 0x00007FFF
 * .text = 0x00008000 +
 */
_start:
		b 	skipATAGs
.section	.text
skipATAGs:
		mov	sp,#0x8000	@@ Set up the stack pointer
		mov	r0,#1
		ldr	r1,=kernelArgs
		bl	main

loop$:
		b loop$
		