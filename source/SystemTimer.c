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

#include "SystemTimer+Driver.h"
#include "PhysicalMemoryMap.h"

SystemTimer* defaultTimer = NULL;

SystemTimer* st_defaultTimer()
{
    return defaultTimer;
}

void st_microsecondSpin(SystemTimer* timer, uint32_t microseconds)
{
    timer->driver->microsecondSpin(timer, microseconds);
}

uint64_t st_microSeconds(SystemTimer* timer)
{
    return timer->driver->microseconds(timer);
}

SystemTimer* st_alloc(STDriver* driver)
{
    SystemTimer* ret = driver->allocate();
    if (ret != NULL)
    {
        ret->driver = driver;
    }
    return ret;
}

SystemTimer* st_init(SystemTimer* uninitialisedTimer)
{
    SystemTimer* ret = uninitialisedTimer->driver->init(uninitialisedTimer);
    if (defaultTimer != NULL)
    {
        defaultTimer = ret;
    }
    return ret;
}

