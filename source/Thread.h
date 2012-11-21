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

#endif
