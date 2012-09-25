//
//  gpio.h
//  BakingPi
//
//  Created by Jeremy Pereira on 17/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

/*!
 *  @file gpio.h
 *  @brief GPIO controller functions
 *  @author Jeremy Pereira
 */

#ifndef BakingPi_gpio_h
#define BakingPi_gpio_h

#include "bptypes.h"


/*!
 *  @brief get the address of the GPIO controller
 *  @return the address of the GPIO controller.
 */
void* getGPIOAddress(void);

/*!
 *  @brief set the GPIO pin function
 *  @param pinNumber number of the pin to set the function for (0 - 53)
 *  @param function function to set.
 *	@return true if the function and pin number are valid and the function was
 *          set.
 */
bool setGPIOFunction(uint32_t pinNumber, uint32_t function);

#endif
