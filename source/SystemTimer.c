//
//  SystemTimer.c
//  BakingPi
//
//  Created by Jeremy Pereira on 26/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#if defined PIOS_SIMULATOR

#include <stdlib.h>

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
    volatile uint32_t counter[2];
    volatile uint32_t compare[NUM_COMPARES];
};

#if defined PIOS_SIMULATOR

SystemTimer* st_alloc()
{
    return calloc(1, sizeof(SystemTimer));
}

void st_microsecondTick(SystemTimer* timer)
{
    uint64_t timerValue = timer->counter[0] | ((uint64_t) timer->counter[1] << 32);
    timerValue++;
    timer->counter[0] = timerValue & 0xFFFFFFFF;
    timer->counter[1] = timerValue >> 32;
    // TODO:  We need a memory barrier
}

#endif


void st_microsecondSpin(SystemTimer* timer, uint32_t microseconds)
{
    uint32_t startTime = timer->counter[0];
    volatile int foo = 0;
    PhysicalMemoryMap* memoryMap = pmm_getPhysicalMemoryMap();
    
    while (timer->counter[0] - startTime < microseconds
           && !pmm_getStopFlag(memoryMap))
    {
        foo++;	// Need to make sure the compiler does not optimise out the loop
    }
}

