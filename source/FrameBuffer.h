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
 *  @brief an opaque type that rerpesents the frame buffer postbox
 */
struct FBPostBox;
typedef struct FBPostBox FBPostBox;

/*!
 *  @brief get the address of the frame buffer postbox
 *  @return The frame buffer postbox
 */
FBPostBox* fb_getFPBostBox();

#endif
