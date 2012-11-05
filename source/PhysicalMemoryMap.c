//
//  PhysicalMemoryMap.c
//  BakingPi
//
//  Created by Jeremy Pereira on 01/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "PhysicalMemoryMap.h"

#if defined PIOS_SIMULATOR

#include <dispatch/dispatch.h>
#include <libkern/OSAtomic.h>

#else

extern uint8_t fontMap;

#endif



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
    VCPostBox* frameBufferPostBox;
    uint32_t* tagSpace;
    Page* freePages;
    uint8_t* systemFont;
    volatile bool stopFlag;
};

struct PhysicalMemoryMap defaultMap =
{
    (SystemTimer*) 0x20003000,
           (GPIO*) 0x20200000,
      (VCPostBox*) 0x2000B880,
       (uint32_t*) 0x00000100,
    NULL,	// free pages
    NULL,	// system font
    false
};

#if defined PIOS_SIMULATOR
void pmm_init(uint32_t* tagSpace, uint8_t* systemFont)
{
    static dispatch_once_t pred;
    dispatch_once(&pred,
    ^{
        defaultMap.systemTimerAddress = st_alloc();
        defaultMap.gpioAddress        = gpio_alloc();
        defaultMap.frameBufferPostBox = vc_postBoxAlloc();
        defaultMap.tagSpace			  = tagSpace;
        defaultMap.systemFont		  = systemFont;
        OSMemoryBarrier();
    });
    defaultMap.stopFlag = false;
}

#endif

PhysicalMemoryMap* pmm_getPhysicalMemoryMap()
{
#if !defined PIOS_SIMULATOR
//    if (defaultMap.systemFont == NULL)
//    {
        defaultMap.systemFont = &fontMap;
//    }
#endif
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


VCPostBox* pmm_getVCPostBox(PhysicalMemoryMap* map)
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

TagList* pmm_getTagList(PhysicalMemoryMap* map)
{
    return tag_initialiseTagList(map->tagSpace);
}

uint8_t* pmm_getSystemFont(PhysicalMemoryMap* map)
{
    return map->systemFont;
}

#if defined PIOS_SIMULATOR

extern uint8_t heap[];
#define stackTop (heap[0])

#else
/*
 * Is the byte immediately above the top of the stack
 */
extern uint8_t stackTop;

#endif



void pmm_initialiseFreePages(PhysicalMemoryMap* map)
{
    /*
     *  First thing is to obtain a page aligned pointer.  We have an array of 
     *  pages and there will be a page boundary somewhere in the first page.
     */
    uintptr_t bottomAsInt = (uintptr_t)&stackTop;
    uintptr_t pageAsInt = bottomAsInt + PIOS_PAGE_SIZE - 1;
    pageAsInt &= PIOS_PAGE_MASK;
    
    TagList* tagList = pmm_getTagList(map);
    uintptr_t topOfRAM = tag_memoryTop(tagList);
    /*
     *  We can only allocate pages to the free list if we know where the top of
     *  RAM is.
     */
    if (topOfRAM > pageAsInt)
    {
        Page* pagePtr = (Page*) pageAsInt;
        size_t size = topOfRAM - pageAsInt;
        size_t allocatablePages = size / PIOS_PAGE_SIZE;
        /*
         *  Now chain all the pages together in a linked list. All but the last page
         *  get a pointer to the next page.  The last usable page gets a NULL pointer
         */
        Page* lastPage = NULL;
        for (int i = 0 ; i < allocatablePages ; ++i)
        {
            Page* currentPage = &pagePtr[allocatablePages - 1 - i];
            currentPage->next = lastPage;
            lastPage = currentPage;
        }
        map->freePages = lastPage;
    }
    else
    {
        map->freePages = NULL;
    }
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

