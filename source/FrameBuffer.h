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

enum
{
    /*!
     *  @brief Number of supported postbox channels for the CPU
     */
    FB_POSTBOX_CHANNELS = 8,
};
/*!
 *  @brief an opaque type that rerpesents the frame buffer postbox
 */
struct FBPostBox;
typedef struct FBPostBox FBPostBox;

struct FrameBuffer;
typedef struct FrameBuffer FrameBuffer;

/*!
 *  @brief get the address of the frame buffer postbox
 *  @return The frame buffer postbox
 */
FBPostBox* fb_getFPBostBox();

/*!
 *  @brief Low level send to mailbox.
 *
 *  This function works by spinning on a status flag until it can write the 
 *  message.
 *  @param postbox the address of the mailbox
 *  @param message the message to write.  The low four bits must be zero.
 *  @param channel the mail box channel - 0 to 7 at the moment
 *  @return true if the parameters validate OK
 */
bool fb_send(FBPostBox* postbox, uint32_t message, uint32_t channel);

/*!
 *  @brief Read the postbox for data from the specified channel.
 *
 *  This function works by spinning on a status flag until it can read some
 *  data.  This means that, if the GPU is not expecting to need to send 
 *  something, this method will hang.  Also, any messages prior to the first
 *  one for the specified channel will be discarded.
 *  @param postbox Address of the GPU postbox.
 *  @param channel Channel to read from, must be in the range 0-7.
 *  @return The data on the channel or ((uint32_t)-1) if the parameters are 
 *  incorrect.
 *  Valid channel data will always have the low four bits clear.
 */
uint32_t fb_read(FBPostBox* postbox, uint32_t channel);


#endif
