//
//  JPPiArmTags.m
//  BakingPi
//
//  Created by Jeremy Pereira on 23/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#import "JPPiArmTags.h"
#import "Tag.h"

#define SERIALISE(N, T, P)	\
do {						\
	*(T*)(P) = (N);		\
	(P) += sizeof(T);		\
} while(0)


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

-(void) addVideoTextX: (uint8_t) x
					y: (uint8_t) y
				 page: (uint16_t) page
				 mode: (uint8_t) mode
                 cols: (uint8_t) cols
                egaBx: (uint16_t) egaBx
                lines: (uint8_t) lines
                isVGA: (uint8_t) isVGA
               points: (uint16_t) points
{
    size_t commandOffset = [theBytes length];
    uint32_t tagSizeInBytes = sizeof(Tag) + sizeof(TagVideo);
    [theBytes increaseLengthBy: tagSizeInBytes];
    Tag* tagHeader = [theBytes mutableBytes] + commandOffset;
    tagHeader->length = tagSizeInBytes / sizeof(uint32_t);
    tagHeader->tagType = TAG_VIDEOTEXT;
    tagHeader++;
    uint8_t* bodyPointer = (uint8_t*)tagHeader;
    SERIALISE(x, uint8_t, bodyPointer);
    SERIALISE(y, uint8_t, bodyPointer);
    SERIALISE(page, uint16_t, bodyPointer);
    SERIALISE(mode, uint8_t, bodyPointer);
    SERIALISE(cols, uint8_t, bodyPointer);
    SERIALISE(egaBx, uint16_t, bodyPointer);
    SERIALISE(lines, uint8_t, bodyPointer);
    SERIALISE(isVGA, uint8_t, bodyPointer);
    SERIALISE(points, uint16_t, bodyPointer);
}

-(void) addRAMDiskFlags: (uint32_t) flags
                   size: (uint32_t) size
                  start: (uint32_t) start
{
    size_t commandOffset = [theBytes length];
    uint32_t tagSizeInBytes = sizeof(Tag) + sizeof(TagRamDisk);
    [theBytes increaseLengthBy: tagSizeInBytes];
    Tag* tagHeader = [theBytes mutableBytes] + commandOffset;
    tagHeader->length = tagSizeInBytes / sizeof(uint32_t);
    tagHeader->tagType = TAG_RAMDISK;
    tagHeader++;
    uint8_t* bodyPointer = (uint8_t*)tagHeader;
    SERIALISE(flags, uint32_t, bodyPointer);
    SERIALISE(size, uint32_t, bodyPointer);
    SERIALISE(start, uint32_t, bodyPointer);
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
