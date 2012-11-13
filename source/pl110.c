//
//  pl110.c
//  BakingPi
//
//  Created by Jeremy Pereira on 05/11/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "pl110.h"
#include "FrameBuffer+Driver.h"
#include "klib.h"
#include "PhysicalMemoryMap.h"

static FrameBuffer* allocate(void);
static void dealloc(FrameBuffer* fb);
static FBError initialiseTheBuffer(FrameBuffer* frameBuffer);
static void forceUpdate(FrameBuffer* fb);

FBDriver thePL110Driver =
{
    .allocate = allocate,
    .dealloc = dealloc,
    .initialiseTheBuffer = initialiseTheBuffer,
    .forceUpdate = forceUpdate,
};

enum PL110Consts
{
    NUM_TIMINGS 		= 4,
    NUM_PALETTE_WORDS 	= 0x378,
    LCD_16_BPP			= 4,			// Register constant for 16 bits per pixel
};

#define MASKDEFINE(N,P,S)	\
	N ## _POS = (P),			\
	N ## _SIZE = (S),		\
	N ## _MASK = KLIB_MAKE_MASK((P),(S))

enum PL110RegisterMasks
{
    MASKDEFINE(PIXELS_PER_LINE,  2,  6),
    MASKDEFINE(LINES_PER_PANEL,  0, 10),
    
	MASKDEFINE(LCD_POWER	  , 11,  1),
	MASKDEFINE(LCD_ENDIAN	  ,  9,  2),
	MASKDEFINE(LCD_BGR	 	  ,  8,  1),
	MASKDEFINE(LCD_DUAL		  ,  7,  1),
	MASKDEFINE(LCD_TFT		  ,  5,  1),
	MASKDEFINE(LCD_BW		  ,  4,  1),
	MASKDEFINE(LCD_BPP		  ,  1,  3),
    MASKDEFINE(LCD_EN		  ,  0,  1),
};

struct PL110
{
    uint32_t  	timing[NUM_TIMINGS];
    uintptr_t 	upperFrameBufferPtr;
    uintptr_t 	lowerFrameBufferPtr;
    // TODO: QEMU veratilePB mofifies the PL110 so the next two registers are
    // the wrong way around.  
    uint32_t  	control;
    uint32_t  	interruptMaskSetClear;
    uint32_t	rawInterruptState;
    uint32_t	maskedInterruptState;
    uint32_t	interruptClear;
    uintptr_t	upperCurrent;
    uintptr_t	lowerCurrent;
    uint32_t	reserved[0x73];
	uint32_t	palette[NUM_PALETTE_WORDS];
    // TODO: Some other stuff
};

struct PL110FrameBuffer
{
    FrameBuffer base;
    PL110* registers;
};

typedef struct PL110FrameBuffer PL110FrameBuffer;

PL110FrameBuffer theOneAndOnlyPL110FrameBuffer = { { 0 } };

FBDriver* pl110_driver()
{
    return &thePL110Driver;
}


#pragma mark -
#pragma mark Driver functions


static FrameBuffer* allocate()
{
    klib_memset(&theOneAndOnlyPL110FrameBuffer,
                0,
                sizeof theOneAndOnlyPL110FrameBuffer);
    return (FrameBuffer*)&theOneAndOnlyPL110FrameBuffer;
}

static void dealloc(FrameBuffer* fb)
{
    // Do nothing - statically allocated
}

static FBError initialiseTheBuffer(FrameBuffer* frameBuffer)
{
    PL110FrameBuffer* pl110FrameBuffer = (PL110FrameBuffer*)frameBuffer;
    
    FBError error = FB_PARAMETER;
    /*
     *  We only support 16 bit colour
     *  TODO: Check that width is divisible by 16
     */
    if (frameBuffer->dimensions.bitDepth == 16)
    {
        PhysicalMemoryMap* memoryMap = pmm_getPhysicalMemoryMap();
        
        PL110* registers = pmm_getPL110(memoryMap);
        pl110FrameBuffer->registers = registers;
        size_t rasterWidth = frameBuffer->dimensions.width * sizeof(uint16_t);
        size_t frameBufferSize = rasterWidth * frameBuffer->dimensions.height;
        size_t numberOfPages
        	= (frameBufferSize + PIOS_PAGE_SIZE - 1) / PIOS_PAGE_SIZE;
        void* frameBufferMemory
        	= pmm_allocateContiguousPages(memoryMap, numberOfPages);
        /*
         *  Now it's time to initialise the LCD
         */
        registers->upperFrameBufferPtr = (uintptr_t)frameBufferMemory;
        /*
         *  Formula for the PPL value in the PL110 register.
         */
        size_t ppl = frameBuffer->dimensions.width / 16 - 1;
        /*
         *  Read and set the horizontal timing register
         */
        uint32_t timingRegister = registers->timing[0];
        timingRegister &= ~PIXELS_PER_LINE_MASK;
        timingRegister |= (ppl << PIXELS_PER_LINE_POS) & PIXELS_PER_LINE_MASK;
        registers->timing[0] = timingRegister;
        // TODO: Other values in the register
        /*
         * Vertical timings
         */
        timingRegister = registers->timing[1];
        timingRegister &= ~LINES_PER_PANEL_MASK;
        timingRegister |= ((frameBuffer->dimensions.height - 1) << LINES_PER_PANEL_POS)
                           & LINES_PER_PANEL_MASK;
        registers->timing[1] = timingRegister;
        uint32_t controlRegister = registers->control;
        /*
         *  Clear all the bits we might want to set
         */
        controlRegister &=    ~LCD_POWER_MASK
//        					| ~LCD_ENDIAN_MASK
        					| ~LCD_BGR_MASK
//        					| ~LCD_DUAL_MASK
//        					| ~LCD_TFT_MASK
//        					| ~LCD_BW_MASK
        					| ~LCD_BPP_MASK
        					| ~LCD_EN_MASK;
        controlRegister |=	  LCD_POWER_MASK		// Switch it on
//        											// endian 0 = little endian
//        											// BGR 0 = RGB
//        											// Dual 0 = one panel
        					| LCD_TFT_MASK			// TFT panel
//        											// BW 0 - in colour
        					| (LCD_16_BPP << LCD_BPP_POS)	// 16 bits per pixel
        					| LCD_EN_MASK;			// Enable the PL110
        registers->control = controlRegister;
        
        frameBuffer->buffer.frameBufferPtr = frameBufferMemory;
        frameBuffer->buffer.frameBufferSize = frameBufferSize;
        frameBuffer->buffer.rasterWidth = rasterWidth;
        frameBuffer->buffer.colourDepths[0] = 5;
        frameBuffer->buffer.colourDepths[1] = 5;
        frameBuffer->buffer.colourDepths[2] = 5;
        frameBuffer->buffer.colourDepths[3] = 0;
        error = FB_OK;
    }
    return error;
}

static void forceUpdate(FrameBuffer* fb)
{
    /*
     *  In QEMU we can force an update simply by reading and writing a register 
     */
    volatile uint32_t control = ((PL110FrameBuffer*)fb)->registers->control;
    ((PL110FrameBuffer*)fb)->registers->control = control;
}

