//
//  JPPHardwareThread.h
//  BakingPi
//
//  Created by Jeremy Pereira on 01/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import <Foundation/Foundation.h>

@class JPPHardwareThread;

/*!
 * @brief Delegate for a hardware thread.  This object is informed of major 
 * state changes such as the thread stopping.
 * All methods in the delegate are executed on the same thread that created the
 * hardware thread.  This means that, if you wish to do UI updated, the 
 * hardware thread should be created on the main thread.
 */
@protocol JPPHWThreadDelegate <NSObject>

/*!
 *  @brief sent when the thread has initialised and is running its update loop.
 *  @param startedThread The thread that sent the message.
 */
-(void) hasStarted: (JPPHardwareThread*) startedThread;
/*!
 *  @brief sent when the thread has finished.
 *  @param finishedThread The thread that sent the message.
 */
-(void) hasFinished: (JPPHardwareThread*) finishedThread;

@end

/*!
 *  @brief Thread that emulates Raspberry Pi hardware.
 *  An object of ythis class encapsulates all of the hardware that forms part
 *  of the Rspberry Pi, including GPIO, Timer, GPU postbox and other.
 */
@interface JPPHardwareThread : NSThread

/*!
 * @brief Delegate for a hardware thread.  This object is informed of major
 * state changes such as the thread stopping.
 * All methods in the delegate are executed on the same thread that created the
 * hardware thread.  This means that, if you wish to do UI updated, the
 * hardware thread should be created on the main thread.
 */
@property (nonatomic, weak) id<JPPHWThreadDelegate> delegate;

@end
