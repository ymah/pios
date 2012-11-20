//
//  SystemTimer.h
//  BakingPi
//
//  Created by Jeremy Pereira on 26/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_SystemTimer_h
#define BakingPi_SystemTimer_h
/*!
 *  @file SystemTimer.h
 *  @brief Definitions relating to the system timer
 */

#include "bptypes.h"

/*!
 *  @brief Opaque type to represent the system timer
 */
struct SystemTimer;
typedef struct SystemTimer SystemTimer;

/*!
 *  @brief driver functions for a system timer driver.
 */
struct STDriver;
typedef struct STDriver STDriver;

#if defined PIOS_SIMULATOR


/*!
 *  @brief advance the microsecond counter by one tick
 */
void st_microsecondTick(SystemTimer* timer);

#endif

/*!
 *  @brief Allocate a system timer
 *  @param driver The system timer driver functions.
 *  @return an empty system timer
 */
SystemTimer* st_alloc(STDriver* driver);

/*!
 *  @brief Initialise the system timer.
 *  @param uninitialisedTimer An uninitialised system timer.
 *  @return The initialised system timer.
 */
SystemTimer* st_init(SystemTimer* uninitialisedTimer);

/*!
 *  @brief Deallocate the system timer.
 *  @param timer Timer to deallocate.
 */
void st_dealloc(SystemTimer* timer);

/*!
 *  @brief Get the default system timer.
 *  
 *  This will be the first system timer initialised.
 *  @return the default system timer.
 */
SystemTimer* st_defaultTimer(void);

/*!
 *  @brief wait or the given number of microseconds
 *
 *  Wait for the given number of microseconds.  This is implemented by polling 
 *  the system timer in a tight loop.
 *  @param timer Timer to wait on
 *  @param microseconds number of microseconds to wait
 */
void st_microsecondSpin(SystemTimer* timer, uint32_t microseconds);

uint64_t st_microSeconds(SystemTimer* timer);

#endif
