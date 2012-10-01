//
//  JPPHardwareThread.m
//  BakingPi
//
//  Created by Jeremy Pereira on 01/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import "JPPHardwareThread.h"

@interface JPPHardwareThread ()

-(void) notifyFinished;
-(void) notifyStarted;

@end

@implementation JPPHardwareThread
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
        @try
        {
            [self performSelector: @selector(notifyStarted)
                         onThread: parentThread
                       withObject: nil
                    waitUntilDone: NO];

            while (![self isCancelled])
            {
                [NSThread sleepForTimeInterval: 1.0];
                NSLog(@"Thread %@ still running", self);
            }
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

@end
