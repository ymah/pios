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

enum
{
    /*!
     *  @brief number of pins on the GPIO controller
     */
    GPIO_NUM_PINS = 54
};

/*!
 *  @brief GPIO pin functions
 */
enum GPIOFunction
{
    /*!
     *  @brief Sets the pin as an input pin
     */
    GPIO_FN_INPUT  = 0,
    /*!
     *  @brief Sets the pin as an output pin
     */
    GPIO_FN_OUTPUT = 1,
    GPIO_FN_ALT_0  = 2,
    GPIO_FN_ALT_1  = 3,
    GPIO_FN_ALT_2  = 4,
    GPIO_FN_ALT_3  = 5,
    GPIO_FN_ALT_4  = 6,
    GPIO_FN_ALT_5  = 7,
};

typedef enum GPIOFunction GPIOFunction;

struct GPIO;
typedef struct GPIO GPIO;

#if defined PIOS_SIMULATOR

GPIO* gpio_alloc();

#endif

/*!
 *  @brief set the GPIO pin function
 *  @param GPIO address of the GPIO registers
 *  @param pinNumber number of the pin to set the function for (0 - 53)
 *  @param function function to set.
 *	@return true if the function and pin number are valid and the function was
 *          set.
 */
bool setGPIOFunction(GPIO* gpio, uint32_t pinNumber, GPIOFunction function);

/*!
 *  @brief Set the pin either on or off.
 *
 *  The pin should be set to output before calling this function or it will
 *  hopefully have no effect (as opposed to blowing up the IO controller).
 *  @param GPIO address of the GPIO registers
 *  @param pinNumber number of the pin to set (0 - 53)
 *  @param pinOn true if the pin should be set on, false to set it off
 *  @return true if the pin number is valid. 
 */
bool setGPIOPin(GPIO* gpio, uint32_t pinNumber, bool pinOn);

#endif
