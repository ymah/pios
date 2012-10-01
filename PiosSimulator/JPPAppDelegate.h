//
//  JPPAppDelegate.h
//  PiosSimulator
//
//  Created by Jeremy Pereira on 01/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "JPPPiController.h"

@interface JPPAppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;

@property (weak) IBOutlet JPPPiController* piController;

@end
