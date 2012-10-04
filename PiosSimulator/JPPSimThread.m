//
//  JPPHardwareThread.m
//  BakingPi
//
//  Created by Jeremy Pereira on 01/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import "JPPSimThread.h"

#import "PhysicalMemoryMap.h"
#import "SystemTimer.h"


@interface JPPSimThread ()

@property (readonly, strong) NSThread* parentThread;

-(void) notifyFinished;
-(void) notifyStarted;
-(void) notifyUpdate;

@end


@implementation JPPSimThread
{
@private
    NSThread* parentThread_;
}

@synthesize parentThread = parentThread_;

-(id) init
{
    self = [super init];
    if (self != nil)
    {
        parentThread_ = [NSThread currentThread];
        [self setName: [self className]];
    }
    return self;
}


-(void) main
{
    @autoreleasepool
    {
        pmm_init();
        @try
        {
            [self performSelector: @selector(notifyStarted)
                         onThread: [self parentThread]
                       withObject: nil
                    waitUntilDone: NO];
            
            [self simThreadMain];
        }
        @catch (NSException* exception)
        {
            NSLog(@"Thread threw exception %@", exception);
        }
        @finally
        {
            [self performSelector: @selector(notifyFinished)
                         onThread: [self parentThread]
                       withObject: nil
                    waitUntilDone: YES];
        }
    }
}

-(void) notifyStarted
{
    [[self delegate] hasStarted: self];
}

-(void) notifyFinished
{
    [[self delegate] hasFinished: self];
}


-(void) notifyUpdate
{
    [[self delegate] hasBeenUpdated: self];
}


-(void) simThreadMain
{
    // Do nothing
}


@end

@implementation JPPHardwareThread

-(void) simThreadMain
{
    uint64_t iterations = 0;
    PhysicalMemoryMap* memoryMap = pmm_getPhysicalMemoryMap();
    while (![self isCancelled])
    {
        st_microsecondTick(pmm_getSystemTimerAddress(memoryMap));
        if (gpio_outputPinsHaveChanged(pmm_getGPIOAddress(memoryMap)))
        {
            [self performSelector: @selector(notifyUpdate)
                         onThread: [self parentThread]
                       withObject: nil
                    waitUntilDone: YES];
        }
        iterations++;
#if 0
        [NSThread sleepForTimeInterval: 1.0/4000000];
        if (iterations % 1000000 == 0)
        {
            NSLog(@"Thread %@ still running, iterations %lld",
                  self,
                  (long long) iterations);
        }
#endif
    }
}

@end

int piosMain(int argc, char** argv);

static char* simulatorArgs[] = { "simulator", NULL };

@implementation JPPSoftwareThread

-(void) simThreadMain
{
    piosMain(1, simulatorArgs);
}

-(void) cancel
{
    [super cancel];
    pmm_setStopFlag(pmm_getPhysicalMemoryMap(), true);
}
@end
