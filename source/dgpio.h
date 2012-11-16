//
//  dgpio.h
//  BakingPi
//
//  Created by Jeremy Pereira on 16/11/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_dgpio_h
#define BakingPi_dgpio_h

#include "gpio.h"

/*!
 *  @brief Get the dummy GPIO driver
 *  @return The dummy GPIO driver
 */
GPIODriver* dgpio_driver(void);

#endif
