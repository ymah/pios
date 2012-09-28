//
//  FrameBuffer.c
//  BakingPi
//
//  Created by Jeremy Pereira on 27/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "FrameBuffer.h"

struct FBPostBox
{
	volatile uint32_t read;
    volatile uint32_t poll;
    volatile uint32_t sender;
    volatile uint32_t status;
    volatile uint32_t configuration;
    volatile uint32_t write;
};

FBPostBox* fb_getFPBostBox()
{
    return (FBPostBox*) 0x2000B880;
}


