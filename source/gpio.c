//
//  gpio.c
//  BakingPi
//
//  Created by Jeremy Pereira on 17/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#if defined PIOS_SIMULATOR

#include <stdlib.h>

#endif

#include "gpio.h"
#include "gpio+driver.h"

static GPIO* defaultGPIO = NULL;

GPIO* gpio_alloc(GPIODriver* driver)
{
    GPIO* ret = driver->allocate();
    ret->driver = driver;
    return ret;
}

GPIO* gpio_init(GPIO* gpio)
{
    GPIO* ret = gpio->driver->init(gpio);
    if (defaultGPIO == NULL)
    {
        defaultGPIO = ret;
    }
    return ret;
}

GPIO* gpio_defaultGPIO(void)
{
    return defaultGPIO;
}

bool gpio_setFunction(GPIO* gpio, uint32_t pinNumber, GPIOFunction function)
{
    return gpio->driver->setFunction(gpio, pinNumber, function);
}

GPIOFunction gpio_getFunction(GPIO* gpio, uint32_t pinNumber)
{
    return gpio->driver->getFunction(gpio, pinNumber);
}

bool gpio_setPin(GPIO* gpio, uint32_t pinNumber, bool pinOn)
{
    return gpio->driver->setPin(gpio, pinNumber, pinOn);
}
