//
//  main.c
//  BakingPi
//
//  Created by Jeremy Pereira on 24/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "bptypes.h"
#include "gpio.h"
#include "SystemTimer.h"
#include "PhysicalMemoryMap.h"

#if defined PIOS_SIMULATOR
#define MAIN piosMain
#else
#define MAIN main
#endif

extern const char* kernelArgs[];
const char* kernelArgs[] = { "Pios", NULL };

#if defined OK_EXERCISE
static bool SOSSequence[] =
{
    false, false, false, false,
	true, false, true, false, true, false, false,
	true, true, true, false, true, true, true, false, true, true, true, false, false,
	true, false, true, false, true, false, false
};

#endif

/*!
 *  @brief Run a sequence of LED flashes.
 *  @param iterations how many times to run the sequence.  If negative, we 
 *  don't ever stop.
 *  @param flashLength Length of each flash or non flash in microseconds.
 *  @param sequence A sequence of booleans true = LED on, false = LED off
 *  @param sequenceLength How long is the sequence.
 */
static void runLEDSequence(int iterations,
                           unsigned int flashLength,
                           bool* sequence,
                           size_t sequenceLength);
static void runLEDFlash(int repeat, int numberOfFlashes);

static FBError initFrameBuffer(void);

#if defined OK_EXERCISE

int MAIN(int argc, char** argv)
{
    PhysicalMemoryMap* memoryMap = pmm_getPhysicalMemoryMap();
    GPIO* gpio = pmm_getGPIOAddress(memoryMap);
    SystemTimer* timer = pmm_getSystemTimerAddress(memoryMap);
    gpio_setFunction(gpio, 16, GPIO_FN_OUTPUT);
    setGPIOPin(gpio, 16, false); // Turn on OK to start with as a diagnostic
    st_microsecondSpin(timer, 1000000); // and wait 1 second
    
    runLEDSequence(10, 250000, SOSSequence, sizeof(SOSSequence) / sizeof(bool));
    
    return 0;
}

#else

int MAIN(int argc, char** argv)
{
    PhysicalMemoryMap* memoryMap = pmm_getPhysicalMemoryMap();
    SystemTimer* timer = pmm_getSystemTimerAddress(memoryMap);
    st_microsecondSpin(timer, 200000); // and wait 1 second
    GPIO* gpio = pmm_getGPIOAddress(memoryMap);
    gpio_setFunction(gpio, 16, GPIO_FN_OUTPUT);
    setGPIOPin(gpio, 16, false); // Turn on OK to start with as a diagnostic
    st_microsecondSpin(timer, 500000); // and wait 1 second


    FBError fbError = initFrameBuffer();
    if (fbError != FB_OK)
    {
        runLEDFlash(10, (int) fbError);
    }
    
    return 0;
}

#endif

static FrameBufferDescriptor fbDescriptor =
{
    .width           = 1024,
    .height          =  768,
    .vWidth          = 1024,
    .vHeight         =  768,
    .pitch           =    0,	// To be filled in by GPU
    .bitDepth        =   16,
    .x               =    0,
    .y               =    0,
    .frameBufferPtr  = NULL,	// To be filled in by GPU
    .frameBufferSize =    0		// To be filled in by GPU
};

FBError initFrameBuffer()
{
    FBPostBox* postbox = pmm_getFBPostBox(pmm_getPhysicalMemoryMap());
    FBError ret = fb_getFrameBuffer(postbox, &fbDescriptor);
    return ret;
}

static bool singleFlash[] = { true, false };
static bool gap[] = { false, false };

static void runLEDFlash(int repeat, int numberOfFlashes)
{
    for (int i = 0 ; i < repeat ; ++i)
    {
        runLEDSequence(numberOfFlashes, 250000, singleFlash, sizeof singleFlash / sizeof(bool));
        runLEDSequence(1, 250000, gap, sizeof gap / sizeof(bool));
    }
}

void runLEDSequence(int iterations,
                    unsigned int flashLength,
                    bool* sequence,
                    size_t sequenceLength)
{
    GPIO* gpio = pmm_getGPIOAddress(pmm_getPhysicalMemoryMap());
    SystemTimer* timer = pmm_getSystemTimerAddress(pmm_getPhysicalMemoryMap());
    gpio_setFunction(gpio, 16, GPIO_FN_OUTPUT);

    int iterationsToGo = iterations > 0 ? iterations : 1;
    while (iterationsToGo > 0)
    {
        for (unsigned int i = 0 ; i < sequenceLength ; ++i)
        {
            setGPIOPin(gpio, 16, !sequence[i]);
            st_microsecondSpin(timer, flashLength);
        }
        if (iterations >= 0)
        {
            iterationsToGo--;
        }
    }
}
