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

enum PostBoxChannel
{
    PB_FRAME_BUFFER_CHANNEL = 1,
};

typedef enum PostBoxChannel PostBoxChannel;

typedef enum FBChannel FBChannel;
/*!
 *  @brief an opaque type that rerpesents the frame buffer postbox
 */
struct FBPostBox;
typedef struct FBPostBox FBPostBox;

/*!
 *  @brief A descriptor for requesting a frame buffer
 */
struct FrameBufferDescriptor
{
    /*!
     *  @brief Width in pixels
     */
    uint32_t width;
    /*!
     *  @brief Height in pixels
     */
    uint32_t height;
    /*!
     *  @brief Don't know but set to same as width
     */
    uint32_t vWidth;
    /*!
     *  @brief Don't know but set to same as height.  Might be the width of a 
     *  raster line.
     */
    uint32_t vHeight;
    /*!
     * @brief width of a raster line in bytes (set by GPU)
     */
    uint32_t pitch;
    /*!
     *  @brief Number of bits per pixel
     */
    uint32_t bitDepth;
    /*!
     *  @brief x offset of top left corner of screen
     */
    uint32_t x;
    /*!
     * @brief y offset of top left corner of screen
     */
    uint32_t y;
    /*!
     *  @brief pointer to frame buffer (set by GPU)
     */
    void* frameBufferPtr;
    /*!
     *  @brief frame buffer size calculated by GPU
     */
    uint32_t frameBufferSize;
};

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
     *  @brief PArameter validation error
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
};

typedef enum FBError FBError;

typedef struct FrameBufferDescriptor FrameBufferDescriptor;

/*!
 *  @brief Bit positions of various important bits within postbox messages.
 */
enum FBBits
{
/*!
 *  @brief How many low order bits of a message will be used for the channel
 */
    FB_CHANNEL_BITS 		   	= 4,
/*!
 *  @brief Required alignment for a pointer to the frame buffer descriptor
 */
    FB_DESCRIPTOR_ALIGNMENT 	= 12,
};

/*!
 *  @brief Masks out the channel bits of a message leaving just the value
 */
#define FB_VALUE_MASK		(0xFFFFFFFFFFFFFFFFl << FB_CHANNEL_BITS)
/*!
 *  @brief MAsks out the value bits of a message leaving just the channel
 */
#define FB_CHANNEL_MASK		(~FB_VALUE_MASK)
/*!
 *  @brief Mask to align a pointer to be used as a frame buffer descriptor
 */
#define FB_DESCRIPTOR_ALIGNMENT_MASK	\
							~((~(uintptr_t) 0) << FB_DESCRIPTOR_ALIGNMENT)

#if defined PIOS_SIMULATOR

/*!
 *  @brief Allocate a post box structure
 *  @return frame buffer postbox
 */
FBPostBox* fb_postBoxAlloc();

/*!
 *  @brief Check if the postbax has been written.
 *
 *  Gives us access to the message that was written.  Also clears the write 
 *  status flag so the postbox can be written again.
 *  @param postbox postbox to check
 *  @param message reference to the location where the message will be put if
 *  the postbox was written.  If NULL, the message written will be discarded.
 *  @return true if the post box was written.
 */
bool fb_postBoxWasWritten(FBPostBox* postbox, uintptr_t* messageRef);

/*!
 *  @brief Set up a postbox read if we can.
 *
 *  If the last read was done, we set the read value and the status bit.
 *  @param postbox Postbox to try this on.
 *  @param channel Channel to set.
 *  @param message Message to send.
 *  @return true if the postbox read was available i.e. the previously set read
 *  value has been read.
 */
bool fb_tryMakeRead(FBPostBox* postbox, uint32_t channel, uintptr_t message);


#endif

/*!
 *  @brief Get a frame buffer
 *  @param postbox The GPU frame buffer postbox address
 *  @param fbDescriptor a pointer to a frame buffer descriptor.  This needs to
 *  be at least 16 byte aligned, possibly even page aligned.  The GPU will fill
 *  in the address and some other stuff.
 *  @return An error code.  0 means success.
 */
FBError fb_getFrameBuffer(FBPostBox* postbox, FrameBufferDescriptor* fbDescriptor);

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
bool fb_send(FBPostBox* postbox, uintptr_t message, uint32_t channel);

/*!
 *  @brief Read the postbox for data from the specified channel.
 *
 *  This function works by spinning on a status flag until it can read some
 *  data.  This means that, if the GPU is not expecting to need to send 
 *  something, this method will hang.  Also, any messages prior to the first
 *  one for the specified channel will be discarded.
 *  @param postbox Address of the GPU postbox.
 *  @param channel Channel to read from, must be in the range 0-7.
 *  @return The data on the channel or ((uintptr_t)-1) if the parameters are 
 *  incorrect.
 *  Valid channel data will always have the low four bits clear.
 */
uintptr_t fb_read(FBPostBox* postbox, uint32_t channel);


#endif
