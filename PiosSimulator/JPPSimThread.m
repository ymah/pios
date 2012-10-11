//
//  JPPHardwareThread.m
//  BakingPi
//
//  Created by Jeremy Pereira on 01/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import "JPPSimThread.h"

#import "PhysicalMemoryMap.h"
#import "SystemTimer.h"
#import <time.h>
#import <assert.h>

@interface JPPSimThread ()

-(void) notifyFinished;
-(void) notifyStarted;
-(void) notifyUpdate;

@end


@implementation JPPSimThread

-(id) init
{
    self = [super init];
    if (self != nil)
    {
        [self setName: [self className]];
    }
    return self;
}


-(void) main
{
    @autoreleasepool
    {
        pmm_init();
        @try
        {
            [self notifyStarted];
            
            [self simThreadMain];
        }
        @catch (NSException* exception)
        {
            NSLog(@"Thread threw exception %@", exception);
        }
        @finally
        {
            [self notifyFinished];
        }
    }
}

-(void) notifyStarted
{
    [[self delegate] hasStarted: self];
}

-(void) notifyFinished
{
    [[self delegate] hasFinished: self];
}


-(void) notifyUpdate
{
    [[self delegate] hasBeenUpdated: self];
}


-(void) simThreadMain
{
    // Do nothing
}


@end

@interface JPPHardwareThread ()

-(void) checkFrameBufferPostbox;

@end

@implementation JPPHardwareThread
{
    uint8_t* frameBuffer;
    PhysicalMemoryMap* memoryMap;
    FBPostBox* frameBufferPostbox;
    FrameBufferDescriptor* fbDescriptor;
}

-(void) simThreadMain
{
    uint64_t iterations = 0;
    memoryMap = pmm_getPhysicalMemoryMap();
    frameBufferPostbox = pmm_getFBPostBox(memoryMap);
    clock_t lastClock = clock();
    while (![self isCancelled])
    {
        [self checkFrameBufferPostbox];
        if (clock() > lastClock)
        {
            lastClock = clock();
            st_microsecondTick(pmm_getSystemTimerAddress(memoryMap));
            if (gpio_outputPinsHaveChanged(pmm_getGPIOAddress(memoryMap)))
            {
                [self notifyUpdate];
            }
            
        }
        iterations++;
    }
}

-(void) checkFrameBufferPostbox
{
    uintptr_t message = -1;
    if (fb_postBoxWasWritten(frameBufferPostbox, &message))
    {
        int channel = message & FB_CHANNEL_MASK;
        if (channel == PB_FRAME_BUFFER_CHANNEL)
        {
            //           fbDescriptor = (FrameBufferDescriptor*)(message & FB_VALUE_MASK);
            fbDescriptor = (FrameBufferDescriptor*)message;
            if (frameBuffer != NULL)
            {
                free(frameBuffer);
                frameBuffer = NULL;
            }
            // TODO:  this may not be correct for 24 bit RGB
            size_t bytesWide
            	= ((size_t)(fbDescriptor->vWidth) * fbDescriptor->bitDepth + 7) / 8;
            size_t bytesNeeded = bytesWide * fbDescriptor->vHeight;
            frameBuffer = calloc(bytesNeeded, sizeof(uint8_t));
            assert(((uintptr_t)frameBuffer & FB_CHANNEL_MASK) == 0);
            fbDescriptor->pitch = (uint32_t)bytesWide;
            fbDescriptor->frameBufferSize = (uint32_t)bytesNeeded;
            fbDescriptor->frameBufferPtr = frameBuffer;
            while (!fb_tryMakeRead(frameBufferPostbox,
                                   PB_FRAME_BUFFER_CHANNEL,
                                   message)
                   && ![self isCancelled])
            {
                // spin
            }
        }
    }
}

@end

int piosMain(int argc, char** argv);

static char* simulatorArgs[] = { "simulator", NULL };

@implementation JPPSoftwareThread

-(void) simThreadMain
{
    piosMain(1, simulatorArgs);
}

-(void) cancel
{
    [super cancel];
    pmm_setStopFlag(pmm_getPhysicalMemoryMap(), true);
}
@end
