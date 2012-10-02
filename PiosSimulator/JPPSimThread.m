//
//  JPPHardwareThread.m
//  BakingPi
//
//  Created by Jeremy Pereira on 01/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import "JPPSimThread.h"

#import "PhysicalMemoryMap.h"


@interface JPPSimThread ()

-(void) notifyFinished;
-(void) notifyStarted;

@end


@implementation JPPSimThread
{
@private
    NSThread* parentThread;
}


-(id) init
{
    self = [super init];
    if (self != nil)
    {
        parentThread = [NSThread currentThread];
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
                         onThread: parentThread
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
                         onThread: parentThread
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

-(void) simThreadMain
{
    // Do nothing
}


@end

@implementation JPPHardwareThread

-(void) simThreadMain
{
    while (![self isCancelled])
    {
        [NSThread sleepForTimeInterval: 1.0];
        NSLog(@"Thread %@ still running", self);
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

@end
