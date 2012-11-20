//
//  SystemTimer.c
//  BakingPi
//
//  Created by Jeremy Pereira on 26/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#if defined PIOS_SIMULATOR

#include <stdlib.h>
#include <stdio.h>
#include <libkern/OSAtomic.h>

#endif

#include "bcm2835SystemTimer.h"
#include "PhysicalMemoryMap.h"
#include "SystemTimer+Driver.h"
#include "PhysicalMemoryMap.h"

enum
{
    SYSTEM_TIMER_ADDRESS = 0x20003000,
    NUM_COMPARES = 4,
};

struct BCM2835SystemTimerRegisters
{
    volatile uint32_t controlStatus;
    volatile uint32_t counterLow;
    volatile uint32_t counterHigh;
    volatile uint32_t compare[NUM_COMPARES];
};

struct BCM2835Timer
{
    SystemTimer base;
    BCM2835SystemTimerRegisters* registers;
};

typedef struct BCM2835Timer BCM2835Timer;

static void microsecondSpin(SystemTimer* timer, uint32_t microseconds);
static uint64_t microseconds(SystemTimer* timer);
static SystemTimer* allocate(void);
static SystemTimer* init(SystemTimer* timer);
static void dealloc(SystemTimer* timer);




#if defined PIOS_SIMULATOR

BCM2835SystemTimerRegisters* bcst_alloc()
{
    return calloc(1, sizeof(BCM2835SystemTimerRegisters));
}

void bcst_microsecondTick(BCM2835SystemTimerRegisters* timer)
{
    if ((uint32_t)OSAtomicIncrement32((int32_t*)(&timer->counterLow)) == 0)
    {
        OSAtomicIncrement32Barrier((int32_t*)&timer->counterHigh);
    }
}

#endif

static BCM2835Timer theTimer = { { 0 }, NULL };

static STDriver theDriver =
{
    .allocate = allocate,
    .init = init,
    .dealloc = dealloc,
    .microsecondSpin = microsecondSpin,
    .microseconds = microseconds,
};

STDriver* bcst_driver()
{
    return &theDriver;
}

SystemTimer* allocate(void)
{
    theTimer.registers = NULL;
    return (SystemTimer*)&theTimer;
}

SystemTimer* init(SystemTimer* timer)
{
    ((BCM2835Timer*) timer)->registers
    	= pmm_getSystemTimerAddress(pmm_getPhysicalMemoryMap());
    return timer;
}

void dealloc(SystemTimer* timer)
{
    // Timer is statically allocated so do nothing.
}

void microsecondSpin(SystemTimer* timer, uint32_t microseconds)
{
    BCM2835Timer* bstTimer = (BCM2835Timer*) timer;
    uint32_t startTime = bstTimer->registers->counterLow;
    volatile int foo = 0;
    PhysicalMemoryMap* memoryMap = pmm_getPhysicalMemoryMap();
    
    while (bstTimer->registers->counterLow - startTime < microseconds
           && !pmm_getStopFlag(memoryMap))
    {
        foo++;	// Need to make sure the compiler does not optimise out the loop
    }
}

uint64_t microseconds(SystemTimer* timer)
{
    BCM2835Timer* bstTimer = (BCM2835Timer*) timer;
    uint64_t ret = ((uint64_t)(bstTimer->registers->counterHigh) << 32)
                   | bstTimer->registers->counterLow;
    return ret;
}

