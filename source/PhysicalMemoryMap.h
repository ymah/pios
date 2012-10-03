//
//  PhysicalMemoryMap.h
//  BakingPi
//
//  Created by Jeremy Pereira on 01/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_PhysicalMemoryMap_h
#define BakingPi_PhysicalMemoryMap_h

#include "gpio.h"
#include "SystemTimer.h"

struct PhysicalMemoryMap;
typedef struct PhysicalMemoryMap PhysicalMemoryMap;

#if defined PIOS_SIMULATOR

void pmm_init();


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
 *  @brief get the address of the GPIO controller
 *  @param map The physical memory map
 *  @return the address of the GPIO controller.
 */
GPIO* pmm_getGPIOAddress(PhysicalMemoryMap* map);

/*!
 *  @brief Get the address of the system timer
 *  @param map Physical memory map.
 *  @return the address of the system timer
 */
SystemTimer* pmm_getSystemTimerAddress(PhysicalMemoryMap* map);


#endif
