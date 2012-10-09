//
//  FrameBuffer.c
//  BakingPi
//
//  Created by Jeremy Pereira on 27/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#if defined PIOS_SIMULATOR
#include <stdlib.h>
#endif

#include "FrameBuffer.h"

enum Bits
{
    STATUS_WRITE_READY_BIT = 31,
    STATUS_READ_READY_BIT  = 30,
    CHANNEL_BITS 		   = 4,
    FB_DESCRIPTOR_ALIGNMENT = 12,
};

enum Masks
{
    STATUS_WRITE_READY_MASK = 1 << STATUS_WRITE_READY_BIT,
    STATUS_READ_READY_MASK  = 1 << STATUS_READ_READY_BIT,
    VALUE_MASK 				= ~0 << CHANNEL_BITS,
    CHANNEL_MASK 			= ~VALUE_MASK,
    FB_DESCRIPTOR_ALIGNMENT_MASK = ~(~0 << FB_DESCRIPTOR_ALIGNMENT)
};

struct FBPostBox
{
	volatile uint32_t read;
    volatile uint32_t poll;
    volatile uint32_t sender;
    volatile uint32_t status;
    volatile uint32_t configuration;
    volatile uint32_t write;
};

enum FrameBufferConsts
{
    MAX_PIXEL_WIDTH  = 4096,
    MAX_PIXEL_HEIGHT = 4096,
    MAX_BIT_DEPTH    =   32,
};

#if defined PIOS_SIMULATOR

FBPostBox* fb_postBoxAlloc()
{
    return calloc(1, sizeof(FBPostBox));
}

#endif

bool fb_send(FBPostBox* postbox, uint32_t message, uint32_t channel)
{
    bool ret =false;
    if ((message & 0xF) == 0 && channel < FB_POSTBOX_CHANNELS)
    {
        while ((postbox->status & STATUS_WRITE_READY_MASK) != 0)
        {
            // spin
        }
        postbox->write = message | channel;
    }
    return ret;
}

uint32_t fb_read(FBPostBox* postbox, uint32_t channel)
{
    uint32_t ret = -1;
    if (channel < FB_POSTBOX_CHANNELS)
    {
        uint32_t readValue = 0xFFFFFFFF;
        uint32_t readChannel = 0x7FFFFFFF; // Requested channel will never be this
        while (readChannel != channel)
        {
            while ((postbox->status & STATUS_READ_READY_MASK) != 0)
            {
                // spin
            }
            readValue = postbox->read;
            readChannel = readValue & CHANNEL_MASK;
        }
        ret = readValue & VALUE_MASK;
    }
    return ret;
}

FBError
fb_getFrameBuffer(FBPostBox* postbox, FrameBufferDescriptor* fbDescriptor)
{
    FBError ret = FB_OK;
    if (fbDescriptor->width >= MAX_PIXEL_WIDTH
        || fbDescriptor->height >= MAX_PIXEL_HEIGHT
        || fbDescriptor->bitDepth >= MAX_BIT_DEPTH)
    {
        ret = FB_PARAMETER;
    }
    else if ((((uint32_t)fbDescriptor) & FB_DESCRIPTOR_ALIGNMENT_MASK) != 0)
    {
        ret = FB_ALIGNMENT;
    }
    return ret;
}

