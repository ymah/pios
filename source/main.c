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
    return 0;
}
