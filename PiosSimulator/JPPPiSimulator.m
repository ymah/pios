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
#import "JPPiArmTags.h"

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
    JPPiArmTags* tags_;
    NSData* systemFont_;
}

@synthesize powerLED = powerLED_;
@synthesize tags = tags_;

-(id) init
{
    self = [super init];
    if (self != nil)
    {
        tags_ = [[JPPiArmTags alloc] init];
    }
    return self;
}

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

-(bool) scanLinesUpdated: (NSRange*) linesToUpdateRef
{
    return [[self hardwareThread] scanLinesUpdated: linesToUpdateRef];
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
    [[self softwareThread] cancel];
    [[self hardwareThread] cancel];
}

-(void) powerOn
{
    [[self tags] clear];
    [[self tags] addCommandLine: @"Hello, World"];
    [[self tags] addTerminator];
    
    [self setHardwareThread: [[JPPHardwareThread alloc] init]];
    [[self hardwareThread] setDelegate: self];
    [[self hardwareThread] start];
    [self setSoftwareThread: [[JPPSoftwareThread alloc] init]];
    [[self softwareThread] setDelegate: self];
}

-(NSData*) systemFont
{
    @synchronized(self)
    {
        if (systemFont_ == nil)
        {
            NSBundle* thisBundle = [NSBundle bundleForClass: [self class]];
            NSURL* fontUrl = [thisBundle URLForResource: @"font0"
                                          withExtension: @"bin"];
            systemFont_ = [NSData dataWithContentsOfURL: fontUrl];
        }
    }
    return systemFont_;
}

#pragma mark JPPHardwareThreadDelegate

-(JPPPiSimulator*) simulatorForThread: (JPPHardwareThread*) thread;
{
    return self;
}

-(void) hasStarted: (JPPHardwareThread*) startedThread
{
    NSLog(@"Thread %@ has started", startedThread);

    if (startedThread == [self hardwareThread])
    {
        [self setPowerLED: true];
        [self notifyUIUpdate];
        [[self softwareThread] start];
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
    [self notifyUIUpdate];
}

-(bool) getScanLine: (const uint8_t**) scanLinePtr
		 pixelBytes: (uint8_t*) pixelBytes
       colourDepths: (uint8_t*) colourDepths
          lineBytes: (size_t*) lineBytes
      scanLineIndex: (size_t) scanLineIndex
{
    return [[self hardwareThread] getScanLine: scanLinePtr
                                   pixelBytes: pixelBytes
                                 colourDepths: colourDepths
                                    lineBytes: lineBytes
                                scanLineIndex: scanLineIndex];
}


@end
