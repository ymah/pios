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
	unsigned uint32_t read;
    unsigned uint32_t poll;
    unsigned uint32_t sender;
    unsigned uint32_t status;
    unsigned uint32_t configuration;
    unsigned uint32_t write;
};

FBPostBox* fb_getFPBostBox()
{
    return (FBPostBox*) 0x2000B880;
}


