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
 *  @brief Add the core tag.
 *  @param flags No idea what the flags do.
 *  @param pageSize Set the page size
 *  @param rootDev What is the root file system device
 */
-(void) addCoreFlags: (uint32_t) flags
  			pageSize: (uint32_t) pageSize
             rootDev: (uint32_t) rootDev;

/*!
 *  @brief Add a memory region tag.
 *  @param size Size of the region in bytes.
 *  @param start Start of the memory region.
 */
-(void) addMemorySize: (uintptr_t) size start: (uintptr_t) start;

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
