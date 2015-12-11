/* STM32F1 Power Management Communications Library Functions

Initial 11 December 2015
*/

/*
 * Copyright 2015 K. Sarkies <ksarkies@internode.on.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "buffer.h"
#include "stringlib.h"
#include "commslib.h"

/* Output buffer global */

char outBuffer[128];

/*--------------------------------------------------------------------------*/
/** @brief Send a data message with two integer parameters

The message consists of a single character ident followed by two parameters
in integer ASCII form, all separated by commas, followed by CR-LF.

This is an asynchronous TxPDO in CANopen and therefore is nominally of fixed
length. The response parameters are converted to ASCII integer for debug.

This will abandon the entire message if the output buffer has insufficient
space.

@param char* ident: an identifier string recognized by the receiving program.
@param int32_t param1: first integer parameter.
@param int32_t param2: second integer parameter.
*/

void dataMessageSend(char* ident, int32_t param1, int32_t param2)
{
    char param1Buffer[11];
    char param2Buffer[11];
    intToAscii(param1,param1Buffer);
    intToAscii(param2,param2Buffer);
    if (buffer_output_space(outBuffer) < 
        stringLength(param1Buffer)+stringLength(param2Buffer)+5) return;
    commsPrintString(ident);
    commsPrintString(",");
    commsPrintString(param1Buffer);
    commsPrintString(",");
    commsPrintString(param2Buffer);
    commsPrintString("\r\n");
}

/*--------------------------------------------------------------------------*/
/** @brief Send a data message with one integer parameter

The message consists of a single character ident followed by a parameter
in integer ASCII form, all separated by commas, followed by CR-LF.

Use to send a simple integer response to a command. This will abandon thw
message if the output buffer is has insufficient space.

@param[in] char* ident. Response identifier string
@param[in] int32_t parameter. Single integer parameter.
*/

void sendResponse(char* ident, int32_t parameter)
{
    char paramBuffer[11];
    intToAscii(parameter,paramBuffer);
    if (buffer_output_space(outBuffer) < stringLength(paramBuffer)+4) return;
    commsPrintString(ident);
    commsPrintString(",");
    commsPrintString(paramBuffer);
    commsPrintString("\r\n");
}

/*--------------------------------------------------------------------------*/
/** @brief Send a string

Use to send a string consisting of a single character ident, a comma and a
variable length string. This will abandon the message if the output buffer is
has insufficient space.

@param[in] char* ident. Response identifier string
@param[in] int32_t parameter. Single integer parameter.
*/

void sendString(char* ident, char* string)
{
    if (buffer_output_space(outBuffer) < stringLength(string)+4) return;
    commsPrintString(ident);
    commsPrintString(",");
    commsPrintString(string);
    commsPrintString("\r\n");
}

/*--------------------------------------------------------------------------*/
/** @brief Print a String

There is no limit to the length of the string sent direct to the output buffer.
Buffer space checking must be done before calling this.

@param[in] char *ch: pointer to string to be printed.
*/

void commsPrintString(char *ch)
{
    while(*ch) commsPrintChar(ch++);
}

/*--------------------------------------------------------------------------*/
/** @brief Print a Character

Disable the Tx interrupt first to prevent queue corruption in the ISR.
This is where the characters are queued for the ISR to transmit.
The ISR is in the hardware module.

Characters are placed on a queue and picked up by the ISR for transmission.
The application is responsible for protecting a message with semaphores
to ensure it is sent in entirety (see convenience functions defined here).

If the queue fails to respond it is reset. A number of messages will be lost but
hopefully the application will continue to run. A receiving program may see
a corrupted message.

@param[in] char *ch: pointer to character to be printed.
*/

void commsPrintChar(char *ch)
{
}


