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
    uint32_t ordinal;
    SyscallFunction	dispatchFunction;
};

typedef struct DispatchTableEntry DispatchTableEntry;

static int32_t ping(void* inBlock, void* outBlock);
static int32_t reschedule(void* inBlock, void* outBlock);

DispatchTableEntry dispatchTable[] =
{
    { SYSCALL_RESCHEDULE, reschedule },
    { SYSCALL_PING, ping },
    
    { SYSCALL_COUNT, NULL }
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
    return dispatchTable[syscallNumber].dispatchFunction(inBlock, outBlock);
}

int32_t ping(void* inBlock, void* outBlock)
{
    Console* console = con_getTheConsole();
    con_newLine(console);
    con_putCString(console, "Ping: ");
    con_putCString(console, "in block = ");
    con_putHex32(console, (uint32_t) inBlock);
    con_putCString(console, ", out block = ");
    con_putHex32(console, (uint32_t) outBlock);
    con_newLine(console);
    return 0;
}

int32_t reschedule(void* inBlock, void* outBlock)
{
    thread_reschedule();
    return 0;
}


