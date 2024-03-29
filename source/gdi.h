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

enum
{
    /*!
     *  @brief Number of bits in a pixel for each colour
     */
    GDI_BITS_PER_COLOUR = 8,
};

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
extern const GDIColour GDI_RED_COLOUR;
extern const GDIColour GDI_GREEN_COLOUR;
extern const GDIColour GDI_BLUE_COLOUR;

/*!
 *  @brief Coordinates of a pixel in a GDI context
 */
struct GDIPoint
{
    int x;
    int y;
};

typedef struct GDIPoint GDIPoint;

/*!
 *  @brief A rectangular size in GDI pixels
 */
struct GDISize
{
    size_t width;
    size_t height;
};

typedef struct GDISize GDISize;

/*!
 *  @brief Defines a rectangle in the GDI.  
 *
 *  The orifin deternines the coordinate of the rectangle closest to the origin
 */
struct GDIRect
{
    GDIPoint origin;
    GDISize size;
};
typedef struct GDIRect GDIRect;

/*!
 *  @brief Initialise the GDI with the given frame buffer.
 *
 *  @param fbDescriptor frame buffer descriptor.
 *  @return An initial graphics context
 */
GDIContext* gdi_initialiseGDI(FrameBuffer* fbDescriptor);

/*!
 *  @brief Gets the current GDI context.
 *  @return The current context unless it has not been initialised yet.
 */
GDIContext* gdi_currentContext();

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

/*!
 *  @brief Gives the frame rectangle for the context
 *  @param context The graphics context to get the frame of
 *  @return A rectangle encompassing the whole frame buffer.
 */
GDIRect gdi_frame(GDIContext* context);

/*!
 *  @brief fills the entire context with the same colour.
 *  @param context The GDI context to fill.
 *  @param colour The colour must be the background, fill or pen colour.
 */
void gdi_fillFrame(GDIContext* context, GDIColourType colour);

/*!
 *  @brief Fill a rectangle with a colour.
 *  @param context GDI context
 *  @param rect Rectangle to fill
 *  @param colour The colour must be the background, fill or pen colour.
 */
void gdi_fillRect(GDIContext* context, GDIRect rect, GDIColourType colour);

/*!
 *  @brief Set the given pixel to the given colour
 *  @param context The graphics context to set the pixel in.
 *  @param coords Device coordinates of the pixel to set
 *  @param colour The colour must be the background, fill or pen colour.
 */
void gdi_setPixel(GDIContext* context, GDIPoint coords, GDIColourType colour);

/*!
 *  @brief draws a line between the two given points in the pen colour
 *  @param context Graphics context.
 *  @param p0 Start point
 *  @param p1 End point
 */
void gdi_line(GDIContext* context, GDIPoint p0, GDIPoint p1);

/*!
 *  @brief Get the character size of the system font.
 *
 *  The width might not be trustworthy for fonts that are not monospaced.
 *  @param context The context to get the character size for.
 *  @return A size containing the average width and height of a character in the
 *  system font.
 */
GDISize gdi_systemFontCharSize(GDIContext* context);

/*!
 *  @brief put a character on the screen at the specified point
 *  @param context Graphics context
 *  @param point origin of GDIRect containing the chracter.
 *  @param character Character to put on the screen.
 *  @return The rectangle in which the character was drawn.
 */
GDIRect gdi_drawChar(GDIContext* context, GDIPoint point, PiosChar character);

#endif
