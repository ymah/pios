//
//  Thread.h
//  BakingPi
//
//  Created by Jeremy Pereira on 21/11/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_Thread_h
#define BakingPi_Thread_h

#include "bptypes.h"

enum ThreadErr
{
    /*!
     *  @brief No error
     */
    THREAD_ERR_OK = 0,
    /*!
     *  @brief Operation attempted when the thread is in the wrong state for the
     *  operation.
     */
    THREAD_ERR_INVALID_STATE = 1,
};

/*!
 *  @brief Thread states.
 *  Each thread starts in the THREAD_CREATED state.  States are:
 *  UNUSED   => thread_create()     => CREATED
 *  CREATED  => thread_start()      => READY
 *           => thread_cancel()     => FINISHED
 *  READY    => thread_cancel()     => FINISHED
 *           => thread_reschedule() => RUNNING
 *  FINISHED => deallocate          => UNUSED
 */
enum ThreadState
{
    /*!
     *  @brief This thread is not in use and its memory can be reused.
     */
    THREAD_UNUSED = 0,
    /*!
     *  @brief The thread has been created .
     */
    THREAD_CREATED,
    /*!
     *  @brief The thread is ready to run.
     */
    THREAD_READY,
    /*!
     *  @brief The thread has finished running.
     */
    THREAD_FINISHED,
};

typedef enum ThreadState ThreadState;;

typedef enum ThreadErr ThreadErr;

/*!
 *  @brief Opaque type that represents a thread.
 */
struct Thread;
typedef struct Thread Thread;

/*!
 *  @brief Set up the initial thread.
 */
void thread_initialise(void);

/*!
 * @brief Returns the number of registers that are saved in a thread.
 * @return the number of registers saved in a thread.
 */
unsigned int thread_numRegs(void);

/*!
 *  @brief Retiurns the name of the register given by the index.  This is used 
 *  by diagnostic routines that, for instance, print the register state to the
 *  console.
 *  @param index index of the register to name.
 *  @return The name of the register as a conventional C String
 */
const char* thread_registerNameAsCString(unsigned int index);

/*!
 *  @brief Return the lowest 32 bits in the saved register given by the index.
 *  @param aThread Thread to get the saved register value from.
 *  @param index Index of register to get the register value from.
 *  @return  The lowest 32 bits of the given register.
 */
uint32_t thread_savedRegister32(Thread* aThread, unsigned int index);

/*!
 *  @brief Get the current thread.
 *  @return The current thread.
 */
Thread* thread_currentThread(void);

/*!
 *  @brief Save the current state of the CPU registers.
 *  This method saves everything that would be needed for a context switch.
 *  @param threadYoSaveIn The thread object to save the register state in.
 */
void thread_saveRegs(Thread* threadToSaveIn);

/*!
 *  @brief Give the OS a chance to make a context switch.
 */
void thread_reschedule(void);

/*!
 *  @brief Typedef for a thread function.
 *  @param context Pointer to context to pass as a parameter to the function 
 *  when the thread starts.
 */
typedef void (*ThreadMain)(void* context);

/*!
 *  @brief Create a new thread.
 *  This function can only be called from the main thread.  The returned thread
 *  has a reference count of 1.
 *  @param threadStart The function to run when the thread starts.
 *  @return A new ready to run thread or NULL if we fail to create the thread.
 */
Thread* thread_create(ThreadMain threadStart);

/*!
 *  @brief Retain the given thread
 *  May only be called from the main thread.
 *  @param thread The thread to retain
 *  @return The thread
 */
Thread* thread_retain(Thread* thread);

/*!
 *  @brief Release the given thread because we are no longer interested in it.
 *  May only be called from the main thread.
 *  @param thread The thread to release.
 */
void thread_release(Thread* thread);

/*!
 *  @brief Start a thread.
 *  @param thread The thread to start.
 *  @param context A pointer to some data that will be passed to the thread's 
 *  main function.
 *  @return An error code or THREAD_OK if the thread started OK.
 */
ThreadErr thread_start(Thread* thread, void* context);
/*!
 *  @brief Cancel the given thread.  
 *  If the current thread is the main thread, the thread will go away straight
 *  away.  If not, it will go away the next time it is attempted to be 
 *  rescheduled.
 *  @param thread The thread tp cancel.
 *  @return THREAD_ERR_OK unless the thread is already dead.
 */ 
ThreadErr thread_cancel(Thread* thread);

/*!
 *  @brief Get the state of the given thread.
 *  @param thread Thread to get the state of.
 *  @return the state of the thread.
 */
ThreadState thread_state(Thread* thread);

#endif
