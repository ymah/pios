//
//  PhysicalMemoryMap.h
//  BakingPi
//
//  Created by Jeremy Pereira on 01/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_PhysicalMemoryMap_h
#define BakingPi_PhysicalMemoryMap_h

#include "bcm2835gpio.h"
#include "bcm2835SystemTimer.h"
#include "SystemTimer.h"
#include "FrameBuffer.h"
#include "Tag.h"
#include "VideoCore.h"
#include "pl110.h"


enum
{
    /*!
     *  @brief Number of bits of an address that represent the index within a
     *  page.
     */
    PIOS_PAGE_BITS = 12,
    /*!
     *  @brief The size of a page of memory.
     */
    PIOS_PAGE_SIZE = 1 << PIOS_PAGE_BITS,
};


struct PhysicalMemoryMap;
typedef struct PhysicalMemoryMap PhysicalMemoryMap;

#if defined PIOS_SIMULATOR

/*!
 *  @brief initialise the physical memory.
 *  @param tagSpace poiter to faked up ARM tags.
 *  @param systemFont location of system font which must be an 8 x 16 
 *  monospace raster font with at least characters from space (ASCII 32) to
 *  ~ (ASCII 0x7E)
 */
void pmm_init(uint32_t* tagSpace, uint8_t* systemFont);

#endif
/*!
 *  @brief Get the stop flag.
 *
 *  The physical memory map has a stop flag that can be checked to exit tight
 *  spin loops and hopefully stop the operating system.  This is a bit of a
 *  nuclear option.
 *  @return the stop flag.
 */
bool pmm_getStopFlag(PhysicalMemoryMap* map);

/*!
 *  @brief Set the stop flag.
 *
 *  The effect of this is that all tight spin loops exit straight away.
 *  @param map The physical memory map.
 *  @param shouldStop true if everything should stop now.
 */
void pmm_setStopFlag(PhysicalMemoryMap* map, bool shouldStop);

/*!
 *  @brief Get the physical memory map.
 *
 *  By default this gets the physical memory map associated with the actual
 *  hardwre as seen by the ARM CPU.
 *
 *  @return the physical memory map
 */
PhysicalMemoryMap* pmm_getPhysicalMemoryMap();

/*!
 *  @brief get the address of the Broadcom GPIO controller
 *  @param map The physical memory map
 *  @return the address of the GPIO controller.
 */
BCM2835GpioRegisters* pmm_getGPIOAddress(PhysicalMemoryMap* map);

/*!
 *  @brief Get the address of the BCM2835 system timer registers
 *  @param map Physical memory map.
 *  @return the address of the system timer
 */
BCM2835SystemTimerRegisters* pmm_getSystemTimerAddress(PhysicalMemoryMap* map);

/*!
 *  @brief get the address of the VideoCore postbox
 *  @return The frame buffer postbox
 */
VCPostBox* pmm_getVCPostBox(PhysicalMemoryMap* map);

/*!
 *  @brief Get the address of the PL110 controller if there is one.
 *  @return The PL110 address.
 */
PL110* pmm_getPL110(PhysicalMemoryMap* map);

/*!
 *  @brief Get the list of ARM tags
 *  @param map Memory map
 *  @return the tag list
 */
TagList* pmm_getTagList(PhysicalMemoryMap* map);

/*!
 *  @brief Get the system font.
 *  
 *  The system font is guaranteed to have characters for all of the ASCII 
 *  printable characters.
 *  @param map The physical memory map.
 *  @return A pointer to the beginning of the system font.
 */
uint8_t* pmm_getSystemFont(PhysicalMemoryMap* map);

/*!
 *  @brief Initialise the free page list for the map.  
 *  
 *  This will only work once at the moment because the page ist is allocated out
 *  of static storage.
 *  @param map Memory pmap containing the free page list
 */
void pmm_initialiseFreePages(PhysicalMemoryMap* map);
/*!
 *  @brief Allocate a page of memory
 *  @param map Memory pmap containing the free page list
 *  @return A single aligned page of memory or NULL if we don't have any left.
 */
void* pmm_allocatePage(PhysicalMemoryMap* map);
/*!
 *  @brief Allocate a set of contiguous pages.
 *  @param map Memory map to allocate pages from.
 *  @param numberOfPages Number of pages to allocate.
 *  @return A pointer to a contiguous set of pages.
 */
void* pmm_allocateContiguousPages(PhysicalMemoryMap* map, size_t numberOfPages);
/*!
 *  @brief free a page of memory.
 *  @param map Memory pmap containing the free page list
 *  @param pagePtr a pointer to the beginning of the page.  If it does not 
 *  point to the beginning of a page, it is ignored and nothing happens.
 */
void pmm_freePage(PhysicalMemoryMap* map, void* pagePtr);

#endif
