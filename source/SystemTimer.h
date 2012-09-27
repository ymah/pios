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
 *  @brief Get a pointer to the system timer 
 *  @return The system timer address
 */
SystemTimer* getSystemTimerAddress(void);

/*!
 *  @brief wait or the given number of microseconds
 *
 *  Wait for the given number of microseconds.  This is implemented by polling 
 *  the system timer in a tight loop.
 *  @param timer Timer to wait on
 *  @param microseconds number of microseconds to wait
 */
void st_microsecondSpin(SystemTimer* timer, uint32_t microseconds);

#endif
