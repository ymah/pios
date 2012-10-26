//
//  main.c
//  BakingPi
//
//  Created by Jeremy Pereira on 24/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#if defined PIOS_SIMULATOR
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#endif

#include "bptypes.h"
#include "gpio.h"
#include "SystemTimer.h"
#include "PhysicalMemoryMap.h"
#include "klib.h"
#include "gdi.h"
#include "console.h"

/*
 *  The simulator is a Mac OS X application and, as such, already has a main
 *  function.  So when we compile for the simulator we call it something else.
 */
#if defined PIOS_SIMULATOR
#define MAIN piosMain
#else
#define MAIN main
#endif

// TODO: Relace these args with the args passed in by the boot loader
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

static FrameBufferDescriptor* alignedDescriptor = NULL;


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
/*
 *  Following functions not declared static to suppress not used warnings if
 *  their use is commented out.
 */
void runRainbow(void);
void runDrawTest(void);
void displayTags(void);

static void printTag(Tag* tagToPrint);

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
    pmm_initialiseFreePages(memoryMap);
    SystemTimer* timer = pmm_getSystemTimerAddress(memoryMap);
    st_microsecondSpin(timer, 200000); // and wait 1 second
    GPIO* gpio = pmm_getGPIOAddress(memoryMap);
    gpio_setFunction(gpio, 16, GPIO_FN_OUTPUT);
    setGPIOPin(gpio, 16, false); // Turn on OK to start with as a diagnostic
    st_microsecondSpin(timer, 1000000); // and wait 1 second


    setGPIOPin(gpio, 16, true); // Turn off OK while getting frame buffer
    FBError fbError = initFrameBuffer();
    GDIContext* context = gdi_initialiseGDI(alignedDescriptor);
    gdi_setColour(context, GDI_BACKGROUND, GDI_BLACK_COLOUR);
    gdi_setColour(context, GDI_PEN, GDI_WHITE_COLOUR);
    gdi_setColour(context, GDI_FILL, GDI_WHITE_COLOUR);
    con_initialiseConsole(context);
    setGPIOPin(gpio, 16, false); // Turn on OK to start with as a diagnostic

    if (fbError == FB_OK)
    {
#if defined SCREEN_01
        runRainbow();
#elif defined SCREEN_02
        runDrawTest();
#else
        displayTags();
#endif
    }
    else
    {
        runLEDFlash(10, (int) fbError);
    }
    
    return 0;
}

#endif

FBError initFrameBuffer()
{
    PhysicalMemoryMap* memoryMap = pmm_getPhysicalMemoryMap();
    FBPostBox* postbox = pmm_getFBPostBox(memoryMap);
    alignedDescriptor = pmm_allocatePage(memoryMap);
    klib_memcpy(alignedDescriptor, &fbDescriptor, sizeof fbDescriptor);
    FBError ret = fb_getFrameBuffer(postbox, alignedDescriptor);
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

void runRainbow()
{
    PhysicalMemoryMap* memoryMap = pmm_getPhysicalMemoryMap();
    uint16_t colour = 0;
	while (!pmm_getStopFlag(memoryMap))
    {
        uint16_t* pixelPtr = alignedDescriptor->frameBufferPtr;
        for (size_t y = 0; y < alignedDescriptor->height; ++y)
        {
            for (size_t x = 0 ; x < alignedDescriptor->width ; ++x)
            {
                *pixelPtr++ = colour;
            }
            colour++;
        }
    }
}

void runDrawTest(void)
{
    GDIContext* context = gdi_initialiseGDI(alignedDescriptor);
    gdi_setColour(context, GDI_BACKGROUND, GDI_BLACK_COLOUR);
    gdi_setColour(context, GDI_PEN, GDI_WHITE_COLOUR);
    gdi_fillFrame(context, GDI_BACKGROUND);
    GDIRect frame = gdi_frame(context);
    size_t maxPixel = MIN(frame.size.width, frame.size.height);
    for (int i = 0 ; i < maxPixel ; ++i)
    {
        GDIPoint point = { .x = i, .y = i };
        gdi_setPixel(context, point, GDI_PEN);
    }
    /*
     *  Now let's draw a border around the screen
     */
    GDIRect border = frame;
    border.size.height--;
    border.size.width--;
    GDIPoint borderPoint = { .x = border.origin.x, .y = border.origin.y };
    gdi_setColour(context, GDI_PEN, gdi_makeColour(255, 0, 0, 255));
	/*
     *  Top of the border rectangle
     */
	for (size_t i = 0 ; i < border.size.width ; ++i)
    {
        gdi_setPixel(context, borderPoint, GDI_PEN);
        borderPoint.x++;
    }
    /*
     *  Right edge of the border rectangle
     */
    for (size_t i = 0 ; i < border.size.height ; ++i)
    {
        gdi_setPixel(context, borderPoint, GDI_PEN);
		borderPoint.y++;
    }
    /*
     *  Bottom edge
     */
    for (size_t i = 0 ; i < border.size.width ; ++i)
    {
        gdi_setPixel(context, borderPoint, GDI_PEN);
        borderPoint.x--;
    }
    /*
     *  Left edge
     */
    for (size_t i = 0 ; i < border.size.height ; ++i)
    {
        gdi_setPixel(context, borderPoint, GDI_PEN);
		borderPoint.y--;
    }
    /*
     *  Test GI line
     */
    GDIPoint p0 = { .x = 10, .y = 12 };
    GDIPoint p1 = { .x = 200, .y = 40 };
    gdi_line(context, p0, p1);
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

void displayTags()
{
    PhysicalMemoryMap* memoryMap = pmm_getPhysicalMemoryMap();
    TagList* tagList = pmm_getTagList(memoryMap);
    /*
     *  Iterate athrough and print the tags
     */
    Tag* tag = tag_getFirstTag(tagList);
    while (tag_type(tag) != TAG_TERMINATOR)
    {
        printTag(tag);
        tag = tag_getNextTag(tagList, tag);
    }
}

static void printTag(Tag* tagToPrint)
{
    size_t tagLength = tag_length(tagToPrint);
    uint32_t tagType = tag_type(tagToPrint);
    tagToPrint++;
    Console* console = con_getTheConsole();
    con_putHex32(console, tagType);
    con_putChars(console, " ", 1);
    con_putHex32(console, (uint32_t) tagLength);
    con_putChars(console, " ", 1);
    switch (tagType)
    {
        case TAG_CORE:
            {
                TagCore* core = (TagCore*) tagToPrint;
                con_putCString(console, "flags=");
                con_putHex32(console, core->flags);
                con_putCString(console, ", page size=");
                con_putHex32(console, core->pageSize);
                con_putCString(console, ", root dev=");
                con_putHex32(console, core->rootDeviceNo);
            }
            break;
        case TAG_MEM:
            {
                TagMem* mem = (TagMem*) tagToPrint;
                con_putCString(console, "size=");
                con_putHex32(console, (uint32_t)mem->size);
                con_putCString(console, ", start=");
                con_putHex32(console, (uint32_t)mem->start);                
            }
            break;
        case TAG_CMDLINE:
            {
                const char* commandLine = (const char*) tagToPrint;
                con_putChars(console,
                             commandLine,
                             klib_strnlen(commandLine,
                                          tagLength * sizeof(uint32_t)));
            }
            break;
            
        default:
            con_putChars(console, "Don't understand this tag", 25);
            break;
    }
    con_newLine(console);
#if defined PIOS_SIMULATOR
    // TODO: Print to the screen not stdout
    fprintf(stdout, "type %d, length %d\n", (int) tagType, (int) tagLength);
#endif
}

