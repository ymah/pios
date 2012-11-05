//
//  FrameBuffer.h
//  BakingPi
//
//  Created by Jeremy Pereira on 27/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_VideoCore_h
#define BakingPi_VideoCore_h
/*!
 *  @brief The VideoCore driver
 */

#include "bptypes.h"

struct VCPostBox;
typedef struct VCPostBox VCPostBox;

enum
{
    /*!
     *  @brief Number of supported postbox channels for the CPU
     */
    VC_POSTBOX_CHANNELS = 8,
};

enum PostBoxChannel
{
    VC_FRAME_BUFFER_CHANNEL = 1,
};

/*!
 *  @prief pixel formats for diffewrent colours.
 *
 *  In all cases, pixels are stored RGBA with alpha in the least significant
 *  position.
 */
enum VCPixelFormat
{
    VCPF_ALPHA_16_BITS 		= 0,
    VCPF_RED_16_BITS 		= 5,
    VCPF_GREEN_16_BITS 		= 6,
    VCPF_BLUE_16_BITS 		= 5,
    
    VCPF_ALPHA_16_BIT_POS 	= 0,
    VCPF_BLUE_16_BIT_POS	= VCPF_ALPHA_16_BITS,
    VCPF_GREEN_16_BIT_POS	= VCPF_BLUE_16_BIT_POS + VCPF_BLUE_16_BITS,
    VCPF_RED_16_BIT_POS		= VCPF_GREEN_16_BIT_POS + VCPF_GREEN_16_BITS,

    
    VCPF_RED_32_BITS		= 8,
    VCPF_GREEN_32_BITS		= 8,
    VCPF_BLUE_32_BITS		= 8,
    VCPF_ALPHA_32_BITS		= 8,
    
    VCPF_ALPHA_32_BIT_POS	= 0,
    VCPF_BLUE_32_BIT_POS	= VCPF_ALPHA_32_BIT_POS + VCPF_ALPHA_32_BITS,
    VCPF_GREEN_32_BIT_POS	= VCPF_BLUE_32_BIT_POS + VCPF_BLUE_32_BITS,
    VCPF_RED_32_BIT_POS		= VCPF_GREEN_32_BIT_POS + VCPF_GREEN_32_BITS,

};


typedef enum PostBoxChannel PostBoxChannel;

typedef enum FBChannel FBChannel;

/*!
 *  @brief Bit positions of various important bits within postbox messages.
 */
enum VCBits
{
/*!
 *  @brief How many low order bits of a message will be used for the channel
 */
    VC_CHANNEL_BITS 		   	= 4,
/*!
 *  @brief Required alignment for a pointer to the frame buffer descriptor
 */
    VC_DESCRIPTOR_ALIGNMENT 	= 12,
};

/*!
 *  @brief Masks out the channel bits of a message leaving just the value
 */
#define VC_VALUE_MASK		(0xFFFFFFFFFFFFFFFFl << VC_CHANNEL_BITS)
/*!
 *  @brief MAsks out the value bits of a message leaving just the channel
 */
#define VC_CHANNEL_MASK		(~VC_VALUE_MASK)
/*!
 *  @brief Mask to align a pointer to be used as a frame buffer descriptor
 */
#define VC_DESCRIPTOR_ALIGNMENT_MASK	\
							~((~(uintptr_t) 0) << VC_DESCRIPTOR_ALIGNMENT)


/*!
 *  @brief A descriptor for requesting a frame buffer
 */
struct VCFrameBufferDescriptor
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

typedef struct VCFrameBufferDescriptor VCFrameBufferDescriptor;


#if defined PIOS_SIMULATOR

/*!
 *  @brief Get the video core driver
 *  @return The videocore driver
 */
FBDriver* vc_driver();

/*!
 *  @brief Allocate a post box structure
 *  @return frame buffer postbox
 */
VCPostBox* vc_postBoxAlloc();

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
bool vc_postBoxWasWritten(VCPostBox* postbox, uintptr_t* messageRef);

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
bool vc_tryMakeRead(VCPostBox* postbox, uint32_t channel, uintptr_t message);


#endif

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
bool vc_send(VCPostBox* postbox, uintptr_t message, uint32_t channel);

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
uintptr_t vc_read(VCPostBox* postbox, uint32_t channel);


#endif
