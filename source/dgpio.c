//
//  dgpio.c
//  BakingPi
//
//  Created by Jeremy Pereira on 16/11/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "dgpio.h"

#include "gpio+driver.h"

enum
{
    NUM_PINS = 100,
};

static GPIO* driverAllocate(void);
static GPIO* driverInit(GPIO* gpio);
static void driverDealloc(GPIO* gpio);
static bool driverSetFunction(GPIO* gpio,
                              uint32_t pinNumber,
                    		  GPIOFunction function);
static GPIOFunction driverGetFunction(GPIO* gpio, uint32_t pinNumber);
static size_t driverPinCount(GPIO* gpio);
static bool driverSetPin(GPIO* gpio, uint32_t pinNumber, bool pinOn);


static struct GPIODriver theDriver =
{
    .allocate 		= driverAllocate,
    .init			= driverInit,
    .dealloc  		= driverDealloc,
    .setFunction 	= driverSetFunction,
    .getFunction 	= driverGetFunction,
    .pinCount 		= driverPinCount,
    .setPin 		= driverSetPin,
};

static GPIO theDummyGPIO;

GPIODriver* dgpio_driver(void)
{
    return &theDriver;
}

GPIO* driverAllocate(void)
{
    return &theDummyGPIO;
}

GPIO* driverInit(GPIO* gpio)
{
    return gpio;
}

void driverDealloc(GPIO* gpio)
{
    // The driver is currently statically allocated.
}

bool driverSetFunction(GPIO* gpio,
                       uint32_t pinNumber,
                       GPIOFunction function)
{
    return pinNumber < NUM_PINS && function == GPIO_FN_OUTPUT;
}

GPIOFunction driverGetFunction(GPIO* gpio, uint32_t pinNumber)
{
    return pinNumber < NUM_PINS ? GPIO_FN_OUTPUT : GPIO_FN_INVALID;
}

size_t driverPinCount(GPIO* gpio)
{
    return NUM_PINS;
}

bool driverSetPin(GPIO* gpio, uint32_t pinNumber, bool pinOn)
{
    return pinNumber < NUM_PINS;
}

