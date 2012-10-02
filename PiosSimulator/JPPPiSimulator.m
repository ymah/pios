//
//  JPPPiSimulator.m
//  BakingPi
//
//  Created by Jeremy Pereira on 02/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import "JPPPiSimulator.h"

@interface JPPPiSimulator ()

@property (nonatomic, assign) bool powerLED;
@property (nonatomic, strong) JPPHardwareThread* hardwareThread;

@end

@implementation JPPPiSimulator
{
@private
    bool powerLED_;
}

@synthesize powerLED = powerLED_;

-(void) setPowerLED: (bool) powerLED
{
    powerLED_ = powerLED;
    dispatch_async(dispatch_get_main_queue(),
    ^{
        [[self delegate] updateUIWithSimulator: self];
    });
}

-(void) powerOff
{
    [[self hardwareThread] cancel];
}

-(void) powerOn
{
    [self setHardwareThread: [[JPPHardwareThread alloc] init]];
    [[self hardwareThread] setDelegate: self];
    [[self hardwareThread] start];
}

#pragma mark JPPHardwareThreadDelegate

-(void) hasStarted: (JPPHardwareThread*) startedThread
{
    [self setPowerLED: true];
}

-(void) hasFinished: (JPPHardwareThread*) finishedThread
{
    [self setPowerLED: false];
}

@end
