//
//  InterruptVector.h
//  BakingPi
//
//  Created by Jeremy Pereira on 29/11/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_InterruptVector_h
#define BakingPi_InterruptVector_h

#include "bptypes.h"

enum IVType
{
    IV_RESET 					= 0,
    IV_UNDEFINED_INSTRUCTION	= 1,
    IV_SOFTWARE					= 2,
    IV_INSTRUCTION_ABORT		= 3,
    IV_DATA_ABORT				= 4,
    IV_IRQ						= 6,
    IV_FAST_IRQ					= 7,
    IV_NUM_VECTORS
};

typedef uint32_t InterruptVector;

struct InterruptVectorTable;
typedef struct InterruptVectorTable InterruptVectorTable;

/*!
 *  @brief Initialise the interrup vector table
 */
void iv_initialise(void);

#endif
