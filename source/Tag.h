//
//  Tag.h
//  BakingPi
//
//  Created by Jeremy Pereira on 22/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_Tag_h
#define BakingPi_Tag_h

#include "bptypes.h"
/*!
 *  @file Tag.h
 *  @brief Functions for iterating and reading ARM tags.
 *  @author Jeremy Pereira
 *
 *  These are the initialisation tags for the ARM CPU.  These are set up at
 *  address 0x100 by the boot loader after loading the kernel image and before
 *  starting it.
 */

/*!
 *  @brief ARM tag types
 */
enum TagType
{
    /*!
     * @brief A 32 bit 0 terminates the list of tags. Magic should be 0
     */
    TAG_TERMINATOR = 0x00000000,
    TAG_CORE	   = 0x54410001,
    TAG_MEM		   = 0x54410002,
    TAG_VIDEOTEXT  = 0x54410003,
    TAG_RAMDISK	   = 0x54410004,
    TAG_INITRD2	   = 0x54420005,
    TAG_SERIAL	   = 0x54410006,
    TAG_REVISION   = 0x54410007,
    TAG_VIDEOLFB   = 0x54410008,
    TAG_CMDLINE	   = 0x54410009,
    /*!
     *  @brief Number of tag types including the terminator.
     *  This should be one higher than the highest tag type without the magic
     *  numbers.
     */
    TAG_NUM_TYPES = 10
};

/*!
 *  @brief Get the tag type as a zero based index for indexing arrays
 *  @param T The tag type
 */
#define TAG_TYPE_INDEX(T)	((T) & 0xFFFF)

typedef enum TagType TagType;

/*!
 *  @brief ARM list of tags.
 */
struct TagList;
typedef struct TagList TagList;

/*!
 *  @brief ARM Tag header.
 */
struct Tag
{
    /*!
     *  @brief Length of the tag in words including the header.
     */
    uint32_t length;
    /*!
     *  @brief The type of this tag.
     */
    uint32_t tagType;
};
typedef struct Tag Tag;

/*!
 * @brief Core tag
 */
struct TagCore
{
    /*!
     *  @brief flags.
     *
     *  Don't know what these mean although bit 0 apparently says something 
     *  is read only.
     */
    uint32_t flags;
    /*!
     *  @brief System page size
     */
    uint32_t pageSize;
    /*!
     *  @brief Root device number.
     */
    uint32_t rootDeviceNo;
};
typedef struct TagCore TagCore;

/*!
 *  @brief Describes an area of physical RAM
 */
struct TagMem
{
    uintptr_t size;
    uintptr_t start;
};
typedef struct TagMem TagMem;
/*!
 *  @brief VGA descriptor
 */
struct TagVideo
{
    uint8_t width;
    uint8_t height;
    uint16_t page;
    uint8_t mode;
    uint8_t cols;
    uint16_t egaBx;
    uint8_t lines;
    uint8_t isVga;
    uint16_t points;
};
typedef struct TagVideo TagVideo;

/*!
 *  @brief Describes how the RAM disk will be used.
 */
struct TagRamDisk
{
    uint32_t flags;
    uint32_t uncompressedSizeKb;
    uint32_t start;
};
typedef struct TagRamDisk TagRamDisk;

/*!
 *  @brief Describes the location of the compressed RAM disk image.
 */
struct TagInitRd2
{
    uint32_t start;
    uint32_t size;
};
typedef struct TagInitRd2 TagInitRd2;

/*!
 *  @brief Serial number of the board
 */
struct TagSerial
{
    uint64_t serialNumber;

};
typedef struct TagSerial TagSerial;

/*!
 *  @brief Board revision
 */
struct TagRevision
{
    uint32_t revision;
};
typedef struct TagRevision TagRevision;

/*!
 *  @brief Describes the parameters for a framebuffer display.
 */
struct TagVideoLFB
{
    uint16_t width;
    uint16_t height;
    uint16_t depth;
    uint16_t linelength;
    uint32_t base;
    uint32_t size;
    uint8_t  redSize;
    uint8_t  redPos;
    uint8_t  greenSize;
    uint8_t  greenPos;
    uint8_t  blueSize;
    uint8_t  bluePos;
    uint8_t  rsvdSize;
    uint8_t  rsvdPos;
};
typedef struct TagVideoLFB TagVideoLFB;

/*!
 *  @brief command line
 */
struct TagCmdLine
{
    /*!
     *  @brief Characters in the command line.
     *
     *  This must be a null terminated C string padded out to a whole number of
     *  words.  This struct represents the minimum possible command line.
     */
    char characters[4];
};
typedef struct TagCmdLine TagCmdLine;

TagList* tag_initialiseTagList(uint32_t* tagSpace);
/*!
 *  @brief Get the first tag in a list of tags.
 *  @param tagList The list of tags.
 *  @return The first tag in the list.
 */
Tag* tag_getFirstTag(TagList* tagList);

/*!
 *  @brief Get the next tag in a tag list.
 *  @param tagList The tag list containing the tag.
 *  @param currentTag The current tag in the list.
 *  @return The next tag sequentially in the list or NULL i we are at the end
 *  or the tag type of the current tag doesn't make sense.
 */
Tag* tag_getNextTag(TagList* tagList, Tag* currentTag);

/*!
 *  @brief Get the type of the given tag
 *  @param tag The tag to get the tag typ of.
 */
TagType tag_type(Tag* tag);

/*!
 *  @brief get the length of the tag in words.
 *  @param tag Tag to get the length of.
 *  @return The length of the tag in words.
 */
size_t tag_length(Tag* tag);



#endif
