//
//  klib.c
//  BakingPi
//
//  Created by Jeremy Pereira on 10/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "klib.h"

void* klib_memcpy(void *restrict s1, const void *restrict s2, size_t n)
{
    for (size_t i = 0 ; i < n ; ++i)
    {
        ((uint8_t*)s1)[i] = ((uint8_t*)s2)[i];
    }
    return s1;
}

void* klib_memset(void* restrict ptr, uint8_t value, size_t length)
{
    for (size_t i = 0 ; i < length ; ++i)
    {
        ((uint8_t*) ptr)[i] = value;
    }
    return ptr;
}


