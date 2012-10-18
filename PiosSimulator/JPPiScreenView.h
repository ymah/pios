//
//  JPPiScreenView.h
//  BakingPi
//
//  Created by Jeremy Pereira on 16/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class JPPPiSimulator;

@interface JPPiScreenView : NSView

-(void) updateWithSimulator: (JPPPiSimulator*) aSimulator;

@end
