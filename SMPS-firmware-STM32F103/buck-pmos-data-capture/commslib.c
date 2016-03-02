/* STM32F1 Power Management Communications Library Functions

The buffering of receive and send characters is handled here, as well as
handling of buffer full and empty situations.

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

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/usart.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "buffer.h"
#include "stringlib.h"
#include "commslib.h"

/*--------------------------------------------------------------------------*/
/* Receive and Transmit buffer globals */

static uint8_t sendBuffer[BUFFER_SIZE+3];
static uint8_t receiveBuffer[BUFFER_SIZE+3];

/*--------------------------------------------------------------------------*/
/** @brief Initialize Communications Buffers to Empty

*/

void commsInit(void)
{
	buffer_init(sendBuffer,BUFFER_SIZE);
	buffer_init(receiveBuffer,BUFFER_SIZE);
}

/*--------------------------------------------------------------------------*/
/** @brief Return next character from receive buffer

The buffer_get function returns 0x100 if no character is present.
*/

uint16_t commsNextCharacter(void)
{
    return buffer_get(receiveBuffer);
}

/*--------------------------------------------------------------------------*/
/** @brief Send a data message with two integer parameters

The message consists of an identification string followed by two parameters
in integer ASCII form, all separated by commas, followed by CR-LF.

This is an asynchronous TxPDO in CANopen and therefore is nominally of fixed
length. The response parameters are converted to ASCII integer for debug.

This will abandon the entire message if the output buffer has insufficient
space.

@param char* ident: an identifier string recognized by the receiving program.
@param int32_t param1: first integer parameter.
@param int32_t param2: second integer parameter.
@returns true if message was buffered.
*/

bool dataMessageSend(char* ident, int32_t param1, int32_t param2)
{
    char param1Buffer[11];
    char param2Buffer[11];
    intToAscii(param1,param1Buffer);
    intToAscii(param2,param2Buffer);
    if (buffer_output_places(sendBuffer) < 
        stringLength(param1Buffer)+stringLength(param2Buffer)+5)
        return false;
    commsPrintString(ident);
    commsPrintString(",");
    commsPrintString(param1Buffer);
    commsPrintString(",");
    commsPrintString(param2Buffer);
    commsPrintString("\r\n");
    return true;
}

/*--------------------------------------------------------------------------*/
/** @brief Send a data message with one integer parameter

The message consists of a identification stringt followed by a parameter
in integer ASCII form, all separated by commas, followed by CR-LF.

Use to send a simple integer response to a command. This will abandon thw
message if the output buffer is has insufficient space.

@param[in] char* ident. Response identifier string
@param[in] int32_t parameter. Single integer parameter.
@returns true if message was buffered.
*/

bool sendResponse(char* ident, int32_t parameter)
{
    char paramBuffer[11];
    intToAscii(parameter,paramBuffer);
    if (buffer_output_places(sendBuffer) < stringLength(paramBuffer)+4)
        return false;
    commsPrintString(ident);
    commsPrintString(",");
    commsPrintString(paramBuffer);
    commsPrintString("\r\n");
    return true;
}

/*--------------------------------------------------------------------------*/
/** @brief Send a string

Use to send a string consisting of a identification string, a comma and a
variable length string. This will abandon the message if the output buffer is
has insufficient space.

@param[in] char* ident. Response identifier string
@param[in] int32_t parameter. Single integer parameter.
@returns true if message was buffered.
*/

bool sendString(char* ident, char* string)
{
    if (buffer_output_places(sendBuffer) < stringLength(string)+4)
        return false;
    commsPrintString(ident);
    commsPrintString(",");
    commsPrintString(string);
    commsPrintString("\r\n");
    return true;
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
The application is responsible for ensuring the message is sent in entirety
(see convenience functions defined here).

@param[in] char *ch: pointer to character to be printed.
*/

void commsPrintChar(char *ch)
{
    usart_disable_tx_interrupt(USART2);
    buffer_put(sendBuffer,*ch);
    usart_enable_tx_interrupt(USART2);
}

/*--------------------------------------------------------------------------*/
/** @brief USART ISR

Find out what interrupted and get or send data as appropriate */

void usart2_isr(void)
{
	static uint16_t data;

	/* Check if we were called because of RXNE. */
	if (usart_get_flag(USART2,USART_SR_RXNE))
	{
		/* If buffer full we'll just drop it */
		buffer_put(receiveBuffer, (uint8_t) usart_recv(USART2));
	}
	/* Check if we were called because of TXE. */
	if (usart_get_flag(USART2,USART_SR_TXE))
	{
		/* If buffer empty, disable the tx interrupt */
		data = buffer_get(sendBuffer);
		if ((data & 0xFF00) > 0) usart_disable_tx_interrupt(USART2);
		else usart_send(USART2, (data & 0xFF));
	}
}

