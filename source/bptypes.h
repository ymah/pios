//
//  bptypes.h
//  BakingPi
//
//  Created by Jeremy Pereira on 24/09/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_bptypes_h
#define BakingPi_bptypes_h

#if !defined __bool_true_false_are_defined
/*!
 *  @brief Boolean type as per C99
 */
typedef _Bool bool;
#define	false	0
#define	true	1

#endif

/*!
 *  @brief unsigned 8 bit type as per C99
 */
typedef unsigned char uint8_t;
/*!
 *  @brief uint32_t as per C99
 */
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;

/*!
 *  @brief uint64_t as per C9900
 */
typedef unsigned long long uint64_t;

typedef unsigned long size_t;

typedef unsigned long uintptr_t;

#if !defined NULL

#define NULL	((void*) 0)

#endif

#if !defined MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

#endif
