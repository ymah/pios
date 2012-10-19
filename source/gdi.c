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

GDIContext theContexts[MAX_CONTEXTS];

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

