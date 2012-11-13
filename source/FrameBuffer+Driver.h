//
//  FrameBuffer+Driver.h
//  BakingPi
//
//  Created by Jeremy Pereira on 05/11/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_FrameBuffer_Driver_h
#define BakingPi_FrameBuffer_Driver_h
/*!
 *  @file FrameBuffer+Driver.h
 *  @brief Interface between a frame buffer and its underlying driver.
 *
 *  This includes the definition of the driver function table and the base
 *  frame buffer itself.
 */

#include "FrameBuffer.h"

/*!
 *  @brief Frame buffer driver provides frame buffer functionality for
 *  specific hardware.
 */
struct FBDriver
{
    /*!
     *  @brief Function that allocates a frame buffer
     *  @return a frame buffer or "subclass"
     */
    FrameBuffer* (*allocate)(void);
    /*!
     *  @brief function that deallocates a frame buffer.
     *  @param fb frame buffer to deallocate
     */
    void (*dealloc)(FrameBuffer* fb);
    
    /*!
     *  @brief Make the driver initialise the frame buffer.
     *  @param frameBuffer The frame buffer to initialise
     *  @param dimensions requested dimensions.
     *  @param buffer Place to put the frame buffer pointer and other driver
     *  generated dimensions.
     *  @return FB_OK or an error.
     */
    FBError (*initialiseTheBuffer)(FrameBuffer* frameBuffer);
    /*!
     *  @brief Function that forces a screen update.
     *  This is probably only necessary for emulated hardware with no 
     *  interrupt to drive it yet.  This function can be set to NULL if there
     *  is no requirment for a manual update.
     *  @param frameBuffer The frame buffer to update manually.
     */
    void 	(*forceUpdate)(FrameBuffer* frameBuffer);
};

/*!
 *  @brief Base frame buffer structure.
 */
struct FrameBuffer
{
    /*!
     *  @brief Driver function table.
     */
    FBDriver* driver;
    /*!
     *  @brief Client supplied dimensions for the frame buffer bitmap.
     */
    FBRequestDimensions dimensions;
    /*!
     *  @brief Driver supplied dimensions of the frame buffer bitmap.
     */
    FBBuffer buffer;
};

#endif
