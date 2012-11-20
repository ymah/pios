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

#include "DSystemTimer.h"
#include "PhysicalMemoryMap.h"
#include "SystemTimer+Driver.h"
#include "PhysicalMemoryMap.h"

struct DummyTimer
{
    SystemTimer base;
    volatile uint64_t theTime;
};

typedef struct DummyTimer DummyTimer;

static void microsecondSpin(SystemTimer* timer, uint32_t microseconds);
static uint64_t microseconds(SystemTimer* timer);
static SystemTimer* allocate(void);
static SystemTimer* init(SystemTimer* timer);
static void dealloc(SystemTimer* timer);


DummyTimer theTimer = { { 0 } };

static STDriver theDriver =
{
    .allocate = allocate,
    .init = init,
    .dealloc = dealloc,
    .microsecondSpin = microsecondSpin,
    .microseconds = microseconds,
};

STDriver* dst_driver()
{
    return &theDriver;
}

SystemTimer* allocate(void)
{
    return (SystemTimer*)&theTimer;
}

SystemTimer* init(SystemTimer* timer)
{
    ((DummyTimer*) timer)->theTime = 0;
    return timer;
}

void dealloc(SystemTimer* timer)
{
    // Timer is statically allocated so do nothing.
}

void microsecondSpin(SystemTimer* timer, uint32_t microseconds)
{
    DummyTimer* dstTimer = (DummyTimer*) timer;
    uint64_t theOldTime = dstTimer->theTime;
    while (dstTimer->theTime < theOldTime + microseconds)
    {
        dstTimer->theTime++;
    }
}

uint64_t microseconds(SystemTimer* timer)
{
    DummyTimer* dstTimer = (DummyTimer*) timer;
    dstTimer->theTime++;
    uint64_t ret = dstTimer->theTime;
    return ret;
}

