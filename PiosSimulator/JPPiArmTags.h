//
//  JPPiArmTags.h
//  BakingPi
//
//  Created by Jeremy Pereira on 23/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import <Foundation/Foundation.h>

/*!
 *  @brief Encapsulates a set of ARM boot tags
 */
@interface JPPiArmTags : NSObject

/*!
 *  @brief add a Command line to the tags
 *  @param commandLine The command line to add.
 */
-(void) addCommandLine: (NSString*) commandLine;

/*!
 *  @brief Add the terminator tag to the bytes
 */
-(void) addTerminator;

/*!
 *  @brief The bytes for the tags
 */
@property (nonatomic, readonly, assign) const uint8_t* bytes;

/*!
 *  @brief Clear all existing tags
 */
-(void) clear;

@end
