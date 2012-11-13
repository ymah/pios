//
//  VideoCore.c
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

#include "klib.h"
#include "FrameBuffer+Driver.h"
#include "VideoCore.h"
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


struct VCPostBox
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

static FrameBuffer* allocate(void);
static void dealloc(FrameBuffer* fb);
static FBError initialiseTheBuffer(FrameBuffer* frameBuffer);

FBDriver theVCDriver =
{
    .allocate = allocate,
    .dealloc = dealloc,
    .initialiseTheBuffer = initialiseTheBuffer,
    .forceUpdate = NULL,
};

struct VCFrameBuffer
{
    FrameBuffer base;
    VCPostBox* postbox;
    VCFrameBufferDescriptor* descriptor;
};

typedef struct VCFrameBuffer VCFrameBuffer;

static VCFrameBuffer theOneAndOnlyVCFrameBuffer;

#if defined PIOS_SIMULATOR

VCPostBox* vc_postBoxAlloc()
{
    VCPostBox* ret = calloc(1, sizeof(VCPostBox));
    /*
     *  There is nothing to read at the moment, obviously
     */
    ret->status = STATUS_READ_READY_MASK;
    
    return ret;
}

bool vc_postBoxWasWritten(VCPostBox* postbox, uintptr_t* messageRef)
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

bool vc_tryMakeRead(VCPostBox* postbox, uint32_t channel, uintptr_t message)
{
    bool ret = false;
    if ((postbox->status & STATUS_READ_READY_MASK) != 0)
    {
        postbox->read = (message & VC_VALUE_MASK) | (channel & VC_CHANNEL_MASK);
        OSAtomicAnd32Barrier(~STATUS_READ_READY_MASK, &postbox->status);
        ret = true;
    }
    return ret;
}

#endif

FBDriver* vc_driver()
{
    return &theVCDriver;
}

bool vc_send(VCPostBox* postbox, uintptr_t message, uint32_t channel)
{
    PhysicalMemoryMap* memoryMap = pmm_getPhysicalMemoryMap();
    bool ret =false;
    if ((message & 0xF) == 0 && channel < VC_POSTBOX_CHANNELS)
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

uintptr_t vc_read(VCPostBox* postbox, uint32_t channel)
{
    PhysicalMemoryMap* memoryMap = pmm_getPhysicalMemoryMap();
    uintptr_t ret = -1;
    if (channel < VC_POSTBOX_CHANNELS)
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
            readChannel = readValue & VC_CHANNEL_MASK;
#if defined PIOS_SIMULATOR
            postbox->status |= STATUS_READ_READY_MASK;
#endif
        }
        ret = readValue & VC_VALUE_MASK;
    }
    return ret;
}

FBError
vc_getFrameBuffer(VCPostBox* postbox, VCFrameBufferDescriptor* fbDescriptor)
{
    PhysicalMemoryMap* memoryMap = pmm_getPhysicalMemoryMap();
    GPIO* gpio = pmm_getGPIOAddress(memoryMap);
    
    uintptr_t messageToSend = (uintptr_t)fbDescriptor;

    FBError ret = FB_OK;
    if (fbDescriptor->width > MAX_PIXEL_WIDTH
        || fbDescriptor->height > MAX_PIXEL_HEIGHT
        || fbDescriptor->bitDepth > MAX_BIT_DEPTH)
    {
        ret = FB_PARAMETER;
    }
    else if ((messageToSend & VC_DESCRIPTOR_ALIGNMENT_MASK) != 0)
    {
        ret = FB_ALIGNMENT;
    }
    else
    {
#if !defined PIOS_SIMULATOR
        /*
         *  This forces the Videocore to use the cache coherent alias for the
         *  SDRAM.  Hopefully, that means we'll recognise straight away when
         *  the pointer arrives.
         */
        messageToSend += 0x40000000;
#endif
        
        if (!vc_send(postbox, messageToSend, VC_FRAME_BUFFER_CHANNEL))
        {
            ret = FB_PARAMETER;
        }
        else
        {
            uintptr_t readResult = vc_read(postbox, VC_FRAME_BUFFER_CHANNEL);
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

#pragma mark -
#pragma mark Driver functions

static FrameBuffer* allocate()
{
    klib_memset(&theOneAndOnlyVCFrameBuffer,
                0,
                sizeof theOneAndOnlyVCFrameBuffer);
    return (FrameBuffer*)&theOneAndOnlyVCFrameBuffer;
}

static void dealloc(FrameBuffer* fb)
{
    // Do nothing - statically allocated
}

static FBError initialiseTheBuffer(FrameBuffer* frameBuffer)
{
    FBError error = FB_PARAMETER;
    /*
     *  Weonly support 16 and 32 bit depths for now
     */
    if (frameBuffer->dimensions.bitDepth == 16
        || frameBuffer->dimensions.bitDepth == 32)
    {
        PhysicalMemoryMap* map = pmm_getPhysicalMemoryMap();
        VCFrameBufferDescriptor* theDescriptor = pmm_allocatePage(map);
        VCPostBox* thePostbox = pmm_getVCPostBox(map);
        theDescriptor->width = frameBuffer->dimensions.width;
        theDescriptor->vWidth = frameBuffer->dimensions.width;
        theDescriptor->height = frameBuffer->dimensions.height;
        theDescriptor->vHeight = frameBuffer->dimensions.height;
        theDescriptor->x = frameBuffer->dimensions.x;
        theDescriptor->y = frameBuffer->dimensions.y;
        theDescriptor->bitDepth = frameBuffer->dimensions.bitDepth;
        error = vc_getFrameBuffer(thePostbox, theDescriptor);
        if (error == FB_OK)
        {
            frameBuffer->buffer.frameBufferPtr = theDescriptor->frameBufferPtr;
            frameBuffer->buffer.frameBufferSize = theDescriptor->frameBufferSize;
            frameBuffer->buffer.rasterWidth = theDescriptor->pitch;
            switch (frameBuffer->dimensions.bitDepth)
            {
                case 16:
                    frameBuffer->buffer.colourDepths[0] = 5;
                    frameBuffer->buffer.colourDepths[1] = 6;
                    frameBuffer->buffer.colourDepths[2] = 5;
                    frameBuffer->buffer.colourDepths[3] = 0;
                    break;
                    
                default:
                    frameBuffer->buffer.colourDepths[0] = 8;
                    frameBuffer->buffer.colourDepths[1] = 8;
                    frameBuffer->buffer.colourDepths[2] = 8;
                    frameBuffer->buffer.colourDepths[3] = 8;
                    break;
            }
            VCFrameBuffer* vcFrameBuffer = (VCFrameBuffer*) frameBuffer;
            vcFrameBuffer->postbox = thePostbox;
            vcFrameBuffer->descriptor = theDescriptor;
        }
        
    }
    return error;
}

