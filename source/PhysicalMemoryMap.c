//
//  PhysicalMemoryMap.c
//  BakingPi
//
//  Created by Jeremy Pereira on 01/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#if defined PIOS_SIMULATOR

#include <dispatch/dispatch.h>
#include <libkern/OSAtomic.h>

#endif

#include "PhysicalMemoryMap.h"

enum
{
    PIOS_PAGE_BITS = 12,
    PIOS_PAGE_SIZE = 1 << PIOS_PAGE_BITS,
    ALLOCATABLE_PAGES = 16,
};

#define PIOS_PAGE_MASK	(~((uintptr_t) 0) << PIOS_PAGE_BITS)

union Page
{
    uint8_t bytes[PIOS_PAGE_SIZE];
    union Page* next;
};

typedef union Page Page;

struct PhysicalMemoryMap
{
    SystemTimer* systemTimerAddress;
    GPIO* gpioAddress;
    FBPostBox* frameBufferPostBox;
    volatile bool stopFlag;
    Page* freePages;
};

static Page pageSpace[ALLOCATABLE_PAGES + 1];

struct PhysicalMemoryMap defaultMap =
{
    (SystemTimer*) 0x20003000,
           (GPIO*) 0x20200000,
      (FBPostBox*) 0x2000B880,
    false,
    NULL
};

#if defined PIOS_SIMULATOR
void pmm_init()
{
    static dispatch_once_t pred;
    dispatch_once(&pred,
    ^{
        defaultMap.systemTimerAddress = st_alloc();
        defaultMap.gpioAddress        = gpio_alloc();
        defaultMap.frameBufferPostBox = fb_postBoxAlloc();
        OSMemoryBarrier();
    });
    defaultMap.stopFlag = false;
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


FBPostBox* pmm_getFBPostBox(PhysicalMemoryMap* map)
{
    return map->frameBufferPostBox;
}


bool pmm_getStopFlag(PhysicalMemoryMap* map)
{
    return map->stopFlag;
}

void pmm_setStopFlag(PhysicalMemoryMap* map, bool shouldStop)
{
    map->stopFlag = shouldStop;
}

void pmm_initialiseFreePages(PhysicalMemoryMap* map)
{
    /*
     *  First thing is to obtain a page aligned pointer.  We have an array of 
     *  pages and there will be a page boundary somewhere in the first page.
     */
    uintptr_t pageAsInt = (uintptr_t)pageSpace;
    pageAsInt += PIOS_PAGE_SIZE - 1;
    pageAsInt &= PIOS_PAGE_MASK;
    Page* pagePtr = (Page*) pageAsInt;
    /*
     *  Now chain all the pages together in a linked list. All but the last page
     *  get a pointer to the next page.  The last usable page gets a NULL pointer
     */
    Page* lastPage = NULL;
    for (int i = 0 ; i < ALLOCATABLE_PAGES ; ++i)
    {
        Page* currentPage = &pagePtr[ALLOCATABLE_PAGES - 1 - i];
        currentPage->next = lastPage;
        lastPage = currentPage;
    }
    map->freePages = lastPage;
}

void* pmm_allocatePage(PhysicalMemoryMap* map)
{
    Page* ret = NULL;
    // TODO: Need a lock
    if (map->freePages != NULL)
    {
        ret = map->freePages;
        map->freePages = ret->next;
        ret->next = NULL;
        // TODO: Should zero the page
    }
    return ret;
}

void pmm_freePage(PhysicalMemoryMap* map, void* pagePtr)
{
    // TODO: Need a lock
    if (((uintptr_t)pagePtr & ~PIOS_PAGE_MASK) == 0)
    {
        ((Page*) pagePtr)->next = map->freePages;
        map->freePages = (Page*) pagePtr;
    }
}

