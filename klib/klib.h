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
#if !defined PIOS_SIMULATOR
/*
 *  memcpy is used as a compiler intrinsic for large structs, so it must exist
 */
#define klib_memcpy	memcpy
#endif
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

/*!
 *  @brief Make a mask of a sequence of 1 bits at the specified position
 *  @param POS bit position of the least significant bit
 *  @param SIZE Number of bits set in the mask
 *  @return a bit mask with 1's set according to the parameters
 */
#define KLIB_MAKE_MASK(POS,SIZE)	((~(~0 << (SIZE))) << (POS))

#endif
