/******************************************************************************
*	kernel.ld
*	 by Alex Chadwick
*
*	A linker script for generation of raspberry pi kernel images.
******************************************************************************/

SECTIONS {
	/*
	* First and formost we need the .init section
	* We allow room for the ATAGs and the stack and then start our code at
	* 0x8000.
	*/
	.init 0x8000 : {
		*(.init)
	}
	
	.text : {
		*(.text)
	}
	
	.rodata : { *(.rodata) }
	/* 
	* Next we put the data.
	*/
	.data : {
		*(.data)
	}
	.bss : {
		*(.bss)
	}

	/*
	* Finally comes everything else. A fun trick here is to put all other 
	* sections into this section, which will be discarded by default.
	*/
	/DISCARD/ : {
		*(*)
	}
	
	. = ALIGN(4096);
	. = . + 0x10000;
	stackTop = .;

}
