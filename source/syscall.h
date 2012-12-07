//
//  syscall.h
//  BakingPi
//
//  Created by Jeremy Pereira on 06/12/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_syscall_h
#define BakingPi_syscall_h

#include "bptypes.h"

enum SysCallOrdinal
{
    SYSCALL_PING = 0,
    /*!
     * @brief count of system calls
     */
    SYSCALL_COUNT
};

extern int32_t syscall(uint32_t syscallNumber,
                       void* inBlock,
                       void* outBlock);

int32_t syscallDispatch(uint32_t trapNumber,
                        uint32_t syscallNumber,
                        void* inBlock,
                        void* outBlock);

#define syscall_ping()	syscall(SYSCALL_PING, NULL, NULL);

#endif
