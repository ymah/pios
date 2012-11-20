//
//  SystemTimer+Driver.h
//  BakingPi
//
//  Created by Jeremy Pereira on 20/11/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_SystemTimer_Driver_h
#define BakingPi_SystemTimer_Driver_h

#include "SystemTimer.h"

struct STDriver
{
    /*!
     *  @brief allocate enough memory to contain a SystemTimer struct of the 
     *  relevant subclass.
     *  @return a System timer struct + subclass.
     */
    SystemTimer* (*allocate)(void);
    /*!
     *  @brief initialise the system timer including hardware initialisation.
     *  @param timer The timer to initialise.
     *  @return The initialised timer
     */
    SystemTimer* (*init)(SystemTimer* timer);
    /*!
     *  @brief deallocate a timer struct
     */
    void (*dealloc)(SystemTimer* timer);
    
    /*!
     *  @brief Spin for the given number of microseconds.
     *  @param timer the timer to spin on.
     *  @param microseconds The number of microseconds to spin for.
     */
    void (*microsecondSpin)(SystemTimer* timer, uint32_t microseconds);
    
    /*!
     *  @brief Return the number of microseconds since start-up.
     *  @param timer The timer to check.
     *  @return Number of microseconds since start up.
     */
    uint64_t (*microseconds)(SystemTimer* timer);

};

/*!
 *  @brief System timer struct.
 */
struct SystemTimer
{
    /*!
     *  @brief The driver for this system timer.
     */
    STDriver* driver;
};


#endif
