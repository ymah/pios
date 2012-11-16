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
 *  @brief GPIO pin functions
 */
enum GPIOFunction
{
    /*!
     *  @brief Returned if the pin number is invalid
     */
    GPIO_FN_INVALID = -1,
    /*!
     *  @brief Sets the pin as an input pin
     */
    GPIO_FN_INPUT  = 0,
    /*!
     *  @brief Sets the pin as an output pin
     */
    GPIO_FN_OUTPUT = 1,
};

typedef enum GPIOFunction GPIOFunction;

struct GPIO;
typedef struct GPIO GPIO;

struct GPIODriver;
typedef struct GPIODriver GPIODriver;


/*!
 *  @brief allocate a GPIO
 *  @param driver the particular GPIO hardware driver
 *  @return An allocated but not initialised GPIO or NULL if it couldn't
 *  be allocated.
 */
GPIO* gpio_alloc(GPIODriver* driver);

/*!
 *  @brief Initialise the GPIO.
 *
 *  The first GPIO initialised is made the default GPIO.
 *  @param gpio GPIO to initialise
 *  @return the GPIO driver
 */
GPIO* gpio_init(GPIO* gpio);

/*!
 *  @brief Get the default GPIO
 *  @return the default GPIO
 */
GPIO* gpio_defaultGPIO(void);
/*!
 *  @brief set the GPIO pin function
 *  @param GPIO address of the GPIO registers
 *  @param pinNumber number of the pin to set the function for (0 - 53)
 *  @param function function to set.
 *	@return true if the function and pin number are valid and the function was
 *          set.
 */
bool gpio_setFunction(GPIO* gpio, uint32_t pinNumber, GPIOFunction function);

/*!
 *  @brief Get the current function of the given pin
 *
 *  @param gpio Address of the GPIO 
 *  @param pinNumber The lin number to get the function of.
 *  @return the function of the pin or GPIO_FN_INVALID if the pin is not between
 *  0 and 53
 */
GPIOFunction gpio_getFunction(GPIO* gpio, uint32_t pinNumber);

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
bool gpio_setPin(GPIO* gpio, uint32_t pinNumber, bool pinOn);

#endif
