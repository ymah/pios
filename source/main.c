//
//  main.c
//  BakingPi
//
//  Created by Jeremy Pereira on 24/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "bptypes.h"
#include "gpio.h"
#include "SystemTimer.h"

extern const char* kernelArgs[];
const char* kernelArgs[] = { "BakeOS", NULL };

int main(int argc, char** argv)
{
    GPIO* gpio = getGPIOAddress();
    SystemTimer* timer = getSystemTimerAddress();
    setGPIOFunction(gpio, 16, GPIO_FN_OUTPUT);
    setGPIOPin(gpio, 16, false);
    while (true)
    {
        st_microsecondSpin(timer, 500000);
        setGPIOPin(gpio, 16, true);
        st_microsecondSpin(timer, 500000);
        setGPIOPin(gpio, 16, false);
    }
    return 0;
}
