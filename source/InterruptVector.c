//
//  InterruptVector.c
//  BakingPi
//
//  Created by Jeremy Pereira on 29/11/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "InterruptVector.h"
#include "PhysicalMemoryMap.h"

enum
{
    INTERRUPT_INSTRUCTION = 0xe59ff018, // ldr pc,[pc, #24]
    RESERVED_LOCATION = 5,				// Not used as a vector yet
};

struct InterruptVectorTable
{
    InterruptVector vectors[IV_NUM_VECTORS];
};

extern void resetFunc(void);
extern void undefinedFunc(void);
extern void prefetchAbortFunc(void);
extern void dataAbortFunc(void);
extern void swiFunc(void);
extern void irqFunc(void);
extern void fastIrqFunc(void);

void iv_initialise(void)
{
#if !defined PIOS_SIMULATOR
    uint32_t* ivCodeLocation
    	= pmm_getIVCodeLocation(pmm_getPhysicalMemoryMap());
    /*
     *  Set the ARM interrupt locations to the LDR instruction
     */
    for (int i = 0 ; i < IV_NUM_VECTORS ; ++i)
    {
        if (i != RESERVED_LOCATION)
        {
            ivCodeLocation[i] = INTERRUPT_INSTRUCTION;
        }
    }
    /*
     *  The vector array is straight after the instructions, set it up.
     */
    uint32_t* vectorArray = ivCodeLocation + IV_NUM_VECTORS;
    vectorArray[IV_RESET] = (uint32_t)resetFunc;
    vectorArray[IV_UNDEFINED_INSTRUCTION] = (uint32_t)undefinedFunc;
    vectorArray[IV_INSTRUCTION_ABORT] = (uint32_t)prefetchAbortFunc;
    vectorArray[IV_DATA_ABORT] = (uint32_t)dataAbortFunc;
    vectorArray[IV_SOFTWARE] = (uint32_t)swiFunc;
    vectorArray[IV_IRQ] = (uint32_t)irqFunc;
    vectorArray[IV_FAST_IRQ] = (uint32_t)fastIrqFunc;
#endif
}