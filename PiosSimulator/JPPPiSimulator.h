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

@end
