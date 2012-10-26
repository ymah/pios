//
//  console.c
//  BakingPi
//
//  Created by Jeremy Pereira on 24/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "console.h"
#include "gdi.h"
#include "klib.h"

// TODO: When we get the allocator, do not hard code the console size

enum
{
    CONSOLE_WIDTH = 1024 / 8,
    CONSOLE_HEIGHT = 768 / 16,
};

struct Console
{
    GDIContext* context;
    GDIPoint cursorPos;
};

static Console defaultConsole = { NULL, { 0, 0 } };

Console* con_initialiseConsole(GDIContext* gdiContext)
{
    defaultConsole.context = gdiContext;
    defaultConsole.cursorPos.x = 0;
    defaultConsole.cursorPos.y = 0;
    con_clearCurrentLine(&defaultConsole);
    return &defaultConsole;
}

Console* con_getTheConsole()
{
    Console* ret = NULL;
    if (defaultConsole.context != NULL)
    {
        ret = &defaultConsole;
    }
    return ret;
}


void con_putChars(Console* console, const char* chars, size_t numChars)
{
    GDIRect frame = gdi_frame(console->context);
    for (size_t i = 0 ; i < numChars ; ++i)
    {
        if (chars[i] == '\n')
        {
            con_newLine(console);
        }
        else
        {
            GDIRect drawnRect = gdi_drawChar(console->context,
                                             console->cursorPos,
                                             chars[i]);
            console->cursorPos.x += drawnRect.size.width;
            if (console->cursorPos.x >= frame.size.width)
            {
                con_newLine(console);
            }
        }
    }
}

void con_putCString(Console* console, const char* chars)
{
    // TODO: The limit is rather arbitrary
    con_putChars(console, chars, klib_strnlen(chars, 1000));
}

void con_newLine(Console* console)
{
    GDIRect frame = gdi_frame(console->context);
    GDISize charSize = gdi_systemFontCharSize(console->context);
	console->cursorPos.x = 0;
    console->cursorPos.y += charSize.height;
    if (console->cursorPos.y >= frame.size.height)
    {
        // TODO: Scroll the screen instead
        console->cursorPos.y = 0;
    }
    con_clearCurrentLine(console);
}

#define DIGIT_BITS	4
#define NUM_DIGITS  (32 / DIGIT_BITS)

void con_putHex32(Console* console, uint32_t aNumber)
{
    char digits[NUM_DIGITS];
    
    for (size_t i = 0 ; i <  NUM_DIGITS ; ++i)
    {
        char thisDigit = aNumber & 0xF;
        if (thisDigit < 0xA)
        {
            thisDigit += '0';
        }
        else
        {
            thisDigit += 'A' - 0xA;
        }
        digits[NUM_DIGITS - i - 1] = thisDigit;
        aNumber >>= DIGIT_BITS;
    }
    con_putChars(console, digits, NUM_DIGITS);
}

void con_clearCurrentLine(Console* console)
{
    GDIRect frame = gdi_frame(console->context);
	GDIRect currentLineRect;
    GDISize charSize = gdi_systemFontCharSize(console->context);
    currentLineRect.origin = console->cursorPos;
    currentLineRect.size.width = frame.size.width;
    currentLineRect.size.height = charSize.height;
    gdi_fillRect(console->context, currentLineRect, GDI_BACKGROUND);
}

