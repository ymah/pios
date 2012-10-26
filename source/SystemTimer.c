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

#include "SystemTimer.h"
#include "PhysicalMemoryMap.h"

enum
{
    SYSTEM_TIMER_ADDRESS = 0x20003000,
    NUM_COMPARES = 4,
};

struct SystemTimer
{
    volatile uint32_t controlStatus;
    volatile uint32_t counterLow;
    volatile uint32_t counterHigh;
    volatile uint32_t compare[NUM_COMPARES];
};

#if defined PIOS_SIMULATOR

SystemTimer* st_alloc()
{
    return calloc(1, sizeof(SystemTimer));
}

void st_microsecondTick(SystemTimer* timer)
{
    if ((uint32_t)OSAtomicIncrement32((int32_t*)(&timer->counterLow)) == 0)
    {
        OSAtomicIncrement32Barrier((int32_t*)&timer->counterHigh);
    }
}

#endif


void st_microsecondSpin(SystemTimer* timer, uint32_t microseconds)
{
#if defined PIOS_SIMULATOR
    fprintf(stderr, "Spinning for %d us\n", (int) microseconds);
#endif
    uint32_t startTime = timer->counterLow;
    volatile int foo = 0;
    PhysicalMemoryMap* memoryMap = pmm_getPhysicalMemoryMap();
    
    while (timer->counterLow - startTime < microseconds
           && !pmm_getStopFlag(memoryMap))
    {
        foo++;	// Need to make sure the compiler does not optimise out the loop
    }
#if defined PIOS_SIMULATOR
    fprintf(stderr, "Spun for %d us\n", (int) microseconds);
#endif
}

uint64_t st_microSeconds(SystemTimer* timer)
{
    uint64_t ret = ((uint64_t)(timer->counterHigh) << 32) | timer->counterLow;
    return ret;
}

