//
//  gpio_driver.h
//  BakingPi
//
//  Created by Jeremy Pereira on 14/11/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_gpio_driver_h
#define BakingPi_gpio_driver_h

#include "gpio.h"

/*!
 *  @brief The GPdriver abstracts the functionality of a specific GPIO chip t
 */
struct GPIODriver
{
    /*!
     *  @brief allocate enough memory to contain a GPIO struct of the relevant
     *  subclass.
     *  @return a GPIO struct + subclass.
     */
    GPIO* (*allocate)(void);
    /*!
     *  @brief initialise the GPIO including hardware initialisation.
     *  @param gpio The GPIO to initialise.
     *  @return The initialised GPIO
     */
    GPIO* (*init)(GPIO* gpio);
    /*!
     *  @brief deallocate a GPIO struct
     */
    void (*dealloc)(GPIO* gpio);
    /*!
     *  @brief set the GPIO pin function
     *  @param GPIO address of the GPIO registers
     *  @param pinNumber number of the pin to set the function for (0 - 53)
     *  @param function function to set.
     *	@return true if the function and pin number are valid and the function was
     *          set.
     */    
    bool (*setFunction)(GPIO* gpio,
                    	uint32_t pinNumber,
                        GPIOFunction function);
    /*!
     *  @brief Get the current function of the given pin
     *
     *  @param gpio Address of the GPIO
     *  @param pinNumber The lin number to get the function of.
     *  @return the function of the pin or GPIO_FN_INVALID if the pin is not 
     *  between 0 and the number of pins on this GPIO controller.
     */
    GPIOFunction (*getFunction)(GPIO* gpio, uint32_t pinNumber);
	/*!
     *  @brief Get the pin count for this GPIO controller
     *  @param gpio Address of the GPIO
     *  @return The pin count for the GPIO controller.
     */
    size_t (*pinCount)(GPIO* gpio);
    
    /*!
     *  @brief Set a particular output pin.
     *
     *  Sets a particular output bin to a particular value.  This has no effect
     *  if the pin has not been set to output.
     *  @param gpio The GPIO for which to set the pin.
     *  @param pinNumber Which pin to set.
     *  @param pinOn true for on and false for off.
     *  @return true unless the pin is invalid.
     */
    bool (*setPin)(GPIO* gpio, uint32_t pinNumber, bool pinOn);
};

/*!
 *  @brief GPIO base object
 */
struct GPIO
{
    GPIODriver* driver;
};

#endif
