//
//  console.c
//  BakingPi
//
//  Created by Jeremy Pereira on 24/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#include "console.h"
#include "gdi.h"

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

static Console defaultConsole;

Console* con_initialiseConsole(GDIContext* gdiContext)
{
    defaultConsole.context = gdiContext;
    defaultConsole.cursorPos.x = 0;
    defaultConsole.cursorPos.y = 0;
    return &defaultConsole;
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
}
