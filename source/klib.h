//
//  klib.h
//  BakingPi
//
//  Created by Jeremy Pereira on 10/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_klib_h
#define BakingPi_klib_h

#include "bptypes.h"
/*!
 *  @file klib.h
 *  @brief Kernel library functions
 *  @author Jeremy Pereira
 */

/*!
 *  @brief if we don't have standard library functions we need memcpy
 */
void* klib_memcpy(void *restrict s1, const void *restrict s2, size_t n);

void* klib_memset(void* restrict ptr, uint8_t value, size_t length);


#endif
