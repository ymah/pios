//
//  JPPPiController.h
//  BakingPi
//
//  Created by Jeremy Pereira on 01/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JPPHardwareThread.h"

@interface JPPPiController : NSObject <JPPHWThreadDelegate>

@property (nonatomic, weak) IBOutlet NSLevelIndicator* okLight;
@property (nonatomic, weak) IBOutlet NSLevelIndicator* powerLight;
@property (nonatomic, weak) IBOutlet NSSegmentedControl* onOffSwitch;

-(IBAction) turnOnOrOff:(id)sender;

@end
