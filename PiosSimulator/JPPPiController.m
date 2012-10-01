//
//  JPPPiController.m
//  BakingPi
//
//  Created by Jeremy Pereira on 01/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import "JPPPiController.h"

@implementation JPPPiController
{
    JPPHardwareThread* hardware;
}

-(IBAction) turnOnOrOff:(id)sender
{
    if ([[self onOffSwitch] selectedSegment] == 0)
    {
        [hardware cancel];
    }
    else
    {
        hardware = [[JPPHardwareThread alloc] init];
        [hardware setDelegate: self];
        [hardware start];
    }
}

#pragma mark JPPHWDelegate methods

-(void) hasStarted: (JPPHardwareThread*) startedThread
{
    [[self powerLight] setIntValue: 1];
}

-(void) hasFinished:(JPPHardwareThread*) finishedThread
{
    [[self powerLight] setIntValue: 0];
}

@end
