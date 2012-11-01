//
//  Tag.c
//  BakingPi
//
//  Created by Jeremy Pereira on 22/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "Tag.h"

#define TAG_BODY(TAG, TYPE)	((TYPE*)((TAG) + 1))

struct TagList
{
    Tag* firstTag;
};

static TagList defaultTagList = { 0 };

TagList* tag_initialiseTagList(uint32_t* tagSpace)
{
    defaultTagList.firstTag = (Tag*) tagSpace;
    return &defaultTagList;
}

Tag* tag_getFirstTag(TagList* tagList)
{
    return tagList->firstTag;
}

size_t tag_length(Tag* aTag)
{
    return aTag->length;
}

TagType tag_type(Tag* aTag)
{
    return aTag->tagType;
}

Tag* tag_getNextTag(TagList* tagList, Tag* currentTag)
{
    Tag* ret = NULL;
    if (tag_type(currentTag) != TAG_TERMINATOR
        && TAG_TYPE_INDEX(tag_type(currentTag)) < TAG_NUM_TYPES)
    {
        size_t tagLength = currentTag->length;
        ret = (Tag*)(((uint32_t*) currentTag) + tagLength);
    }
    return ret;
}

uintptr_t tag_memoryTop(TagList* tagList)
{
    uintptr_t theTop = 0;
    
    Tag* tag = tag_getFirstTag(tagList);
    while (tag != NULL && tag_type(tag) != TAG_TERMINATOR)
    {
        if (tag_type(tag) == TAG_MEM)
        {
            uintptr_t currentTop = TAG_BODY(tag, TagMem)->size;
            if (currentTop > theTop)
            {
                theTop = currentTop;
            }
        }
        tag = tag_getNextTag(tagList, tag);
    }
    return theTop;
}
