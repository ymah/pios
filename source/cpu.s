.globl	saveRegs
.globl	restoreRegs
.globl	contextSwitch
.globl	interruptcode
.globl	resetVector
.globl	undefinedFunc
.globl	swiFunc
.globl	prefetchAbortFunc
.globl	dataAbortFunc
.globl	irqFunc
.globl	fastIrqFunc
.globl	syscall


@
@   Saves all the registers except those associated with the stack and program
@   counter.
@   @param r0 block to save registers to.
@	
saveRegs:
	stm		r0,{r0-r15}
	bx 		lr

@
@  Restores the registers except those associated with the stack and program
@  counter	
@  @param r0 block to get th registers from
@ 
restoreRegs:
	ldm		r0,{r0-r12}
	bx		lr

@
@   Perform a context switch.  This means saving all the registers to the 
@   from block and then retrieving the registers from the to block.  The one 
@   register we do not change is the program counter because, if this is the 
@   only place  we do a context switch, it is guaranteed to be correct already.
@   
@   @param r0 from register block
@   @param r1 to register block
@ 
contextSwitch:
	stm		r0,{r0-r15}
	ldm		r1,{r0-r14}
	bx		lr
	
@
@  Interrupt handling
@
interruptCode:
	ldr		pc,[pc, #(resetVector - . - 8)]
	ldr		pc,[pc, #(undefinedVector - . - 8)]
	ldr		pc,[pc, #(swiVector - . - 8)]
	ldr		pc,[pc, #(prefetchAbortVector - . - 8)]
	ldr		pc,[pc, #(dataAbortVector - . - 8)]
	.word	0
	ldr		pc,[pc, #(irqVector - . - 8)]
	ldr		pc,[pc, #(fastIrqVector - . - 8)]

resetVector:			.word 	0
undefinedVector:		.word 	0
swiVector:				.word	0
prefetchAbortVector:	.word	0
dataAbortVector:		.word	0
						.word	0	@ reserved
irqVector:				.word	0
fastIrqVector:			.word	0

@
@  Interrupt handler functions.  Each one needs to restore enough environment
@  (stack etc) to call the real handler for the interrupt.  
@
undefinedFunc:
	b 	undefinedFunc
swiFunc:
	b 	swiFunc
prefetchAbortFunc: 
	b 	prefetchAbortFunc
dataAbortFunc:
	b 	dataAbortFunc
irqFunc:
	b 	irqFunc
fastIrqFunc:
	b 	fastIrqFunc

syscall:
	push 	{lr}
	swi		#1
	pop		{lr}
	bx		lr
	
@ last line

