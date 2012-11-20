//
//  SystemTimer.h
//  BakingPi
//
//  Created by Jeremy Pereira on 26/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_bcm2835SystemTimer_h
#define BakingPi_bcm2835SystemTimer_h

#include "SystemTimer.h"

/*!
 *  @brief struct representing the registers of the Broadcom system timer.
 */
struct BCM2835SystemTimerRegisters;
typedef struct BCM2835SystemTimerRegisters BCM2835SystemTimerRegisters;

#if defined PIOS_SIMULATOR

/*!
 *  @brief Allocate a system timer
 *  @return an empty system timer 
 */
BCM2835SystemTimerRegisters* bcst_alloc();

/*!
 *  @brief advance the microsecond counter by one tick
 */
void bcst_microsecondTick(BCM2835SystemTimerRegisters* timer);

#endif

/*!
 *  @brief Return the driver for the Broadcom system timer
 *  @return The Broadcom system timer driver.
 */
STDriver* bcst_driver(void);

#endif
