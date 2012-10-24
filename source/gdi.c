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

/*
 *  typedef for a function that can copy pixels in a device format
 */
typedef void PixelCopy(uint8_t* start, uint32_t newValue, size_t pixelCount);

struct GDIContext
{
    int referenceCount;
    FrameBufferDescriptor* fbDescriptor;
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


GDIContext theContexts[MAX_CONTEXTS] = { { 0 } };

GDIContext* gdi_initialiseGDI(FrameBufferDescriptor* fbDescriptor)
{
    klib_memset(theContexts, 0, sizeof theContexts);
    GDIContext* ret = &theContexts[0];
    ret->fbDescriptor = fbDescriptor;
    ret->font = pmm_getSystemFont(pmm_getPhysicalMemoryMap());
    ret->fontMinChar = 32;
    ret->fontMaxChar = 0x7E;
    switch(fbDescriptor->bitDepth)
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
    return ret;
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
        
        switch (context->fbDescriptor->bitDepth)
        {
            case 16:
                if (GDI_BITS_PER_COLOUR > FBPF_RED_16_BITS)
                {
                    redComponent >>= GDI_BITS_PER_COLOUR - FBPF_RED_16_BITS;
                }
                if (GDI_BITS_PER_COLOUR > FBPF_GREEN_16_BITS)
                {
                    greenComponent >>= GDI_BITS_PER_COLOUR - FBPF_GREEN_16_BITS;
                }
                if (GDI_BITS_PER_COLOUR > FBPF_BLUE_16_BITS)
                {
                    blueComponent >>= GDI_BITS_PER_COLOUR - FBPF_BLUE_16_BITS;
                }
                if (GDI_BITS_PER_COLOUR > FBPF_ALPHA_16_BITS)
                {
                    alphaComponent >>= GDI_BITS_PER_COLOUR - FBPF_ALPHA_16_BITS;
                }

                deviceColour = (redComponent << FBPF_RED_16_BIT_POS)
                             | (greenComponent << FBPF_GREEN_16_BIT_POS)
                			 | (blueComponent << FBPF_BLUE_16_BIT_POS)
                			 | (alphaComponent << FBPF_ALPHA_16_BIT_POS);
                break;
            case 32:
                if (GDI_BITS_PER_COLOUR > FBPF_RED_32_BITS)
                {
                    redComponent >>= GDI_BITS_PER_COLOUR - FBPF_RED_32_BITS;
                }
                if (GDI_BITS_PER_COLOUR > FBPF_GREEN_32_BITS)
                {
                    greenComponent >>= GDI_BITS_PER_COLOUR - FBPF_GREEN_32_BITS;
                }
                if (GDI_BITS_PER_COLOUR > FBPF_BLUE_32_BITS)
                {
                    blueComponent >>= GDI_BITS_PER_COLOUR - FBPF_BLUE_32_BITS;
                }
                if (GDI_BITS_PER_COLOUR > FBPF_ALPHA_16_BITS)
                {
                    alphaComponent >>= GDI_BITS_PER_COLOUR - FBPF_ALPHA_32_BITS;
                }
                
                deviceColour = (redComponent << FBPF_RED_32_BIT_POS)
                			 | (greenComponent << FBPF_GREEN_32_BIT_POS)
                			 | (blueComponent << FBPF_BLUE_32_BIT_POS)
                			 | (alphaComponent << FBPF_ALPHA_32_BIT_POS);
                break;
            default:
                ret = false;
                break;
        }
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
    ret.size.width = context->fbDescriptor->width;
    ret.size.height = context->fbDescriptor->height;
    return ret;
}


void gdi_fillFrame(GDIContext* context, GDIColourType colour)
{
    if (colour < GDI_NUM_COLOUR_TYPES)
    {
        uint8_t* bufferPtr = context->fbDescriptor->frameBufferPtr;
        uint32_t fillColour = context->deviceColours[colour];
        size_t numberOfLines = context->fbDescriptor->height;
        size_t width = context->fbDescriptor->width;
        size_t bytesPerLine = context->fbDescriptor->pitch;
        // TODO: can optimise by doing the whole buffer at once but requires
        // division or substitute.
        for (size_t line = 0 ; line < numberOfLines ; ++line)
        {
            context->pixelCopy(bufferPtr, fillColour, width);
            bufferPtr += bytesPerLine;
        }
    }
}


void gdi_setPixel(GDIContext* context, GDIPoint coords, GDIColourType colour)
{
    if (colour < GDI_NUM_COLOUR_TYPES
        && coords.x < context->fbDescriptor->width
        && coords.y < context->fbDescriptor->height)
    {
        size_t bytesPerPixel = context->fbDescriptor->bitDepth / 8;
        uint8_t* frameBufferPtr = context->fbDescriptor->frameBufferPtr;
        size_t index = coords.y * context->fbDescriptor->pitch + coords.x * bytesPerPixel;
        context->pixelCopy(frameBufferPtr + index, context->deviceColours[colour], 1);
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

void gdi_drawChar(GDIContext* context, GDIPoint point, PiosChar character)
{
    // TODO: We assume character width is 8 pixels and height is 16 rows
	if (character >= context->fontMinChar && character <= context->fontMaxChar)
    {
        
    }
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


