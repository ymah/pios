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

#include "FrameBuffer+Driver.h"
#include "PhysicalMemoryMap.h"
#include "klib.h"

static FrameBuffer* screenFrameBuffer = NULL;


FBError fb_initialiseFrameBuffer(FrameBuffer* frameBuffer,
                                 FBRequestDimensions* fbDimensions)
{
    frameBuffer->dimensions = *fbDimensions;
    klib_memset(&frameBuffer->buffer, 0, sizeof frameBuffer->buffer);
    return frameBuffer->driver->initialiseTheBuffer(frameBuffer);
}

FBError fb_getDimensions(FrameBuffer* frameBuffer,
                         FBRequestDimensions* dimensions,
                         FBBuffer* bufferDescriptor)
{
    FBError ret = FB_NOT_INITIALISED;
    
    if (frameBuffer->buffer.frameBufferPtr != NULL)
    {
        ret = FB_OK;
        if (dimensions != NULL)
        {
            *dimensions = frameBuffer->dimensions;
        }
        if (bufferDescriptor != NULL)
        {
            *bufferDescriptor = frameBuffer->buffer;
        }
    }
    return ret;
}

FrameBuffer* fb_getFrameBuffer(FBDriver* driver)
{
    FrameBuffer* ret = driver->allocate();
    ret->driver = driver;
    return ret;
}

FrameBuffer* fb_getScreenFrameBuffer()
{
    return screenFrameBuffer;
}

void fb_setScreenFrameBuffer(FrameBuffer* frameBuffer)
{
    screenFrameBuffer = frameBuffer;
}

void fb_forceUpdate(FrameBuffer* frameBuffer)
{
    if (frameBuffer->driver->forceUpdate != NULL)
    {
        frameBuffer->driver->forceUpdate(frameBuffer);
    }
}
