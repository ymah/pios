//
//  FrameBuffer.c
//  BakingPi
//
//  Created by Jeremy Pereira on 27/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#if defined PIOS_SIMULATOR
#include <stdlib.h>
#include <libkern/OSAtomic.h>
#include <stdio.h>
#endif

#include "FrameBuffer.h"
#include "PhysicalMemoryMap.h"

enum
{
    
    STATUS_WRITE_READY_BIT = 31,
    STATUS_READ_READY_BIT  = 30,
};

enum Masks
{
    STATUS_WRITE_READY_MASK = 1 << STATUS_WRITE_READY_BIT,
    STATUS_READ_READY_MASK  = 1 << STATUS_READ_READY_BIT,
};


struct FBPostBox
{
	volatile uintptr_t read;
    uint32_t padding[3];			// This caught me out from Baking Pi
    volatile uint32_t  poll;
    volatile uint32_t  sender;
    volatile uint32_t  status;
    volatile uint32_t  configuration;
    volatile uintptr_t write;
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
    FBPostBox* ret = calloc(1, sizeof(FBPostBox));
    /*
     *  There is nothing to read at the moment, obviously
     */
    ret->status = STATUS_READ_READY_MASK;
    
    return ret;
}

bool fb_postBoxWasWritten(FBPostBox* postbox, uintptr_t* messageRef)
{
    bool ret = false;
    if ((postbox->status & STATUS_WRITE_READY_MASK) != 0)
    {
        if (messageRef != NULL)
        {
            *messageRef = postbox->write;
        }
        postbox->status &= ~STATUS_WRITE_READY_MASK;
        ret = true;
    }
    return ret;
}

bool fb_tryMakeRead(FBPostBox* postbox, uint32_t channel, uintptr_t message)
{
    bool ret = false;
    if ((postbox->status & STATUS_READ_READY_MASK) != 0)
    {
        postbox->read = (message & FB_VALUE_MASK) | (channel & FB_CHANNEL_MASK);
        OSAtomicAnd32Barrier(~STATUS_READ_READY_MASK, &postbox->status);
        ret = true;
    }
    return ret;
}

#endif

bool fb_send(FBPostBox* postbox, uintptr_t message, uint32_t channel)
{
    PhysicalMemoryMap* memoryMap = pmm_getPhysicalMemoryMap();
    bool ret =false;
    if ((message & 0xF) == 0 && channel < FB_POSTBOX_CHANNELS)
    {
        while ((postbox->status & STATUS_WRITE_READY_MASK) != 0
               && !pmm_getStopFlag(memoryMap))
        {
            // spin
        }
        postbox->write = message | channel;
        ret = true;
#if defined PIOS_SIMULATOR
        fprintf(stderr,
                "Before change status: mask 0x%08x, value 0x%08x\n",
                (unsigned int)STATUS_WRITE_READY_MASK,
                (unsigned int) postbox->status);
        OSAtomicOr32Barrier(STATUS_WRITE_READY_MASK, &postbox->status);
        fprintf(stderr,
                "After change status: mask 0x%08x, value 0x%08x\n",
                (unsigned int)STATUS_WRITE_READY_MASK,
                (unsigned int) postbox->status);
#endif
    }
    return ret;
}

uintptr_t fb_read(FBPostBox* postbox, uint32_t channel)
{
    PhysicalMemoryMap* memoryMap = pmm_getPhysicalMemoryMap();
    uintptr_t ret = -1;
    if (channel < FB_POSTBOX_CHANNELS)
    {
        uintptr_t readValue = (uintptr_t)-1;
        uint32_t readChannel = 0x7FFFFFFF; // Requested channel will never be this
        while (readChannel != channel && !pmm_getStopFlag(memoryMap))
        {
            while ((postbox->status & STATUS_READ_READY_MASK) != 0
                   && !pmm_getStopFlag(memoryMap))
            {
                // spin
            }
            readValue = postbox->read;
            readChannel = readValue & FB_CHANNEL_MASK;
#if defined PIOS_SIMULATOR
            postbox->status |= STATUS_READ_READY_MASK;
#endif
        }
        ret = readValue & FB_VALUE_MASK;
    }
    return ret;
}

FBError
fb_getFrameBuffer(FBPostBox* postbox, FrameBufferDescriptor* fbDescriptor)
{
    PhysicalMemoryMap* memoryMap = pmm_getPhysicalMemoryMap();
    GPIO* gpio = pmm_getGPIOAddress(memoryMap);

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
    else
    {
        if (!fb_send(postbox, (uintptr_t)fbDescriptor, PB_FRAME_BUFFER_CHANNEL))
        {
            ret = FB_PARAMETER;
        }
        else
        {
            uintptr_t readResult = fb_read(postbox, PB_FRAME_BUFFER_CHANNEL);
            setGPIOPin(gpio, 16, false);
            if (readResult == 0)
            {
                while (fbDescriptor->frameBufferPtr == NULL
                       && !pmm_getStopFlag(memoryMap))
                {
                    // spin.  Apparently if we have no pointer, we can just
                    // wait.
                }
            }
            else
            {
                ret = FB_FAILED_GET;
            }
        }
    }
    if (pmm_getStopFlag(memoryMap))
    {
        ret = FB_STOPPED;
    }
    return ret;
}

