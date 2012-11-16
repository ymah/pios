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

#include "bcm2835gpio.h"
#include "gpio.h"
#include "gpio+driver.h"
#include "PhysicalMemoryMap.h"

enum
{
    NUM_PINS = 54,
    ADDRESS = 0x20200000,
    PINS_PER_BLOCK = 10,
    PIN_BLOCK_SIZE = 4,				// in bytes
    FUNCTION_SELECT_REGISTERS = 6,	// How many function select registers
    PIN_BANK_SIZE = 2,				// How many regs to set or clear output
};

/*!
 *  @brief Broadcom GPIO pin functions
 */
enum BCGPIOFunction
{
    BCGPIO_FN_ALT_0  = 2,
    BCGPIO_FN_ALT_1  = 3,
    BCGPIO_FN_ALT_2  = 4,
    BCGPIO_FN_ALT_3  = 5,
    BCGPIO_FN_ALT_4  = 6,
    BCGPIO_FN_ALT_5  = 7,
};


/*
 *  All the GPIO registers are 32 bit, so we shouldn't have any trouble with 
 *  packing and alignment.  The size of this structure should be 41 * 4 bytes.
 */
struct BCM2835GpioRegisters
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

static bool bcgpio_setFunction(BCM2835GpioRegisters* gpioAddress,
                               uint32_t pinNumber,
                               GPIOFunction function);
static GPIOFunction bcgpio_getFunction(BCM2835GpioRegisters* gpio,
                                       uint32_t pinNumber);

static GPIO* driverAllocate(void);
static GPIO* driverInit(GPIO* gpio);
static void driverDealloc(GPIO* gpio);
static bool driverSetFunction(GPIO* gpio,
                              uint32_t pinNumber,
                    		  GPIOFunction function);
static GPIOFunction driverGetFunction(GPIO* gpio, uint32_t pinNumber);
static size_t driverPinCount(GPIO* gpio);
static bool driverSetPin(GPIO* gpio, uint32_t pinNumber, bool pinOn);


static struct GPIODriver bcgpioDriver =
{
    .allocate 		= driverAllocate,
    .init			= driverInit,
    .dealloc  		= driverDealloc,
    .setFunction 	= driverSetFunction,
    .getFunction 	= driverGetFunction,
    .pinCount 		= driverPinCount,
    .setPin 		= driverSetPin,
};

struct BCM2835Gpio
{
    GPIO base;
    BCM2835GpioRegisters* registers;
};

typedef struct BCM2835Gpio BCMGpio;

static BCMGpio theGPIO;

#if defined PIOS_SIMULATOR

BCM2835GpioRegisters* bcgpio_alloc()
{
    BCM2835GpioRegisters* gpio = calloc(1, sizeof(BCM2835GpioRegisters));
    /*
     *  We force the pins high to start
     */
    gpio->lastPinOutputState = 0xFFFFFFFF;
    gpio->pinOutputState = 0xFFFFFFFF;
    return gpio;
}

bool bcgpio_outputPinsHaveChanged(BCM2835GpioRegisters* gpio)
{
    bool ret =  (gpio->pinOutputMask & gpio->pinOutputState)
                != (gpio->pinOutputMask & gpio->lastPinOutputState);
    gpio->lastPinOutputState = gpio->pinOutputState;
    return ret;
}

bool bcgpio_outputPinValue(BCM2835GpioRegisters* gpio, uint32_t pinNumber)
{
    bool ret = true;	// If no output set pull up
    if (bcgpio_getFunction(gpio, pinNumber) == GPIO_FN_OUTPUT)
    {
        ret = (gpio->pinOutputState & (1 << pinNumber)) != 0;
    }
    return ret;
}

#endif

GPIODriver* bcgpio_driver()
{
    return &bcgpioDriver;
}


bool bcgpio_setFunction(BCM2835GpioRegisters* gpioAddress,
       	                uint32_t pinNumber,
                        GPIOFunction function)
{
    bool ret = false;
    
    if (pinNumber < NUM_PINS && function <= BCGPIO_FN_ALT_5)
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

GPIOFunction bcgpio_getFunction(BCM2835GpioRegisters* gpio, uint32_t pinNumber)
{
    GPIOFunction ret = GPIO_FN_INVALID;
    if (pinNumber < NUM_PINS)
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


bool setGPIOPin(BCM2835GpioRegisters* gpioAddress, uint32_t pinNumber, bool pinOn)
{
    bool ret =false;
    
    if (pinNumber < NUM_PINS)
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

#pragma mark -
#pragma mark Driver functions

GPIO* driverAllocate(void)
{
    theGPIO.registers = NULL;
    return (GPIO*) &theGPIO;
}

GPIO* driverInit(GPIO* gpio)
{
    ((BCMGpio*)gpio)->registers = pmm_getGPIOAddress(pmm_getPhysicalMemoryMap());
    return gpio;
}

void driverDealloc(GPIO* gpio)
{
    // Do nothing, the driver is statically allocated.
}

static bool driverSetFunction(GPIO* gpio,
                              uint32_t pinNumber,
                    		  GPIOFunction function)
{
    return bcgpio_setFunction(((BCMGpio*)gpio)->registers, pinNumber, function);
}

static GPIOFunction driverGetFunction(GPIO* gpio, uint32_t pinNumber)
{
    return bcgpio_getFunction(((BCMGpio*)gpio)->registers, pinNumber);
}

static size_t driverPinCount(GPIO* gpio)
{
    return NUM_PINS;
}

bool driverSetPin(GPIO* gpio, uint32_t pinNumber, bool pinOn)
{
    return setGPIOPin(((BCMGpio*)gpio)->registers, pinNumber, pinOn);
}
