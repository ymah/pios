//
//  gdi.c
//  BakingPi
//
//  Created by Jeremy Pereira on 19/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "gdi.h"
#include "klib.h"

const GDIColour GDI_BLACK_COLOUR = { .components = { 255, 0, 0, 0 }};
const GDIColour GDI_WHITE_COLOUR = { .components = { 255, 255, 255, 255 }};

struct GDIContext
{
    int referenceCount;
    FrameBufferDescriptor* fbDescriptor;
    uint32_t deviceColours[GDI_NUM_COLOUR_TYPES];
    GDIContext* previousContext;
};

enum
{
    MAX_CONTEXTS = 20,
};

GDIContext theContexts[MAX_CONTEXTS] = { { 0 } };

GDIContext* gdi_initialiseGDI(FrameBufferDescriptor* fbDescriptor)
{
    klib_memset(theContexts, 0, sizeof theContexts);
    GDIContext* ret = &theContexts[0];
    ret->fbDescriptor = fbDescriptor;
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
        switch (context->fbDescriptor->bitDepth)
        {
            case 16:
                deviceColour = newColour.components.red;
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
        size_t frameBufferSize = context->fbDescriptor->frameBufferSize;
        uint8_t* bufferPtr = context->fbDescriptor->frameBufferPtr;
        uint32_t fillColour = context->deviceColours[colour];
        size_t bytesPerPixel = context->fbDescriptor->bitDepth / 8;
        uint8_t* colourBytePtr = (uint8_t*)&fillColour;
        size_t colourIndex = 0;
        for (size_t bufferIndex = 0 ; bufferIndex < frameBufferSize ; ++bufferIndex)
        {
            bufferPtr[bufferIndex] = colourBytePtr[colourIndex++];
            if (colourIndex >= bytesPerPixel)
            {
                colourIndex = 0;
            }
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

