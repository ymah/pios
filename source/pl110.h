//
//  pl110.h
//  BakingPi
//
//  Created by Jeremy Pereira on 05/11/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_pl110_h
#define BakingPi_pl110_h

#include "FrameBuffer.h"

/*!
 *  @brief Get the PL110 driver
 *  @return The PL110 driver
 */
FBDriver* pl110_driver();

/*!
 *  @brief PL110 controller handle
 */
struct PL110;
typedef struct PL110 PL110;

#endif
