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

enum
{
    GPIO_ADDRESS = 0x20200000,
    PINS_PER_BLOCK = 10,
    PIN_BLOCK_SIZE = 4,				// in bytes
    FUNCTION_SELECT_REGISTERS = 6,	// How many function select registers
    PIN_BANK_SIZE = 2,				// How many regs to set or clear output
};

/*
 *  All the GPIO registers are 32 bit, so we shouldn't have any trouble with 
 *  packing and alignment.  The size of this structure should be 41 * 4 bytes.
 */
struct GPIO
{
    uint32_t functionSelect[FUNCTION_SELECT_REGISTERS];
    uint32_t reserved1;
    uint32_t outputSet[PIN_BANK_SIZE];
    uint32_t reserved2;
    uint32_t outputClear[PIN_BANK_SIZE];
    uint32_t reserved3;
    uint32_t pinLevel[PIN_BANK_SIZE];
    uint32_t reserved4;
    uint32_t pinEventDetectStatus[PIN_BANK_SIZE];
    uint32_t reserved5;
    uint32_t risingEdgeDetectEnable[PIN_BANK_SIZE];
    uint32_t reserved6;
    uint32_t fallingEdgeDetectEnable[PIN_BANK_SIZE];
    uint32_t reserved7;
    uint32_t highDetectEnable[PIN_BANK_SIZE];
    uint32_t reserved8;
    uint32_t lowDetectEnable[PIN_BANK_SIZE];
    uint32_t reserved9;
    uint32_t risingEdgeDetect[PIN_BANK_SIZE];
    uint32_t reserved10;
    uint32_t fallingEdgeDetect[PIN_BANK_SIZE];
    uint32_t reserved11;
    uint32_t pullUpDownEnable;
    uint32_t pullUpDownEnableClock[PIN_BANK_SIZE];
    uint32_t reserved12;
#if defined PIOS_SIMULATOR
    volatile uint64_t pinOutputMask;
    volatile uint64_t lastPinOutputState;
    volatile uint64_t pinOutputState;
#endif
};

#if defined PIOS_SIMULATOR

GPIO* gpio_alloc()
{
    return calloc(1, sizeof(GPIO));
}

bool gpio_outputPinsHaveChanged(GPIO* gpio)
{
    bool ret =  (gpio->pinOutputMask & gpio->pinOutputState)
                != (gpio->pinOutputMask & gpio->lastPinOutputState);
    gpio->lastPinOutputState = gpio->pinOutputState;
    return ret;
}

bool gpio_outputPinValue(GPIO* gpio, uint32_t pinNumber)
{
    bool ret = true;	// If no output set pull up
    if (gpio_getFunction(gpio, pinNumber) == GPIO_FN_OUTPUT)
    {
        ret = (gpio->pinOutputState & (1 << pinNumber)) != 0;
    }
    return ret;
}

#endif


bool gpio_setFunction(GPIO* gpioAddress,
                      uint32_t pinNumber,
                      GPIOFunction function)
{
    bool ret = false;
    
    if (pinNumber < GPIO_NUM_PINS && function <= GPIO_FN_ALT_5)
    {
        /*
         *  First find the index of the 32 bit word containing the pin
         *  function bits.  This is done by repeated subtraction because 
         *  division requires a library call
         */
        unsigned int selectRegisterIndex = 0;
        uint32_t pinOffset = pinNumber;
        while (pinOffset >= PINS_PER_BLOCK)
        {
            pinOffset -= PINS_PER_BLOCK;
            selectRegisterIndex++;
        }
        uint32_t pinLowBit = 3 * pinOffset;
        /*
         *  Get the register masking out the function bits for our pin
         */
        uint32_t gpioRegister
        	= gpioAddress->functionSelect[selectRegisterIndex] & ~(7 << pinLowBit);
        /*
         *  Set the function and put it back in the GPIO
         */
        gpioAddress->functionSelect[selectRegisterIndex]
        	= gpioRegister | (function << pinLowBit);
        
        ret = true;
    }
#if defined PIOS_SIMULATOR
    switch (function)
    {
        case GPIO_FN_OUTPUT:
            gpioAddress->pinOutputMask |= 1 << pinNumber;
            break;
            
        default:
            gpioAddress->pinOutputMask &= ~(1 << pinNumber);
            break;
    }
#endif
    return ret;
}

GPIOFunction gpio_getFunction(GPIO* gpio, uint32_t pinNumber)
{
    GPIOFunction ret = GPIO_FN_INVALID;
    if (pinNumber < GPIO_NUM_PINS)
    {
        /*
         *  First find the index of the 32 bit word containing the pin
         *  function bits.  This is done by repeated subtraction because
         *  division requires a library call
         */
        unsigned int selectRegisterIndex = 0;
        while (pinNumber >= PINS_PER_BLOCK)
        {
            pinNumber -= PINS_PER_BLOCK;
            selectRegisterIndex++;
        }
        uint32_t pinLowBit = 3 * pinNumber;
        uint32_t registerValue = gpio->functionSelect[selectRegisterIndex];
        ret = (registerValue >> pinLowBit) & 7;
    }
    return ret;
}


bool setGPIOPin(GPIO* gpioAddress, uint32_t pinNumber, bool pinOn)
{
    bool ret =false;
    
    if (pinNumber < GPIO_NUM_PINS)
    {
        /*
         *  The GPIO controller has two 32 bit "banks" for controlling pin 
         *  output.  Find the address of the bank we want
         */
        int pinBank = pinNumber < 32 ? 0 : 1;
        if (pinBank == 1)
        {
            pinNumber -= 32;
        }
        if (pinOn)
        {
            gpioAddress->outputSet[pinBank] |= 1 << pinNumber;
        }
        else
        {
            gpioAddress->outputClear[pinBank] |= 1 << pinNumber;
        }
#if defined PIOS_SIMULATOR
        /*
         *  In the simulator we need to update the state of the pin we have just
         *  set.
         */
        if (pinBank == 1)
    	{
            pinNumber += 32;
        }
        if (pinOn)
        {
            gpioAddress->pinOutputState |= 1 << pinNumber;
        }
        else
        {
            gpioAddress->pinOutputState &= ~(1 << pinNumber);
        }
#endif
        ret = true;
    }
    return ret;
}
