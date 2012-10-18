//
//  JPPiScreenView.m
//  BakingPi
//
//  Created by Jeremy Pereira on 16/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import "JPPiScreenView.h"
#import "JPPPiSimulator.h"

@interface JPPiScreenView ()

@property (strong) JPPPiSimulator* currentSimulator;

@end

@implementation JPPiScreenView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self != nil)
    {
        // Initialization code here.
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    JPPPiSimulator* theSimulator = [self currentSimulator];
    
    NSEraseRect(dirtyRect);
    
    CGContextRef theContext = [[NSGraphicsContext currentContext] graphicsPort];
    CGContextSetRGBFillColor(theContext, 0, 0, 0, 1);
    CGRect theRect;
    theRect.origin.x = 0;
    theRect.origin.y = dirtyRect.origin.y;
    theRect.size.height = 1;
    theRect.size.width = 1;
    
    const uint8_t* scanLinePtr;
    uint8_t pixelBytes;
    uint8_t colourDepths[4];
    size_t lineBytes;
    size_t numScanLines = dirtyRect.size.height;
    
    size_t startScanLine = theRect.origin.y;
    for (size_t scanLineIndex = 0 ; scanLineIndex < numScanLines ; ++scanLineIndex)
    {
        theRect.origin.y = scanLineIndex + dirtyRect.origin.y;
        
        if ([theSimulator getScanLine: &scanLinePtr
                           pixelBytes: &pixelBytes
                     	 colourDepths: colourDepths
                        	lineBytes: &lineBytes
                    	scanLineIndex: scanLineIndex + startScanLine])
        {
            for (size_t x = 0 ; x < lineBytes / pixelBytes ; ++x)
            {
                uint32_t rawColour;
                if (pixelBytes == 2)
                {
                    /*
                     *  16 bit colour
                     */
                    rawColour = *(uint16_t*) scanLinePtr;
                }
                else if (pixelBytes == 4)
                {
                    /*
                     *  32 bit colour
                     */
                    rawColour = *(uint32_t*) scanLinePtr;
                }
                else
                {
                    rawColour = 0;
                }
        		scanLinePtr += pixelBytes;
        		/*
                 *  We assume RGBA with alpha in the least significant bits
                 */
                CGFloat red = 0;
                CGFloat green = 0;
                CGFloat blue = 0;
                CGFloat alpha = 1;
                
        		uint32_t maxValue = (1 << colourDepths[CDI_ALPHA]) - 1;
                uint32_t mask = ~(0xFFFFFFFF << colourDepths[CDI_ALPHA]);
        		uint32_t value = rawColour & mask;
                if (maxValue > 0)
                {
                    alpha = (CGFloat) value / (CGFloat) maxValue;                    
                }
                rawColour >>= colourDepths[CDI_ALPHA];
                
        		maxValue = (1 << colourDepths[CDI_BLUE]) - 1;
                mask = ~(0xFFFFFFFF << colourDepths[CDI_BLUE]);
        		value = rawColour & mask;
                if (maxValue > 0)
                {
                    blue = (CGFloat) value / (CGFloat) maxValue;
                }
                rawColour >>= colourDepths[CDI_BLUE];
                
        		maxValue = (1 << colourDepths[CDI_GREEN]) - 1;
                mask = ~(0xFFFFFFFF << colourDepths[CDI_GREEN]);
        		value = rawColour & mask;
                if (maxValue > 0)
                {
                    green = (CGFloat) value / (CGFloat) maxValue;                    
                }
                rawColour >>= colourDepths[CDI_GREEN];
                
        		maxValue = (1 << colourDepths[CDI_RED]) - 1;
                mask = ~(0xFFFFFFFF << colourDepths[CDI_RED]);
        		value = rawColour & mask;
                if (maxValue > 0)
                {
                    red = (CGFloat) value / (CGFloat) maxValue;                    
                }
                
                CGContextSetRGBFillColor(theContext, red, green, blue, alpha);
				theRect.origin.x = x;
                CGContextFillRect(theContext, theRect);
            }
        }
    }    
}

-(BOOL) isFlipped
{
    return YES;
}

-(void) updateWithSimulator: (JPPPiSimulator*) aSimulator
{
    NSRange scanLineRange;
    if ([aSimulator scanLinesUpdated: &scanLineRange])
    {
        NSRect needsDisplayRect;
        needsDisplayRect.origin.x = 0;
        needsDisplayRect.origin.y = scanLineRange.location;
        needsDisplayRect.size.width = [self frame].size.width;
        needsDisplayRect.size.height = scanLineRange.length;
        [self setCurrentSimulator: aSimulator];
        [self setNeedsDisplayInRect: needsDisplayRect];
    }

}

@end
