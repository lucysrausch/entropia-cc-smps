/* STM32F1 ASCII and Communications Library Functions

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

#ifndef COMMS_LIB_H_
#define COMMS_LIB_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define BUFFER_SIZE 128

void commsInit(void);
uint16_t commsNextCharacter(void);
bool dataMessageSend(char* ident, int32_t parm1, int32_t parm2);
bool sendResponse(char* ident, int32_t parameter);
bool sendString(char* ident, char* string);
void commsPrintString(char *ch);
void commsPrintChar(char *ch);

#endif

