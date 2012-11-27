//
//  Thread.c
//  BakingPi
//
//  Created by Jeremy Pereira on 21/11/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "Thread.h"
#include "klib.h"
#include "PhysicalMemoryMap.h"
#include "console.h"


enum
{
    NUM_REGISTERS = 16,
    STACK_REG = 13,
    LINK_REG = 14,
    PC_REG = 15,
    FIRST_CONTEXT_REG = STACK_REG,
    MAX_THREADS = 32,
    STACK_PAGES = 4,
};

static const char* registerNames[NUM_REGISTERS] =
{
    " r0", " r1", " r2", " r3", " r4", " r5", " r6", " r7",
    " r8", " r9", "r10", "r11", "r12", "sp", "lr", "pc"
};

typedef uint32_t Register;

struct Thread
{
    ThreadState state;
    Register savedRegs[NUM_REGISTERS];
    ThreadMain mainFunction;
    unsigned int refCount;
    size_t stackSize;
    uint8_t* stack;
    Thread* prev;
    Thread* next;
    struct ThreadQueue* queue;
};

static Thread allThreads[MAX_THREADS] = { { 0, {0}, NULL, 0, 0, NULL, NULL, NULL } };

static Thread* currentThread = NULL;

struct ThreadQueue
{
    Thread* head;
    Thread* tail;
};

#define MAIN_THREAD	(&allThreads[0])

typedef struct ThreadQueue ThreadQueue;

static ThreadQueue readyQueue = { NULL, NULL };
static ThreadQueue unusedThreads = { NULL, NULL };

#define QUEUE_ADD_LAST(Q,T)			\
do {								\
	if ((Q)->tail == NULL)			\
	{								\
        (Q)->tail = (T);			\
		(Q)->head = (T);			\
    }								\
	else							\
	{								\
		(T)->prev = (Q)->tail;		\
        (Q)->tail = (T);			\
	}								\
	(T)->queue = (Q);				\
} while(0)

extern void saveRegs(Register* registerBlock);
extern void restoreRegs(Register* registerBlock);
extern void contextSwitch(Register* fromContext, Register* toContext);

static void threadStub(void* context);
static ThreadErr makeReady(Thread* thread);
static ThreadErr dequeue(Thread* thread);
static void removeDeadThreads(void);

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
    klib_memset(allThreads, 0, sizeof allThreads);
    currentThread = &allThreads[0];
    if (MAX_THREADS > 1)
    {
        for (int i = 1 ; i < MAX_THREADS ; ++i)
        {
            QUEUE_ADD_LAST(&unusedThreads, &allThreads[i]);
        }
    }
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

ThreadState thread_state(Thread* thread)
{
    ThreadState ret = THREAD_UNUSED;
    if (thread != NULL)
    {
        ret = thread->state;
    }
    return ret;
}

Thread* thread_create(ThreadMain mainFunction)
{
    Thread* ret = NULL;
    if (thread_currentThread() == &allThreads[0] && unusedThreads.head != NULL)
    {
        void* stack = pmm_allocateContiguousPages(pmm_getPhysicalMemoryMap(),
                                                  STACK_PAGES);
        if (stack != NULL)
        {
            ret = unusedThreads.head;
            dequeue(ret);
            ret->prev = NULL;
            ret->next = NULL;
            ret->refCount = 1;
            ret->mainFunction = mainFunction;
            ret->stack = stack;
            ret->stackSize = STACK_PAGES * PIOS_PAGE_SIZE;
            klib_memset(ret->savedRegs, 0, sizeof ret->savedRegs);
            ret->state = THREAD_CREATED;
        }
    }
    return ret;
}

ThreadErr thread_start(Thread* thread, void* context)
{
    ThreadErr ret = THREAD_ERR_INVALID_STATE;
    
    if (thread->state == THREAD_CREATED)
    {
        /*
         *  Set up the processor state as if threadStub has called in to 
         *  contextSwitch
         */
        thread->savedRegs[STACK_REG] = (Register)(thread->stack + thread->stackSize);
        thread->savedRegs[LINK_REG] = (Register)threadStub;
        thread->savedRegs[0] = (Register)context; // Thread stub has the context as a param
         ret = makeReady(thread);
        if (currentThread == MAIN_THREAD)
        {
            Console* con = con_getTheConsole();
            con_putCString(con, "Starting thread ");
            con_putHex32(con, (uint32_t) thread);
            con_newLine(con);
        }
        
    }
    return ret;
}

ThreadErr thread_cancel(Thread* thread)
{
    ThreadErr ret = THREAD_ERR_INVALID_STATE;

    if (thread->state != THREAD_FINISHED && thread->state != THREAD_UNUSED)
    {
        thread->state = THREAD_FINISHED;
        if (currentThread == MAIN_THREAD)
        {
            dequeue(thread);
            thread_release(thread);
        }
        if (thread == currentThread)
        {
            thread_reschedule();
        }
        ret = THREAD_ERR_OK;
    }
    return ret;
}

void thread_release(Thread* thread)
{
    KLIB_ASSERT(currentThread == MAIN_THREAD,
                "Only the main thread is allowed to manipulate thread queues");

    if (currentThread == MAIN_THREAD)
    {
        Console* con = con_getTheConsole();
        con_putCString(con, "Releasing thread ");
        con_putHex32(con, (uint32_t) thread);
        con_putCString(con, ", refCount ");
        con_putDecimal32(con,  thread->refCount, 2);
        con_newLine(con);
    }
    if (thread->refCount == 1)
    {
        thread->state = THREAD_UNUSED;
        thread->refCount = 0;
        pmm_freeContiguousPages(pmm_getPhysicalMemoryMap(),
                                thread->stack,
                                thread->stackSize / PIOS_PAGE_SIZE);
        QUEUE_ADD_LAST(&unusedThreads, thread);
    }
    else if (thread->refCount > 0)
    {
        thread->refCount--;
    }
}

void thread_reschedule(void)
{
    /*
     *  This is a bit confusing because, when we enter the function we are in a
     *  different thread to when we leave.  The rule is that all the mucking 
     *  about with variables and state is done on the main thread.  This
     *  obviates the need for mutexes.  
     *  If we come in on the main thread, we need to set up to switch to the
     *  first thread on the ready queue (getting rid of the finished threads 
     *  first).  If we come in on any other thread, we just do the context 
     *  switch.  
     *  After the context switch, if we were are on the main thread, it means we
     *  need to tidy up after the context switch.
     */
    if (currentThread == MAIN_THREAD)
    {
        removeDeadThreads();
    }
    if (currentThread == MAIN_THREAD && readyQueue.head == NULL)
    {
        // No ready threads, do nothing
    }
    else
    {
        Thread* from = currentThread;
        Thread* to;
        if (currentThread == MAIN_THREAD)
        {
            to = readyQueue.head;
            dequeue(to);
            currentThread = to;
        }
        else
        {
            to = MAIN_THREAD;
        }
        contextSwitch(from->savedRegs, to->savedRegs);
        if (from == MAIN_THREAD)
        {
            QUEUE_ADD_LAST(&readyQueue, currentThread);
            currentThread = MAIN_THREAD;
        }
    }
}



#pragma mark -

void threadStub(void* context)
{
    if (currentThread->mainFunction != NULL)
    {
        currentThread->mainFunction(context);
    }
    thread_cancel(currentThread);		// Will not return from here
}

ThreadErr makeReady(Thread* thread)
{
    KLIB_ASSERT(thread->prev == NULL
                && thread->next == NULL
                && thread->queue == NULL,
                "Thread must not be in a queue");
    KLIB_ASSERT(currentThread == MAIN_THREAD,
                "Only the main thread is allowed to manipulate thread queues");
    
    ThreadErr ret = THREAD_ERR_INVALID_STATE;
    if (thread->state == THREAD_CREATED)
    {
        thread->refCount++;
        
    }
    QUEUE_ADD_LAST(&readyQueue, thread);
    return ret;
}

static ThreadErr dequeue(Thread* thread)
{
    KLIB_ASSERT(currentThread == MAIN_THREAD,
                "Only the main thread is allowed to manipulate thread queues");
    
    if (thread->prev != NULL)
    {
        thread->prev->next = thread->next;
    }
    else
    {
        thread->queue->head = thread->next;
    }
    if (thread->next != NULL)
    {
        thread->next->prev = thread->prev;
    }
    else
    {
        thread->queue->tail = thread->prev;
    }
    thread->queue = NULL;
    thread->prev  = NULL;
    thread->next  = NULL;
    return THREAD_ERR_OK;
}

static void removeDeadThreads(void)
{
    KLIB_ASSERT(currentThread == MAIN_THREAD,
                "Only the main thread is allowed to manipulate thread queues");
	while (readyQueue.head != NULL && readyQueue.head->state == THREAD_FINISHED)
    {
        Thread* theThread = readyQueue.head;
        dequeue(theThread);
        thread_release(theThread);
    }
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
