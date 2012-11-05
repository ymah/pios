//
//  FrameBuffer.h
//  BakingPi
//
//  Created by Jeremy Pereira on 27/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_FrameBuffer_h
#define BakingPi_FrameBuffer_h

#include "bptypes.h"


/*!
 *  @brief Error codes returned when getting a frame buffer.
 */
enum FBError
{
    /*!
     *  @brief Success!
     */
    FB_OK 		  = 0,
    /*!
     *  @brief Parameter validation error
     */
    FB_PARAMETER  = 1,
    /*!
     *  @brief Descriptor alignment error
     */
 	FB_ALIGNMENT  = 2,
    /*!
     *  @brief The actual get from the GPU failed.
     */
    FB_FAILED_GET = 3,
    /*!
     *  @brief The communication was interrupted because we are stopping
     */
    FB_STOPPED    = 4,
    /*!
     *  @brief Function not implemented yet
     */
    FB_NOT_IMPLEMENTED = 6,
    /*!
     *  @brief Attempt to do an operation on an uninitialised frame buffer
     */
    FB_NOT_INITIALISED = 7,
};

typedef enum FBError FBError;

struct FrameBuffer;
typedef struct FrameBuffer FrameBuffer;

struct FBDriver;
typedef struct FBDriver FBDriver;


/*!
 *  @brief Used when requesting a frame buffer to define the dimensions of the
 *  buffer.
 */
struct FBRequestDimensions
{
    /*!
     *  @brief Width of frame buffer in pixels
     */
    uint16_t width;
    /*!
     *  @brief Height of frame buffer in pixels
     */
    uint16_t height;
    /*!
     *  @brief Bit depth of frame buffer in bits
     */
    uint16_t bitDepth;
    /*!
     *  @brief X coordinate of top left corner of the frame buffer
     */
    uint16_t x;
    /*!
     *  @brief Y coordinate of top left corner of the frame buffer
     */
    uint16_t y;
};

typedef struct FBRequestDimensions FBRequestDimensions;

/*!
 *  @brief structure that describes the frame buffer bytes returned by 
 *  fb_initialiseFrameBuffer()
 */
struct FBBuffer
{
    /*!
     *  @brief Pointer to the top left corner of the frame buffer bytes
     */
    void* frameBufferPtr;
    /*!
     *  @brief Size of the frame buffer bytes
     */
    size_t frameBufferSize;
    /*!
     *  @brief Width of a raster line in the frame buffer in bytes
     */
    size_t rasterWidth;
    /*!
     *  @brief Array of the number of bits assigned to each colour (RGBA)
     */
    uint8_t colourDepths[4];
};

typedef struct FBBuffer FBBuffer;

/*!
 *  @brief Get a (or the) frame buffer.
 *
 *  Allocates and initialises a frame buffer.
 *  @param driver Functions used by the underlying frame buffer driver.
 *  @return A new frame buffer object or NULL if something goes wrong.
 */
FrameBuffer* fb_getFrameBuffer(FBDriver* driver);

/*!
 * @brief Get the frame buffer associated with the screen.
 * @return The screen's frame buffer
 */
FrameBuffer* fb_getScreenFrameBuffer();

/*!
 *  @brief Set the screen frame buffer
 *  @param frameBuffer The new fram buffer for the screen
 */
void fb_setScreenFrameBuffer(FrameBuffer* frameBuffer);

/*!
 *  @brief Get a frame buffer
 *  @param postbox The GPU frame buffer postbox address
 *  @param fbDescriptor a pointer to a frame buffer descriptor.  This needs to
 *  be at least 16 byte aligned, possibly even page aligned.  The GPU will fill
 *  in the address and some other stuff.
 *  @return An error code.  0 means success.
 */

FBError fb_initialiseFrameBuffer(FrameBuffer* frameBuffer,
                                 FBRequestDimensions* fbDimensions);

/*!
 *  @brief Get the dimensions and description of the frame buffer.
 *  @param frameBuffer The frame buffer
 *  @param dimensions If not null will be filled in with the dimensions used to 
 *  initialise the frame buffer.
 *  @param bufferDescriptor If not null will be filled in with the location and
 *  size of the frame buffer RAM.
 *  @return FB_OK if it works or an error if the frame buffer has not been 
 *  initialised.
 */
FBError fb_getDimensions(FrameBuffer* frameBuffer,
                         FBRequestDimensions* dimensions,
                         FBBuffer* bufferDescriptor);

#endif
