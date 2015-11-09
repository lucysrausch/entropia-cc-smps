/* STM32F1 SMPS Battery Charger

This header file contains defines and prototypes specific to the
communications task. A number of convenience functions are
provided for sending different formats.

Copyright K. Sarkies <ksarkies@internode.on.net>

Initial 1 February 2014
*/

/*
 * This file is part of the smps-batterycharger project.
 *
 * Copyright 2014 K. Sarkies <ksarkies@internode.on.net>
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

#ifndef SMPS_BATTERYCHARGER_COMMS_H_
#define SMPS_BATTERYCHARGER_COMMS_H_

#include <stdint.h>
#include <stdbool.h>
#include "smps-batterycharger-arm-objdic.h"

#define COMMS_QUEUE_SIZE            512
#define COMMS_SEND_DELAY            128

/*--------------------------------------------------------------------------*/
/* Prototypes */
/*--------------------------------------------------------------------------*/
void prvCommsTask(void *pvParameters);
void initComms(void);
void dataMessageSend(char* ident, int32_t parm1, int32_t parm2);
void dataMessageSendLowPriority(char* ident, int32_t param1, int32_t param2);
void sendResponse(char* ident, int32_t parameter);
void sendString(char* ident, char* string);
void sendStringLowPriority(char* ident, char* string);
void commsPrintInt(int32_t value);
void commsPrintHex(uint32_t value);
void commsPrintString(char *ch);
void commsPrintChar(char *ch);

#endif

