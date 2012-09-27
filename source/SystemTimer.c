//
//  SystemTimer.c
//  BakingPi
//
//  Created by Jeremy Pereira on 26/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "SystemTimer.h"

enum
{
    SYSTEM_TIMER_ADDRESS = 0x20003000,
    NUM_COMPARES = 4,
};

struct SystemTimer
{
    uint32_t controlStatus;
    uint32_t counter[2];
    uint32_t compare[NUM_COMPARES];
};

SystemTimer* getSystemTimerAddress(void)
{
    return (SystemTimer*) SYSTEM_TIMER_ADDRESS;
}

void st_microsecondSpin(SystemTimer* timer, uint32_t microseconds)
{
    uint32_t startTime = timer->counter[0];
    volatile int foo = 0;
    while (timer->counter[0] - startTime < microseconds)
    {
        foo++;	// Need to make sure the compiler does not optimise out the loop
    }
}

