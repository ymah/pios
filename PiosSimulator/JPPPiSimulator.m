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

/*
 *  We need some storage to emulate the free heap pages.  We have a uint8_t
 *  array that pretends to be the top of the area allocated to the 
 *
 *  Alignment is not important because the physical memory manager adjusts the 
 *  pointers it uses to be page aligned.
 *
 */
uint8_t heap[4096 * 32];	// Allocate 31 or 32 pages depending on alignment.


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
    [[self tags] addCoreFlags: 1 pageSize: 4096 rootDev: 0];
    [[self tags] addMemorySize: (uintptr_t)(heap + sizeof heap) start: 0x0000];
    [[self tags] addVideoTextX: 80
                             y: 25
                          page: 1
                          mode: 1
                          cols: 80
                         egaBx: 1
                         lines: 25
                         isVGA: 1
                        points: 800];
    [[self tags] addRAMDiskFlags: 1 size: 1024 start: 1024];
    [[self tags] addCommandLine: @"Hello, World\nGoodbye"];
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
