//
//  console.h
//  BakingPi
//
//  Created by Jeremy Pereira on 24/10/2012.
//  Copyright (c) 2012 Jeremy Pereira. All rights reserved.
//

#ifndef BakingPi_console_h
#define BakingPi_console_h

#include "bptypes.h"
#include "gdi.h"

/*!
 *  @brief Opaque type reresenting the console
 */
struct Console;
typedef struct Console Console;

/*!
 *  @brief Initialise a console using the given context
 *  @param gdiContext The GDI context to whih  the console will write
 *  @return A new console object.
 */
Console* con_initialiseConsole(GDIContext* gdiContext);

/*!
 *  @brief Get a pointer to the console.
 *
 *  The console must have been initialised before getting it.  
 *  @return The console.
 */
Console* con_getTheConsole();

/*!
 *  @brief Clear the current line to background colour 
 *  @param console The console to operate on.
 */
void con_clearCurrentLine(Console* console);

/*!
 *  @brief Put the given characters on the console at the cursor location.
 *  
 *  Newlines are respected.
 *  @param console The console on which to put the characters.
 *  @param chars Pointer to the first character to print.
 *  @param numChars Number of characters to print.
 */
void con_putChars(Console* console, const char* chars, size_t numChars);

/*!
 *  @brief Convenience method to put a C string.
 *
 *  The C string is a sequence of 8 bit characters terminated with a '\0'.
 *  @param console The console.
 *  @param chars The null terminated C string to put on the screen.
 */
void con_putCString(Console* console, const char* chars);


/*!
 *  @brief Put a new line to the console
 *  @param console The console.
 */
void con_newLine(Console* console);

/*!
 *  @brief Ouput an unsigned 32 bit integer
 *  @param console The console object
 *  @param aNumber The number to output.
 */
void con_putHex32(Console* console, uint32_t aNumber);

void con_putHex16(Console* console, uint16_t aNumber);
void con_putHex8(Console* console, uint8_t aNumber);

void con_gotoLineStart(Console* console);

#endif
