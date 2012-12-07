//
//  syscall.c
//  BakingPi
//
//  Created by Jeremy Pereira on 06/12/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "syscall.h"
#include "thread.h"
#include "console.h"

/*
 *  System call dispatch table entry.  If the size of it is a power of 2, it 
 *  will be faster than otherwise.
 */
struct DispatchTableEntry
{
    uintptr_t	dispatchFunction;
};

int32_t syscallDispatch(uint32_t trapNumber,
                        uint32_t syscallNumber,
                        void* inBlock,
                        void* outBlock)
{
    if (syscallNumber >= SYSCALL_COUNT)
    {
        // TODO: Set some sort of error code for the thread
        thread_cancel(thread_currentThread());
    }
    Console* console = con_getTheConsole();
    con_newLine(console);
    con_putCString(console, "Syscall: trap number = ");
    con_putHex32(console, trapNumber);
    con_putCString(console, ", syscall number = ");
    con_putHex32(console, syscallNumber);
    con_putCString(console, ", in block = ");
    con_putHex32(console, (uint32_t) inBlock);
    con_putCString(console, ", out block = ");
    con_putHex32(console, (uint32_t) outBlock);
    con_newLine(console);
    return 0;
}

