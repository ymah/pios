//
//  Thread.c
//  BakingPi
//
//  Created by Jeremy Pereira on 21/11/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "Thread.h"


enum
{
    NUM_REGISTERS = 16,
    MAX_THREADS = 32,
};

static const char* registerNames[NUM_REGISTERS] =
{
    " r0", " r1", " r2", " r3", " r4", " r5", " r6", " r7",
    " r8", " r9", "r10", "r11", "r12", " sp", " lr", " pc"
};

typedef uint32_t Register;

struct Thread
{
    Register savedRegs[NUM_REGISTERS];
};

static Thread allThreads[MAX_THREADS] = { { {0} } };

static Thread* currentThread = NULL;

extern void saveRegs(Register* registerBlock);

unsigned int thread_numRegs(void)
{
    return NUM_REGISTERS;
}

const char* thread_registerNameAsCString(unsigned int index)
{
    const char* ret = "???";
    if (index < NUM_REGISTERS)
    {
        ret = registerNames[index];
    }
    return ret;
}

void thread_initialise(void)
{
    currentThread = &allThreads[0];
}

Thread* thread_currentThread(void)
{
    return currentThread;
}

void thread_saveRegs(Thread* threadToSaveIn)
{
    saveRegs(&threadToSaveIn->savedRegs[0]);
}

uint32_t thread_savedRegister32(Thread* aThread, unsigned int index)
{
    uint32_t ret = 0;
    if (index < NUM_REGISTERS)
    {
        ret = aThread->savedRegs[index];
    }
    return ret;
}

#if defined PIOS_SIMULATOR

/*
 *  A dummy register save routine
 */
void saveRegs(Register* registerBlock)
{
    for (int i = 0 ; i < NUM_REGISTERS ; ++i)
    {
        registerBlock[i] = i;
    }
}
#endif
