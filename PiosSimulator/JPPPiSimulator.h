//
//  JPPPiSimulator.h
//  BakingPi
//
//  Created by Jeremy Pereira on 02/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JPPSimThread.h"

enum
{
    SIMULATOR_OK_LED_PIN = 16,
};

/*!
 *  @brief Colour depth index for colour depths in 
 *  -getScanLine:pixelBytes:colourDepths:lineBytes:lineBytes
 */
enum ColourDepthIndex
{
    CDI_ALPHA 	= 0,
    CDI_BLUE 	= 1,
    CDI_GREEN 	= 2,
    CDI_RED 	= 3,
};


@class JPPPiSimulator;

/*!
 *  @brief protocol to which the delegate must confoem
 */
@protocol JPPPiSimulatorDelegate <NSObject>

/*!
 *  @brief Message sent when the simulator needs to notify the delegate that
 *  user interface changes are needed.  This message will always be sent on the
 *  main thread.
 *  @param aSimulator simulator that needs updating
 */
-(void) updateUIWithSimulator: (JPPPiSimulator*) aSimulator;

@end

/*!
 *  @brief A Raspberry Pi hardware simulator.
 */
@interface JPPPiSimulator : NSObject <JPPThreadDelegate>

@property (nonatomic, weak) id<JPPPiSimulatorDelegate> delegate;

-(void) powerOn;
-(void) powerOff;

@property (nonatomic, readonly, assign) bool powerLED;
@property (nonatomic, readonly, assign) bool okLED;

-(bool) gpioPin: (uint32_t) pinNumber;

/*!
 *  @brief Determine the latest set of scan lines to have been updated.
 *  @param linesToUpdateRef An NSRange that will be filled in with the first 
 *  scan line updated and the number of lines updated.
 *  @return true if the lines updated have changed since the last time this 
 *  method was invoked.
 */
-(bool) scanLinesUpdated: (NSRange*) linesToUpdateRef;

/*!
 *  @brief get a pointer to the buffer containing a scan line of the frame buffer.
 *
 *  We only handle RGB formats at the moment and each pixle must be a whole 
 *  number of bytes.  Each of the filled in pointers may be set to NULL if you
 *  are not interested in the returned value.
 *  @param scanLinePtr Will be set to the start of the buffer representing the 
 *  scan line
 *  @param pixelBytes Filled in with number of bytes per pixel.
 *  @param colourDepths An array of four uint8_ts that will be filled in with
 *  the number of bits per colour.  The colours will be in RGBA order.
 *  @param lineBytes Will be filled in with the number of bytes in the scan line.
 *  @param scanLineIndex he index of the scan line to fetch.
 *  @return true if scanLineIndex is within the range of the frame buffer, 
 *  if false, the other reference parameters will not be filled in.
 */
-(bool) getScanLine: (const uint8_t**) scanLinePtr
		 pixelBytes: (uint8_t*) pixelBytes
       colourDepths: (uint8_t*) colourDepths
          lineBytes: (size_t*) lineBytes
      scanLineIndex: (size_t) scanLineIndex;
@end
