//
//  gdi.c
//  BakingPi
//
//  Created by Jeremy Pereira on 19/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "gdi.h"
#include "klib.h"
#include "PhysicalMemoryMap.h"

const GDIColour GDI_BLACK_COLOUR = { .components = { 255, 0, 0, 0 }};
const GDIColour GDI_WHITE_COLOUR = { .components = { 255, 255, 255, 255 }};
const GDIColour GDI_RED_COLOUR   = { .components = { 255, 0, 0, 255 }};
const GDIColour GDI_GREEN_COLOUR = { .components = { 255, 0, 255, 0 }};
const GDIColour GDI_BLUE_COLOUR  = { .components = { 255, 255, 0, 0 }};

/*
 *  typedef for a function that can copy pixels in a device format
 */
typedef void PixelCopy(uint8_t* start, uint32_t newValue, size_t pixelCount);

struct GDIContext
{
    int referenceCount;
    FrameBuffer* fbDescriptor;
    uint32_t deviceColours[GDI_NUM_COLOUR_TYPES];
    GDIContext* previousContext;
    PixelCopy* pixelCopy;
    uint8_t bytesPerPixel;
    uint8_t* font;
    uint32_t fontMinChar;
    uint32_t fontMaxChar;
};

enum
{
    MAX_CONTEXTS = 20,
};

static void pixelCopy16(uint8_t* start, uint32_t newValue, size_t pixelCount);
static void pixelCopy32(uint8_t* start, uint32_t newValue, size_t pixelCount);


static GDIContext theContexts[MAX_CONTEXTS] = { { 0 } };
static GDIContext* currentContext = NULL;

static void release(GDIContext* context);


GDIContext* gdi_initialiseGDI(FrameBuffer* fbDescriptor)
{
    if (currentContext == NULL)
    {
        klib_memset(theContexts, 0, sizeof theContexts);
        GDIContext* ret = &theContexts[0];
        
        ret->referenceCount = 1;
        ret->fbDescriptor = fbDescriptor;
        ret->font = pmm_getSystemFont(pmm_getPhysicalMemoryMap());
        ret->fontMinChar = 32;
        ret->fontMaxChar = 0x7E;
        FBRequestDimensions dimensions;
        if (fb_getDimensions(fbDescriptor, &dimensions, NULL) == FB_OK)
        {
            switch(dimensions.bitDepth)
            {
                case 16:
                    ret->pixelCopy = pixelCopy16;
                    ret->bytesPerPixel = 2;
                    break;
                case 32:
                    ret->pixelCopy = pixelCopy32;
                    ret->bytesPerPixel = 4;
                    break;
                default:
                    ret = NULL;
            }
        }
        else
        {
            ret = NULL;
        }
        currentContext = ret;
    }
    return currentContext;
}

GDIContext* gdi_pushContext(GDIContext* contextToSave)
{
    GDIContext* emptyContext = NULL;
    /*
     *  Find an empty slot by checking the reference counts
     */
    for (int i = 0 ; i < MAX_CONTEXTS && emptyContext == NULL ; ++i)
    {
        if (theContexts[i].referenceCount == 0)
        {
            emptyContext = &theContexts[i];
        }
    }
    if (emptyContext != NULL)
    {
        contextToSave->referenceCount++;
        *emptyContext = *contextToSave;
        emptyContext->referenceCount = 1;
        emptyContext->previousContext = contextToSave;
    }
    return emptyContext;
}

GDIContext* gdi_popContext(GDIContext* aContext)
{
    GDIContext* retContext = aContext;
    
    if (aContext->previousContext != NULL)
    {
        retContext = aContext->previousContext;
        release(aContext);
    }
    return retContext;
}

void release(GDIContext* context)
{
    if (context != NULL)
    {
        if (context->referenceCount == 1)
        {
            context->referenceCount = 0 ;
            release(context->previousContext);
        }
        else if (context->referenceCount > 0)
        {
            context->referenceCount--;
        }
    }
}

GDIContext* gdi_currentContext()
{
    return currentContext;
}

bool gdi_setColour(GDIContext* context,
         	       GDIColourType colourType,
                   GDIColour newColour)
{
    bool ret = false;
    if (colourType < GDI_NUM_COLOUR_TYPES)
    {
        ret = true;
        uint32_t deviceColour = 0;
        uint32_t redComponent = newColour.components.red;
        uint32_t greenComponent = newColour.components.green;
        uint32_t blueComponent = newColour.components.blue;
        uint32_t alphaComponent = newColour.components.alpha;
        
        FBBuffer bufferDescriptor;
        
        fb_getDimensions(context->fbDescriptor, NULL, &bufferDescriptor);
        uint8_t redBits		= bufferDescriptor.colourDepths[0];
        uint8_t greenBits	= bufferDescriptor.colourDepths[1];
        uint8_t blueBits	= bufferDescriptor.colourDepths[2];
        uint8_t alphaBits	= bufferDescriptor.colourDepths[3];
        
        uint8_t alphaBitPos = 0;
        uint8_t blueBitPos	= alphaBitPos + alphaBits;
        uint8_t greenBitPos	= blueBitPos + blueBits;
        uint8_t redBitPos	= greenBitPos + greenBits;
        
        if (GDI_BITS_PER_COLOUR > redBits)
        {
            redComponent >>= GDI_BITS_PER_COLOUR - redBits;
        }
        if (GDI_BITS_PER_COLOUR > greenBits)
        {
            greenComponent >>= GDI_BITS_PER_COLOUR - greenBits;
        }
        if (GDI_BITS_PER_COLOUR > blueBits)
        {
            blueComponent >>= GDI_BITS_PER_COLOUR - blueBits;
        }
        if (GDI_BITS_PER_COLOUR > alphaBits)
        {
            alphaComponent >>= GDI_BITS_PER_COLOUR - alphaBits;
        }
        
        deviceColour =    (redComponent << redBitPos)
        				| (greenComponent << greenBitPos)
        				| (blueComponent << blueBitPos)
        				| (alphaComponent << alphaBitPos);

        if (ret)
        {
            context->deviceColours[colourType] = deviceColour;
        }
    }
    return ret;
}

GDIRect gdi_frame(GDIContext* context)
{
    GDIRect ret;
    ret.origin.x = 0;
    ret.origin.y = 0;
    FBRequestDimensions dimensions;
    fb_getDimensions(context->fbDescriptor, &dimensions, NULL);
    ret.size.width = dimensions.width;
    ret.size.height = dimensions.height;
    return ret;
}


void gdi_fillFrame(GDIContext* context, GDIColourType colour)
{
    if (colour < GDI_NUM_COLOUR_TYPES)
    {
        FBRequestDimensions dimensions;
        FBBuffer buffer;
        fb_getDimensions(context->fbDescriptor, &dimensions, &buffer);

        uint8_t* bufferPtr = buffer.frameBufferPtr;
        uint32_t fillColour = context->deviceColours[colour];
        for (size_t line = 0 ; line < dimensions.height ; ++line)
        {
            context->pixelCopy(bufferPtr, fillColour, dimensions.width);
            bufferPtr += buffer.rasterWidth;
        }
    }
}

void gdi_fillRect(GDIContext* context, GDIRect rect, GDIColourType colour)
{
    GDIPoint currentPoint = rect.origin;
    
    for (int y = 0 ; y < rect.size.height ; ++y)
    {
        for (int x = 0 ; x < rect.size.width ; ++x)
        {
            gdi_setPixel(context, currentPoint, colour);
            currentPoint.x++;
        }
        currentPoint.y++;
        currentPoint.x = rect.origin.x;
    }
}


void gdi_setPixel(GDIContext* context, GDIPoint coords, GDIColourType colour)
{
    FBRequestDimensions dimensions;
    FBBuffer buffer;
    fb_getDimensions(context->fbDescriptor, &dimensions, &buffer);

    if (colour < GDI_NUM_COLOUR_TYPES
        && dimensions.width
        && dimensions.height)
    {
        size_t bytesPerPixel = dimensions.bitDepth / 8;
        uint8_t* frameBufferPtr = buffer.frameBufferPtr;
        size_t index = coords.y * buffer.rasterWidth + coords.x * bytesPerPixel;
        context->pixelCopy(frameBufferPtr + index,
                           context->deviceColours[colour],
                           1);
    }
}

void gdi_line(GDIContext* context, GDIPoint p0, GDIPoint p1)
{
    int dx = p1.x - p0.x;
    int dy = p1.y - p0.y;
    
    if (dx < 0)
    {
        dx = -dx;
    }
    if (dy < 0)
    {
        dy = -dy;
    }
    
    int sx = p0.x < p1.x ? 1 : -1;
    int sy = p0.y < p1.y ? 1 : -1;
    int error = dx - dy;
    while(p0.x != p1.x && p0.y != p1.y)
    {
        gdi_setPixel(context, p0, GDI_PEN);
        int e2 = 2 * error;
        if (e2 > -dy)
        {
            error -= dy;
            p0.x += sx;
        }
        if (e2 < dx)
        {
            error += dx;
            p0.y += sy;
        }
    }
    gdi_setPixel(context, p0, GDI_PEN);

}

#define CHAR_WIDTH	8
#define CHAR_HEIGHT	16

GDIRect gdi_drawChar(GDIContext* context, GDIPoint point, PiosChar character)
{
    // TODO: We assume character width is 8 pixels and height is 16 rows
	if (character >= context->fontMinChar && character <= context->fontMaxChar)
    {
        GDIPoint currentPixel = point;
        
        uint8_t* charStart = context->font
                           + character * CHAR_HEIGHT * (CHAR_WIDTH / 8);
        for (int row = 0 ; row < CHAR_HEIGHT ; ++row)
        {
            uint8_t rasterLine = *charStart;
            if (rasterLine != 0) // Quick check for a blank line
            {
                for (int col = 0 ; col < CHAR_WIDTH ; ++col)
                {
                    if ((rasterLine & 1) != 0)
                    {
                        gdi_setPixel(context, currentPixel, GDI_PEN);
                    }
                    rasterLine >>= 1;
                    currentPixel.x++;
                }
            }
            currentPixel.y++;
            currentPixel.x = point.x;
            charStart += CHAR_WIDTH / 8;
        }
    }
    GDIRect ret;
    ret.origin = point;
    ret.size.width = CHAR_WIDTH;
    ret.size.height = CHAR_HEIGHT;
    return ret;
}

GDISize gdi_systemFontCharSize(GDIContext* context)
{
    GDISize ret = { .width = CHAR_WIDTH, .height = CHAR_HEIGHT };
    return ret;
}

GDIColour gdi_makeColour(uint8_t red,
                         uint8_t green,
                         uint8_t blue,
                         uint8_t alpha)
{
    GDIColour ret;
    ret.components.alpha = alpha;
    ret.components.blue = blue;
    ret.components.green = green;
    ret.components.red = red;
  	return ret;
}

void pixelCopy16(uint8_t* start, uint32_t newValue, size_t pixelCount)
{
    uint16_t* pixelPtr = (uint16_t*) start;
    for (size_t i = 0 ; i < pixelCount ; ++i)
    {
        *pixelPtr++ = (uint16_t) newValue;
    }
    
}

static void pixelCopy32(uint8_t* start, uint32_t newValue, size_t pixelCount)
{
    uint32_t* pixelPtr = (uint32_t*) start;
    for (size_t i = 0 ; i < pixelCount ; ++i)
    {
        *pixelPtr++ = newValue;
    }
    
}


