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
    SYSCALL_RESCHEDULE 	= 0,
    SYSCALL_PING 		= 1,
    /*!
     * @brief count of system calls
     */
    SYSCALL_COUNT
};

/*!
 *  @brief Make a system call.
 *
 *  This function makes a system call.  It drops the CPU into supervisor mode
 *  and executes the given system call.
 *  @param syscallNumber The system call number.
 *  @param inBlock Block of input arguments.
 *  @param outBlock Block of output arguments.
 *  @return An error code to indicate how it all went.
 */
extern int32_t syscall(uint32_t syscallNumber,
                       void* inBlock,
                       void* outBlock);

/*!
 *  @brief Dispatch a system call.
 *
 *  This function dispatches the system call.  It's assumed that we are already 
 *  in supervisor mode.
 *  @param trapNumber The SWI trap number.
 *  @param syscallNumber The system call number.
 *  @param inBlock Block of input arguments.
 *  @param outBlock Block of output arguments.
 *  @return An error code to indicate how it all went.
 */
int32_t syscallDispatch(uint32_t trapNumber,
                        uint32_t syscallNumber,
                        void* inBlock,
                        void* outBlock);

/*!
 *  @brief System call that just registers it has happened.  Used for debugging
 *  the system call mechanism.
 *	@return system call error code.
 */
#define syscall_ping()			(syscall(SYSCALL_PING, NULL, NULL))
/*!
 *  @brief Yields the processor to another thread.
 *  @return system call error code.
 */
#define syscall_reschedule()	(syscall(SYSCALL_RESCHEDULE, NULL, NULL))

typedef int32_t (*SyscallFunction)(void* inBlock, void* outBlock);

#endif
