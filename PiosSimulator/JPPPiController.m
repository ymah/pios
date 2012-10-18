//
//  JPPPiController.m
//  BakingPi
//
//  Created by Jeremy Pereira on 01/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import "JPPPiController.h"
#import "JPPiScreenView.h"

@interface JPPPiController ()

-(JPPPiSimulator*) simulator;

@end

static JPPPiSimulator* simulator = nil;

@implementation JPPPiController

-(JPPPiSimulator*) simulator
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken,
    ^{
        simulator = [[JPPPiSimulator alloc] init];
    });
    return simulator;
}

-(IBAction) turnOnOrOff:(id)sender
{
    if ([[self onOffSwitch] selectedSegment] == 0)
    {
        [[self simulator] powerOff];
    }
    else
    {
        [[self simulator] setDelegate: self];
        [[self simulator] powerOn];
    }
}

#pragma mark JPPPiSimulatorDelegate methods

-(void) updateUIWithSimulator: (JPPPiSimulator*) aSimulator
{
    [[self screenView] updateWithSimulator: aSimulator];
    [[self powerLight] setIntValue: [[self simulator] powerLED] ? 1 : 0];
    [[self okLight] setIntValue: [[self simulator] okLED] ? 1 : 0];
}

@end
