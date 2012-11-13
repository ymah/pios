.section .text

.globl	__aeabi_uidivmod
.globl	__aeabi_uidiv

@
@  This is a 32 bit unsigned divide and modulus that is used by the EABI 
@  compiler to emulate division (the ARM CPU has no integer division).  
@  From disassembling a call to it, it appears that on entry:
@  r0 is numerator
@  r1 is divisor
@
@  On exit:
@  r0 is quotient
@  r1 is remainder
@
@  the functioncalculates both the quotient and the remainder but can be used 
@  as a function to calculate just the quotient since the ABI allows us to zap
@  r1 even if we are not returning it.
@ 
@  The algorithm is a fairly standard long division algorithm.
@
@	quotient = 0
@	shift up divisor so that MSB set aligns with the MSB set of the numerator
@	calculate iterations needed (number of shifts + 1)
@
@	while more iterations needed
@		shift quotient left
@		if numerator > divisor
@			set 1 in bottom bit of quotient
@			subtract divisor from numerator
@		shift divisor right
@
@	remainder = numerator
@
@   Before we do that though, there is a case where the divisor is greater than
@   the numerator where the quotient will be zero and the remainder will be
@   the numerator.  We also optimise out the case where the numerator and 
@   denominator are equal.
@

__aeabi_uidiv:
__aeabi_uidivmod:
@
@  If the numerator < divisor, the quotient is 0 and the remainder is the 
@  numerator
@
	cmp		r0, r1	
	beq		uidivmod_equalCase	
	bpl		uidivmod_standardCase
@
@   Case where the numerator is smaller than the denominator
@
	mov		r1, r0		@ remainder = numerator
	mov		r0, #0		@ quotient = 0
	bx		lr			@ We are done, bailing

uidivmod_equalCase:
@
@   Case where numerator and denominator are equal 
@	
	mov		r0, #1		@ quotient = 1
	mov		r1, #0		@ remainder = 0
	bx 		lr
	
uidivmod_standardCase:
@
@   r2 will be used as the quotient.
@   r6 and r7 are used to calculate the initial bit shift
@
	push	{ r6, r7, fp, lr}
@
@ quotient = 0
@
	mov		r2, #0
@
@ shift up divisor so that MSB set aligns.  At this point we know the numerator
@ is bigger than the divisor so we do not need to worry about negative shifts.
@
	clz		r6, r0				@ Calculate the leading zeros of the numerator	
	clz		r7, r1				@ Calculate leading zeros of divisor

@
@ calculate and perform shift on divisor.  The shift might be 0, so that is 
@  tested. The shift is done by calculating and getting the number of bits
@  shifted out of the low word and orring them with the shifted top word
@
uidivmod_LZDiff:
	subs	r6, r7, r6			@ Calculate the required shift
	beq		uidivmod_calcIterations

	mov		r1, r1, LSL r6		@ bottom word shifted
	
uidivmod_calcIterations:
	adds	r6, r6, #1			@ Iterations is 1  + number of shifts
	b		uidivmod_loopTest

@
@ while more iterations needed
@	
uidivmod_loop:
@
@  Shift quotient left 1 bit
@
	mov		r2, r2, LSL #1		@ Shift low word, top bit goes to carry
@
@ 	if numerator > divisor
@
	cmp		r0, r1
	bls		uidivmod_setOneEnd

uidivmod_setOne:
@
@ 	set 1 in bottom bit of quotient and subtract the divisor from the numerator
@
	orr		r2,	r2, #1
	sub		r0, r0, r1			@ low word setting carry if necessary
	
uidivmod_setOneEnd:
@
@ 	shift divisor right 1 bit
@
	mov		r1, r1, LSR #1
	
	subs	r6, r6, #1			@   subtract 1 from iterations
uidivmod_loopTest:
	bne		uidivmod_loop

uidivmod_loopExit:
@
@   Remainder is what is left of the numerator
@
	mov		r1, r0
@
@   quotient in r0
@
	mov		r0,	r2

__CLEAN_UP_aeabi_uidivmod:
	pop 	{ r6, r7, fp, lr }
	bx		lr	

.globl	__aeabi_uldivmod

@
@  This is a 64 bit unsigned divide that is used by the EABI compiler to 
@  emulate division (the ARM CPU has no integer ddivision.  
@  From disassembling a call to it, it appears that on entry:
@  r0 is numerator low
@  r1 is numerator high
@  r2 is divisor low
@  r3 is divisor high
@
@  On exit:
@  r0 is quotient low
@  r1 is quotient high
@  r2 is remainder low
@  r3 is remainder high
@ 
@  The algorithm is a fairly standard long division algorithm.
@
@	quotient = 0
@	shift up divisor so that MSB set aligns with the MSB set of the numerator
@	calculate iterations needed (number of shifts + 1)
@
@	while more iterations needed
@		shift quotient left
@		if numerator > divisor
@			set 1 in bottom bit of quotient
@			subtract divisor from numerator
@		shift divisor right
@
@	remainder = numerator
@
@   Before we do that though, there is a case where the divisor is greater than
@   the numerator where the quotient will be zero and the remainder will be
@   the numerator.  We also optimise out the case where the numerator and 
@   denominator are equal.
@

__aeabi_uldivmod:
@
@  If the numerator < divisor, the quotient is 0 and the remainder is the 
@  numerator
@
	cmp		r1, r3		@ high words first
	cmpeq	r0, r2		@ low words if high words are the same
	beq		uldivmod_equalCase
	bpl		uldivmod_standardCase
@
@   Case where the numerator is smaller than the denominator
@
	mov		r3, r1		@ remainder = numerator
	mov		r2, r0
	mov		r1, #0		@ quotient = 0
	mov		r0, #0
	bx		lr			@ We are done, bailing

uldivmod_equalCase:
@
@   Case where numerator and denominator are equal 
@	
	mov		r0, #1		@ quotient = 1
	mov		r1, #0
	mov		r2, #0		@ remainder = 0
	mov		r3, #0
	bx 		lr
	
uldivmod_standardCase:
@
@   r4, r5 will be used as the quotient.
@   r6 and r7 are used to calculate the initial bit shift
@   r8 is used as a scratch register
@
	push	{r4, r5, r6, r7, r8, fp, lr}
@
@ quotient = 0
@
	mov		r4, #0
	mov		r5, #0
@
@ shift up divisor so that MSB set aligns.  At this point we know the numerator
@ is bigger than the divisor so we do not need to worry about negative shifts.
@
	clz		r6, r1				@ Calculate the leading zeros of the numerator
	cmp		r6, #32				@ The high register might be entirely zeros
	bne		uldivmod_calcLZDivisor
	clz		r8, r0
	add		r6, r6, r8
	
uldivmod_calcLZDivisor:
	clz		r7, r3				@ Calculate leading zeros of divisor
	cmp		r7, #32				@ the high reg might be entirely zeros
	bne		uldivmod_LZDiff
	clz		r8, r2
	add		r7, r7, r8

@
@  calculate and perform shift on divisor.  The shift might be 0, so that is 
@  tested. The shift is done by calculating and getting the number of bits
@  shifted out of the low word and orring them with the shifted top word
@
uldivmod_LZDiff:
	subs	r6, r7, r6			@ Calculate the required shift
	beq		uldivmod_calcIterations

@
@  Split the actual shift into two cases, if the required shift is greater than
@  31:
@      top_word = low_word << (shift - 32)
@      low_word = 0
@
@  If the shift <= 31:
@      top_word <<= shift
@      top_word |= low_word >> (32 - shift)
@	   low_word <<= shift
@
	cmp		r6, #32
	bpl		uldivmod_bigShift
@
@   Shift < 32
@	
	mov		r3, r3, LSL	r6
	mov		r7, #32
	sub 	r7, r7, r6			@ 32 - shift
	orr		r3,	r2,	LSR r7
	mov 	r2,	r2,	LSL	r6
	b		uldivmod_calcIterations
@
@   Shift >= 32
@
uldivmod_bigShift:
	sub		r6, #32
	mov		r3, r2, LSL	r6
	mov		r2,	#0
	add		r6, #32
	
uldivmod_calcIterations:
	adds	r6, r6, #1			@ Iterations is 1  + number of shifts
	b		uldivmod_loopTest

@
@ while more iterations needed
@	
uldivmod_loop:
@
@  Shift quotient left 1 bit
@
	movs	r4, r4, LSL #1		@ Shift low word, top bit goes to carry
	adc		r5, r5, r5			@ Multiplies top word by 2 and adds carry
@
@ 	if numerator > divisor
@
	cmp		r1, r3
	cmpeq	r0, r2
	bls		uldivmod_setOneEnd
	
	
uldivmod_setOne:
@
@ 	set 1 in bottom bit of quotient and subtract the divisor from the numerator
@
	orr		r4,	r4, #1
	subs	r0, r0, r2			@ low word setting carry if necessary
	sbc		r1,	r1, r3			@ high word with carry
	
uldivmod_setOneEnd:
@
@ 	shift divisor right 1 bit
@
	movs	r3, r3, LSR #1		@   Shift top setting carry to bit shifted out
	mov		r2, r2, RRX			@ 	shift bottom, carry going to top bit 
	
	subs	r6, r6, #1			@   subtract 1 from iterations
uldivmod_loopTest:
	bne		uldivmod_loop

uldivmod_loopExit:
@
@   Remainder is what is left of the numerator
@
	mov		r3, r1
	mov		r2, r0
@
@   quotient in r0, r1
@
	mov		r1, r5
	mov		r0,	r4

__CLEAN_UP_aeabi_uldivmod:
	pop 	{r4, r5, r6, r7, r8, fp, lr }
	bx		lr	
	bx		lr
@ The last line seemed to be missing
