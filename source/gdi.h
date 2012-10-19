//
//  gdi.h
//  BakingPi
//
//  Created by Jeremy Pereira on 19/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_gdi_h
#define BakingPi_gdi_h

#include "FrameBuffer.h"

/*!
 *  @file gdi.h
 *  @brief Lowish level graphics routines
 *  @author Jeremy Pereira
 */

/*!
 *  @brief Graphics context.
 *
 *  A graphics context contains information about the size of the frame buffer
 *  bit and colour depths etc.  It also knows what things like the current 
 *  foreground and background colours are.
 */
struct GDIContext;
typedef struct GDIContext GDIContext;

/*!
 *  @brief constants denoting pen fill or background.
 *
 *  These may be used as indexes into an array.
 */
enum GDIColourType
{
    GDI_BACKGROUND 	= 0,
    GDI_PEN 	 	= 1,
    GDI_FILL		= 2,
    GDI_NUM_COLOUR_TYPES,
};
typedef enum GDIColourType GDIColourType;

/*!
 *  @brief represents a GDI colour
 *
 *  Colours are 32 bit RGBA values
 */
union GDIColour
{
    uint32_t rgba;
    struct Components
    {
        uint8_t alpha;
        uint8_t blue;
        uint8_t green;
        uint8_t red;
    } components;
};

typedef union GDIColour GDIColour;

extern const GDIColour GDI_BLACK_COLOUR;
extern const GDIColour GDI_WHITE_COLOUR;


/*!
 *  @brief Initialise the GDI with the given frame buffer.
 *
 *  @param fbDescriptor frame buffer descriptor.
 *  @return An initial graphics context
 */
GDIContext* gdi_initialiseGDI(FrameBufferDescriptor* fbDescriptor);

/*!
 *  @brief Saves the current state of the graphics context.
 *  @param currentContext Context to save.
 *  @return A copy of currentContext
 */
GDIContext* gdi_pushContext(GDIContext* currentContext);

/*!
 *  @brief Retrieve the previously saved state of the graphics context.
 *
 *  @param currentContext The graphics context that will be popped.
 *  @return the previously saved state of the graphics context.
 */
GDIContext* gdi_popContext(GDIContext* currentContext);

/*!
 *  @brief Set either the foreground or the background colour.
 *  @param context The GDI context to set the colour for
 *  @param colourType Is it the pen or the background or fill
 *  @param newColour The new colour to set
 *  @return true if the colour was successfully set.
 */
bool gdi_setColour(GDIContext* context,
         	       GDIColourType colourType,
                   GDIColour newColour);

/*!
 *  @brief Make a GDI colour from its components.
 *  @param red Red component
 *  @param green Green component
 *  @param blue Blue component
 *  @return A GDI colour made from the components given
 */
GDIColour gdi_makeColour(uint8_t red,
                         uint8_t green,
                         uint8_t blue,
                         uint8_t alpha);

#endif
