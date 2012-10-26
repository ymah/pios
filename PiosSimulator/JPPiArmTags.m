//
//  JPPiArmTags.m
//  BakingPi
//
//  Created by Jeremy Pereira on 23/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import "JPPiArmTags.h"
#import "Tag.h"

@implementation JPPiArmTags
{
@private
    NSMutableData* theBytes;
    bool haveTerminator;
}

-(id) init
{
    self = [super init];
    if (self != nil)
    {
        theBytes = [[NSMutableData alloc] init];
    }
    return self;
}

-(const uint8_t*) bytes
{
    return [theBytes bytes];
}

-(void) addCommandLine: (NSString*) commandLine
{
    const char* commandBytes = [commandLine UTF8String];
    /*
     *  Calculated string size is the size of the string plus the null byte +
     *  padding to get up to a multiple of 4
     */
    uint32_t requiredStringWords = ((strlen(commandBytes) + 1) + 3) / sizeof(uint32_t);
    uint32_t tagWords = sizeof(Tag) / sizeof(uint32_t);
    size_t commandOffset = [theBytes length];
    [theBytes increaseLengthBy: (tagWords + requiredStringWords) * sizeof(uint32_t)];
    Tag* tagHeader = [theBytes mutableBytes] + commandOffset;
    tagHeader->length = tagWords + requiredStringWords;
    tagHeader->tagType = TAG_CMDLINE;
    tagHeader++;
    strcpy((char*) tagHeader, commandBytes); // We know there's room
}

-(void) addCoreFlags: (uint32_t) flags
  			pageSize: (uint32_t) pageSize
             rootDev: (uint32_t) rootDev
{
    size_t commandOffset = [theBytes length];
    uint32_t tagSizeInBytes = sizeof(Tag) + 3 * sizeof(uint32_t);
    [theBytes increaseLengthBy: tagSizeInBytes];
    Tag* tagHeader = [theBytes mutableBytes] + commandOffset;
    tagHeader->length = tagSizeInBytes / sizeof(uint32_t);
    tagHeader->tagType = TAG_CORE;
    tagHeader++;
    uint32_t* bodyPointer = (uint32_t*)tagHeader;
    *bodyPointer++ = flags;
    *bodyPointer++ = pageSize;
    *bodyPointer = rootDev;
}

-(void) addMemorySize: (uintptr_t) size start: (uintptr_t) start
{
    size_t commandOffset = [theBytes length];
    uint32_t tagSizeInBytes = sizeof(Tag) + 2 * sizeof(uintptr_t);
    [theBytes increaseLengthBy: tagSizeInBytes];
    Tag* tagHeader = [theBytes mutableBytes] + commandOffset;
    tagHeader->length = tagSizeInBytes / sizeof(uint32_t);
    tagHeader->tagType = TAG_MEM;
    tagHeader++;
    uintptr_t* bodyPointer = (uintptr_t*)tagHeader;
    *bodyPointer++ = size;
    *bodyPointer++ = start;
}


-(void) addTerminator
{
    size_t commandOffset = [theBytes length];
    [theBytes increaseLengthBy: sizeof(Tag)];
    Tag* tagHeader = [theBytes mutableBytes] + commandOffset;
    tagHeader->length = 0;
    tagHeader->tagType = TAG_TERMINATOR;
    haveTerminator = true;
}

-(void) clear
{
    [theBytes setLength: 0];
}

@end
