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
 *  @brief if we don't have standard library functions we need memcpy.
 *  @param s1 Destination pointer.
 *  @param s2 Source pointer.
 *  @param n Number of chars to copy.
 *  @return The destination pointer.
 */
void* klib_memcpy(void *restrict s1, const void *restrict s2, size_t n);

/*!
 *  @brief Replacement for standard library memset.
 *  @param ptr Start of area to fill.
 *  @param value Value to fill memory with.
 *  @param length Number of bytes to fill memory with.
 *  @return ptr
 */
void* klib_memset(void* restrict ptr, uint8_t value, size_t length);

/*!
 *  @brief Replacement for C lib strnlen
 *  @param charSequence Character sequence to check length of.
 *  @param maxLength  Abort the search after this many characters.
 *  @return The smaller of maxLength and the length of the sequence before the
 *  first zero bytes.
 */
size_t klib_strnlen(const char* restrict charSequence, size_t maxLength);

#endif
