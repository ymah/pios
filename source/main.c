//
//  main.c
//  BakingPi
//
//  Created by Jeremy Pereira on 24/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "bptypes.h"
#include "gpio.h"

extern const char* kernelArgs[];
const char* kernelArgs[] = { "BakeOS", NULL };

int main(int argc, char** argv)
{
    setGPIOFunction(16, GPIO_FN_OUTPUT);
    while (true)
    {
        volatile unsigned int countDown;
        setGPIOPin(16, true);
        for (countDown = 0 ; countDown < 0x3F0000 ; countDown++);
        setGPIOPin(16, false);
        for (countDown = 0 ; countDown < 0x3F0000 ; countDown++);
    }
    return 0;
}
