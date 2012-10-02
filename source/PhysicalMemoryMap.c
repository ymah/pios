//
//  PhysicalMemoryMap.c
//  BakingPi
//
//  Created by Jeremy Pereira on 01/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#if defined PIOS_SIMULATOR

#include <dispatch/dispatch.h>

#endif

#include "PhysicalMemoryMap.h"

struct PhysicalMemoryMap
{
    SystemTimer* systemTimerAddress;
    GPIO* gpioAddress;
};

struct PhysicalMemoryMap defaultMap =
{
    (SystemTimer*) 0x20003000,
           (GPIO*) 0x20200000
};

#if defined PIOS_SIMULATOR

void pmm_init()
{
    static dispatch_once_t pred;
    dispatch_once(&pred,
    ^{
        defaultMap.systemTimerAddress = st_alloc();
        defaultMap.gpioAddress        = gpio_alloc();
    });
}

#endif


PhysicalMemoryMap* pmm_getPhysicalMemoryMap()
{
    return &defaultMap;
}


GPIO* pmm_getGPIOAddress(PhysicalMemoryMap* aMap)
{
    return aMap->gpioAddress;
}

SystemTimer* pmm_getSystemTimerAddress(PhysicalMemoryMap* map)
{
    return map->systemTimerAddress;
}
