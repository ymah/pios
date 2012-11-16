//
//  bcm2835gpio_h
//  BakingPi
//
//  Created by Jeremy Pereira on 17/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_bcm2835gpio_h
#define BakingPi_bcm2835gpio_h

#include "bptypes.h"
#include "gpio.h"

struct BCM2835GpioRegisters;
typedef struct BCM2835GpioRegisters BCM2835GpioRegisters;

#if defined PIOS_SIMULATOR

BCM2835GpioRegisters* bcgpio_alloc();

bool bcgpio_outputPinsHaveChanged(BCM2835GpioRegisters* gpio);

bool bcgpio_outputPinValue(BCM2835GpioRegisters* gpio, uint32_t pinNumber);

#endif

/*!
 *  @brief Get the GPIO driver for the Broadcom GPIO
 *  @return BC2835 driver
 */
GPIODriver* bcgpio_driver();

#endif
