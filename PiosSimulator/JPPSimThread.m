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
#import "JPPPiSimulator.h"

/*
 *  We assume a screen refresh of 30Hz at 1000 lines.  We won't send an update
 *  for every single line but we group them together to try to avoid flooding
 *  the main thread.
 */
enum ScreenScanConsts
{
    SCREEN_LINES_PER_UPDATE = 10,
    SCREEN_REFRESH_HZ 		= 30,
    SCREEN_NOMINAL_LINES	= 1000,
    SCREEN_UPDATE_CLOCKS 	= CLOCKS_PER_SEC
                              / (SCREEN_REFRESH_HZ
                                 * (SCREEN_NOMINAL_LINES / SCREEN_LINES_PER_UPDATE)),
};

enum ColourBits
{
    RED_16_MASK			= (0xFFFF << FBPF_RED_16_BIT_POS) & 0xFFFF,
    GREEN_16_MASK		= ((0xFFFF << FBPF_GREEN_16_BIT_POS) & ~RED_16_MASK) & 0xFFFF,
    BLUE_16_MASK		= (((0xFFFF << FBPF_BLUE_16_BIT_POS) & ~RED_16_MASK)
    					  	& ~GREEN_16_MASK)
                          & 0xFFFF,
    
    RED_32_MASK			= (0xFFFFFFFF << FBPF_RED_32_BIT_POS) & 0xFFFFFFFF,
    GREEN_32_MASK		= ((0xFFFFFFFF << FBPF_GREEN_32_BIT_POS) & ~RED_32_MASK) & 0xFFFFFFFF,
    BLUE_32_MASK		= (((0xFFFFFFFF << FBPF_BLUE_32_BIT_POS) & ~RED_32_MASK)
    						& ~GREEN_16_MASK)
                          & 0xFFFFFFFF,
    ALPHA_32_MASK		= ((((0xFFFFFFFF << FBPF_ALPHA_32_BIT_POS) & ~RED_32_MASK)
                           		& ~GREEN_16_MASK)
                           	& ~BLUE_32_MASK)
    					& 0xFFFFFFFF,

};



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
-(void) refreshTheScreen: (clock_t) microSeconds;

@end

@implementation JPPHardwareThread
{
    uint8_t* frameBuffer;
    PhysicalMemoryMap* memoryMap;
    FBPostBox* frameBufferPostbox;
    FrameBufferDescriptor* fbDescriptor;
    CGContextRef screenBitmap;
    
    NSRange scanLinesToUpdate;
    bool scanLinesNeedReading;
}

-(void) dealloc
{
    if (screenBitmap != NULL)
    {
        CGContextRelease(screenBitmap);
    }
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
        clock_t now = clock();
        if (now > lastClock)
        {
            st_microsecondTick(pmm_getSystemTimerAddress(memoryMap));
            if (gpio_outputPinsHaveChanged(pmm_getGPIOAddress(memoryMap)))
            {
                [self notifyUpdate];
            }
            if ((now - lastClock) > SCREEN_UPDATE_CLOCKS)
            {
                [self refreshTheScreen: now - lastClock];
            }
            lastClock = now;
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
            uintptr_t pointerAsInt = message - channel;
            fbDescriptor = (FrameBufferDescriptor*)pointerAsInt;
            if (frameBuffer != NULL)
            {
                free(frameBuffer);
                frameBuffer = NULL;
            }
            if (screenBitmap != NULL)
            {
                CGContextRelease(screenBitmap);
            }
            /*
             *  Create the bitmap that the software thread can write to
             */
            // TODO:  this may not be correct for 24 bit RGB
            size_t bytesWide
            	= ((size_t)(fbDescriptor->width) * fbDescriptor->bitDepth + 7) / 8;
            size_t bytesNeeded = bytesWide * fbDescriptor->height;
            frameBuffer = calloc(bytesNeeded, sizeof(uint8_t));
            assert(((uintptr_t)frameBuffer & FB_CHANNEL_MASK) == 0);
            /*
             *  Fill in the frame buffer descriptor and notify the software
             *  thread.
             */
            fbDescriptor->pitch = (uint32_t)bytesWide;
            fbDescriptor->frameBufferSize = (uint32_t)bytesNeeded;
            fbDescriptor->frameBufferPtr = frameBuffer;
            while (!fb_tryMakeRead(frameBufferPostbox,
                                   PB_FRAME_BUFFER_CHANNEL,
                                   0)
                   && ![self isCancelled])
            {
                // spin
            }
            NSLog(@"Screen started, updates every %d microseconds", (int) SCREEN_UPDATE_CLOCKS);
			[self notifyUpdate];
        }
    }
}

-(void) refreshTheScreen: (clock_t) microseconds
{
    if (fbDescriptor != NULL && fbDescriptor->frameBufferPtr != NULL)
    {
        @synchronized(self)
        {
            size_t numberOfScanLines = SCREEN_LINES_PER_UPDATE;
            if (scanLinesNeedReading)
            {
                numberOfScanLines += scanLinesToUpdate.length;
            }
            else
            {
                scanLinesToUpdate.location += scanLinesToUpdate.length;
            }
            if (scanLinesToUpdate.location >= fbDescriptor->height)
            {
                scanLinesToUpdate.location = 0;
            }
            scanLinesToUpdate.length = MIN(numberOfScanLines,
                                           fbDescriptor->height - scanLinesToUpdate.location);
            scanLinesNeedReading = true;
        }
        [self notifyUpdate];
    }
}

-(bool) scanLinesUpdated: (NSRange*) linesToUpdateRef
{
    bool ret = false;
    @synchronized(self)
    {
        *linesToUpdateRef = scanLinesToUpdate;
        ret = scanLinesNeedReading;
        scanLinesNeedReading = false;
    }
    return ret;
}


-(bool) getScanLine: (const uint8_t**) scanLinePtr
		 pixelBytes: (uint8_t*) pixelBytes
       colourDepths: (uint8_t*) colourDepths
          lineBytes: (size_t*) lineBytes
      scanLineIndex: (size_t) scanLineIndex
{
    bool ret = false;
    if (fbDescriptor != NULL
        && fbDescriptor->frameBufferPtr != NULL
        && scanLineIndex < fbDescriptor->height)
    {
        size_t myPixelBytes = fbDescriptor->bitDepth / 8;
        size_t myLineBytes = fbDescriptor->width * myPixelBytes;
        ret = true;
        if (pixelBytes != NULL)
        {
            *pixelBytes = myPixelBytes; // Assume byte is 8 bits
        }
        if (colourDepths != NULL)
        {
            switch (fbDescriptor->bitDepth)
            {
                case 16:
                    colourDepths[CDI_ALPHA] = FBPF_ALPHA_16_BITS;
                    colourDepths[CDI_BLUE]  = FBPF_BLUE_16_BITS;
                    colourDepths[CDI_GREEN] = FBPF_GREEN_16_BITS;
                    colourDepths[CDI_RED] 	= FBPF_RED_16_BITS;
                    break;
                case 32:
                    colourDepths[CDI_ALPHA] = FBPF_ALPHA_32_BITS;
                    colourDepths[CDI_BLUE]  = FBPF_BLUE_32_BITS;
                    colourDepths[CDI_GREEN] = FBPF_GREEN_32_BITS;
                    colourDepths[CDI_RED] 	= FBPF_RED_32_BITS;
                    break;
                default:
                    ret = false;
                    break;
            }
        }
        if (lineBytes != NULL)
        {
            *lineBytes = myLineBytes;
        }
        if (scanLinePtr != NULL)
        {
            *scanLinePtr = (uint8_t*) fbDescriptor->frameBufferPtr
            			 + (myLineBytes * scanLineIndex);
        }
    }
    return ret;
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
