//
//  JPPPiSimulator.m
//  BakingPi
//
//  Created by Jeremy Pereira on 02/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import "JPPPiSimulator.h"
#import "PhysicalMemoryMap.h"
#import "gpio.h"

@interface JPPPiSimulator ()

@property (nonatomic, assign) bool powerLED;
@property (nonatomic, strong) JPPHardwareThread* hardwareThread;
@property (nonatomic, strong) JPPSoftwareThread* softwareThread;

-(void) notifyUIUpdate;

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
    [self notifyUIUpdate];
}

-(bool) okLED
{
    return ![self gpioPin: SIMULATOR_OK_LED_PIN] && [self powerLED];
}

-(bool) gpioPin: (uint32_t) pinNumber
{
    return gpio_outputPinValue(pmm_getGPIOAddress(pmm_getPhysicalMemoryMap()),
                               pinNumber);
}

-(void) notifyUIUpdate
{
    dispatch_async(dispatch_get_main_queue(),
       ^{
           [[self delegate] updateUIWithSimulator: self];
       });
}


-(void) powerOff
{
    [[self hardwareThread] cancel];
    [[self softwareThread] cancel];
}

-(void) powerOn
{
    [self setHardwareThread: [[JPPHardwareThread alloc] init]];
    [[self hardwareThread] setDelegate: self];
    [[self hardwareThread] start];
    [self setSoftwareThread: [[JPPSoftwareThread alloc] init]];
    [[self softwareThread] setDelegate: self];
    [[self softwareThread] start];
}

#pragma mark JPPHardwareThreadDelegate

-(void) hasStarted: (JPPHardwareThread*) startedThread
{
    NSLog(@"Thread %@ has started", startedThread);

    if (startedThread == [self hardwareThread])
    {
        [self setPowerLED: true];
    }
}

-(void) hasFinished: (JPPHardwareThread*) finishedThread
{
    if (finishedThread == [self hardwareThread])
    {
        [self setPowerLED: false];
    }
    NSLog(@"Thread %@ has exited", finishedThread);
}

-(void) hasBeenUpdated: (JPPSimThread*)updatedThread
{
    [[self delegate] updateUIWithSimulator: self];
}

@end
