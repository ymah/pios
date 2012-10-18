//
//  JPPHardwareThread.h
//  BakingPi
//
//  Created by Jeremy Pereira on 01/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import <Foundation/Foundation.h>

@class JPPSimThread;

/*!
 * @brief Delegate for a hardware thread.  This object is informed of major 
 * state changes such as the thread stopping.
 * All methods in the delegate are executed on the new thred.  This means that, 
 * if you wish to do UI updated, the delegate method should arrange for the 
 * UI updates to happen on the main thread.
 */
@protocol JPPThreadDelegate <NSObject>

/*!
 *  @brief sent when the thread has initialised and is running its update loop.
 *  @param startedThread The thread that sent the message.
 */
-(void) hasStarted: (JPPSimThread*) startedThread;
/*!
 *  @brief sent when the thread has finished.
 *  @param finishedThread The thread that sent the message.
 */
-(void) hasFinished: (JPPSimThread*) finishedThread;

/*!
 *  @brief sent when the hardware has been updatd.
 *
 *  This is sent synchronously to avoid swamping the run loop of the main 
 *  thread.
 *  @param updatedThread Thread that has been updated.
 */
-(void) hasBeenUpdated: (JPPSimThread*) updatedThread;

@end

/*!
 *  @brief Superclass of simulator threads that can have a delegate.
 *  
 *  Subclasses should override -simThreadMain to do work.  
 *  Delegate methods are sent on the new thread 
 */
@interface JPPSimThread : NSThread

/*!
 *  @brief Override to do some work in the thread.
 *
 *  This message is sent in the context of already having an autorelease pool
 *  and the thread will already have sent -startedThread and will have called
 *  pmm_init().  
 *  -finishedThread will be executed after this method returns, even if an 
 *  exception is thrown and not caught.
 */
-(void) simThreadMain;

/*!
 * @brief Delegate for a hardware thread.  This object is informed of major
 * state changes such as the thread stopping.
 * All methods in the delegate are executed on the same thread that created the
 * hardware thread.  This means that, if you wish to do UI updated, the
 * hardware thread should be created on the main thread.
 */
@property (nonatomic, weak) id<JPPThreadDelegate> delegate;

@end

/*!
 *  @brief Thread that emulates Raspberry Pi hardware.
 *  An object of ythis class encapsulates all of the hardware that forms part
 *  of the Rspberry Pi, including GPIO, Timer, GPU postbox and other.
 */
@interface JPPHardwareThread : JPPSimThread

/*!
 *  @brief Determine the latest set of scan lines to have been updated.
 *  @param linesToUpdateRef An NSRange that will be filled in with the first
 *  scan line updated and the number of lines updated.
 *  @return true if the lines updated have changed since the last time this
 *  method was invoked.
 */
-(bool) scanLinesUpdated: (NSRange*) linesToUpdateRef;

-(bool) getScanLine: (const uint8_t**) scanLinePtr
		 pixelBytes: (uint8_t*) pixelBytes
       colourDepths: (uint8_t*) colourDepths
          lineBytes: (size_t*) lineBytes
      scanLineIndex: (size_t) scanLineIndex;

@end
/*!
 *  @brief Thread that emulates Raspberry Pi CPU thread of execution.
 */
@interface JPPSoftwareThread : JPPSimThread
@end
