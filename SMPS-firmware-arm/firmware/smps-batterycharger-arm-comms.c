/* STM32F1 SMPS Battery Charger

Defines the Communications Task and library

This task receives incoming characters and assembles them into a command line.
The command is then interpreted and executed.
Commands include file operations, switch settings, all parameter settings.
The library consists of all routines needed by other tasks to communicate.

The communication medium is non-specific, referring on to the hardware module.

This task is the only one having write access to the object dictionary items.

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

#include <stdint.h>
#include <stdbool.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "smps-batterycharger-arm-hardware.h"
#include "smps-batterycharger-arm-objdic.h"
#include "smps-batterycharger-arm-measurement.h"
#include "smps-batterycharger-arm-lib.h"
#include "smps-batterycharger-arm-comms.h"

/*--------------------------------------------------------------------------*/
/* Local Prototypes */
static void initGlobals(void);
static void parseCommand(uint8_t* line);

/*--------------------------------------------------------------------------*/
/* Global Variables */
/* These configuration variables are part of the Object Dictionary. */
/* This is defined in smps-batterycharger-arm-objdic and is updated in response to
received messages. */
extern union ConfigGroup configData;

/* FreeRTOS queues and intercommunication variables */
/* Serial Comms defined here */
/* The semaphore must be used to protect messages until they have been queued
in their entirety. This is done in convenience functions defined below */
xQueueHandle commsSendQueue, commsReceiveQueue;
xSemaphoreHandle commsSendSemaphore, commsEmptySemaphore;
/* FreeRTOS queues and intercommunication variables defined in File */
extern xQueueHandle fileSendQueue, fileReceiveQueue;
extern xSemaphoreHandle fileSendSemaphore;

/*--------------------------------------------------------------------------*/
/* Local Variables */

/*--------------------------------------------------------------------------*/
/* @brief Communications Receive Task

This collects characters received over the communications interface and packages
them into a line for action as a command.
*/

void prvCommsTask( void *pvParameters )
{
    static uint8_t line[80];
    static uint8_t characterPosition = 0;

    initGlobals();

    while(1)
    {
/* Build a command line string before actioning. the task will block
indefinitely waiting for input. */
        char character;
        xQueueReceive(commsReceiveQueue,&character,portMAX_DELAY);
        if ((character == 0x0D) || (characterPosition > 78))
        {
            line[characterPosition] = 0;
            characterPosition = 0;
            parseCommand(line);
        }
        else line[characterPosition++] = character;
    }
}

/*--------------------------------------------------------------------------*/
/* @brief Initialize

This initializes the queues and semaphores used by the task.
*/

void initComms(void)
{
/* Setup the queues to use */
	commsSendQueue = xQueueCreate(COMMS_QUEUE_SIZE,1);
	commsReceiveQueue = xQueueCreate(COMMS_QUEUE_SIZE,1);
    commsSendSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(commsSendSemaphore);
    commsEmptySemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(commsEmptySemaphore);
}

/*--------------------------------------------------------------------------*/
/** @brief Initialise Global Variables

*/

static void initGlobals(void)
{
}

/*--------------------------------------------------------------------------*/
/** @brief Parse a command line and act on it.

Commands begin with a lower case letter a, d, p or f followed by an upper case
command letter.

@param[in] char *line: the command line in ASCII
*/

static void parseCommand(uint8_t* line)
{
/* ======================== Action commands ========================  */
    if (line[0] == 'a')
    {
        switch (line[1])
        {
        }
    }
/* ======================== Data request commands ==================  */
    else if (line[0] == 'd')
    {
        switch (line[1])
        {
/* Bn Ask for battery parameters to be sent */
        case 'B':
            {
                char id[] = "pR0";
                id[2] = line[2];
                uint8_t battery = line[2] - '1';
                dataMessageSend(id,getBatteryResistanceAv(),0);
                id[1] = 'T';
                dataMessageSend(id,(int32_t)configData.config.batteryType,
                                   (int32_t)configData.config.batteryCapacity);
                id[1] = 'F';
                dataMessageSend(id,(int32_t)configData.config.floatStageCurrentScale,
                                   (int32_t)configData.config.floatVoltage);
                id[1] = 'A';
                dataMessageSend(id,(int32_t)configData.config.bulkCurrentLimitScale,
                                   (int32_t)configData.config.absorptionVoltage);
                break;
            }
        }
    }
/* ===================== Parameter setting commands ================  */
    else if (line[0] == 'p')
    {
        switch (line[1])
        {
/* Tntxx Set battery type and capacity, t is type, xx is capacity */
        case 'T':
            {
                uint8_t type = line[2]-'0';
                if (type < 3) configData.config.batteryType = (battery_Type)type;
                configData.config.batteryCapacity = asciiToInt((char*)line+3);
                break;
            }
/* Inxx Set bulk current limit, xx is limit */
        case 'I':
            {
                configData.config.bulkCurrentLimitScale = asciiToInt((char*)line+2);
                setCurrentLimit(configData.config.bulkCurrentLimitScale);
                break;
            }
/* Anxx Set battery absorption voltage limit, xx is limit */
        case 'A':
            {
                configData.config.absorptionVoltage = asciiToInt((char*)line+2);
                break;
            }
/* fnxx Set battery float current trigger, xx is trigger */
        case 'f':
            {
                configData.config.floatStageCurrentScale = asciiToInt((char*)line+2);
                break;
            }
/* Fnxx Set battery float voltage limit, xx is limit */
        case 'F':
            {
                configData.config.floatVoltage = asciiToInt((char*)line+2);
                break;
            }
/* Dxx Set charger duty cycle (debug only) */
        case 'D':
            {
                uint8_t dutyCycle = asciiToInt((char*)line+2);
                if (dutyCycle > 99) dutyCycle = 99;
                pwmSetDutyCycle(dutyCycle,0);
                break;
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
/** @brief Send a data message with two parameters

This is an asynchronous TxPDO in CANopen and therefore is nominally of fixed
length. The response parameters are converted to ASCII integer for debug.

This will abandon the message if commsSendSemaphore cannot be obtained.

@param char* ident: an identifier string recognized by the receiving program.
@param int32_t param1: first parameter.
@param int32_t param2: second parameter.
*/

void dataMessageSend(char* ident, int32_t param1, int32_t param2)
{
    if (! xSemaphoreTake(commsSendSemaphore,COMMS_SEND_DELAY)) return;
    commsPrintString(ident);
    commsPrintString(",");
    commsPrintInt(param1);
    commsPrintString(",");
    commsPrintInt(param2);
    commsPrintString("\r\n");
    xSemaphoreGive(commsSendSemaphore);
}

/*--------------------------------------------------------------------------*/
/** @brief Send a data message with two parameters at low priority.

This is the same as dataMessageSend except that a message is only sent if the
commsSendQueue is empty.

This blocks indefinitely until the queue is empty of all messages.

@param char* ident: an identifier string recognized by the receiving program.
@param int32_t param1: first parameter.
@param int32_t param2: second parameter.
*/

void dataMessageSendLowPriority(char* ident, int32_t param1, int32_t param2)
{
/* If any characters are on the send queue, block on the commsEmptySemaphore
which is released by the ISR after the last character has been sent. One
message is then sent. The calling task cannot queue more than one message. */
    while (uxQueueMessagesWaiting(commsSendQueue) > 0)
        xSemaphoreTake(commsEmptySemaphore,portMAX_DELAY);
/* Hold indefinitely as the message must not be abandoned */
    xSemaphoreTake(commsSendSemaphore,portMAX_DELAY);
    commsPrintString(ident);
    commsPrintString(",");
    commsPrintInt(param1);
    commsPrintString(",");
    commsPrintInt(param2);
    commsPrintString("\r\n");
    xSemaphoreGive(commsSendSemaphore);
}

/*--------------------------------------------------------------------------*/
/** @brief Send a data message with one parameter

Use to send a simple response to a command. The calling task will block until
commsSendSemaphore is available.

@param[in] char* ident. Response identifier string
@param[in] int32_t parameter. Single integer parameter.
*/

void sendResponse(char* ident, int32_t parameter)
{
    xSemaphoreTake(commsSendSemaphore,portMAX_DELAY);
    commsPrintString(ident);
    commsPrintString(",");
    commsPrintInt(parameter);
    commsPrintString("\r\n");
    xSemaphoreGive(commsSendSemaphore);
}

/*--------------------------------------------------------------------------*/
/** @brief Send a string

Use to send a string. This abandons the message if it cannot get the
commsSendSemaphore.

@param[in] char* ident. Response identifier string
@param[in] int32_t parameter. Single integer parameter.
*/

void sendString(char* ident, char* string)
{
    if (! xSemaphoreTake(commsSendSemaphore,COMMS_SEND_DELAY)) return;
    commsPrintString(ident);
    commsPrintString(",");
    commsPrintString(string);
    commsPrintString("\r\n");
    xSemaphoreGive(commsSendSemaphore);
}

/*--------------------------------------------------------------------------*/
/** @brief Send a string at low priority.

Use to send a string. A message is only sent if the commsSendQueue is empty.

This blocks indefinitely until the queue is empty of all messages.

@param[in] char* ident. Response identifier string
@param[in] int32_t parameter. Single integer parameter.
*/

void sendStringLowPriority(char* ident, char* string)
{
/* If any characters are on the send queue, block on the commsEmptySemaphore
which is released by the ISR after the last character has been sent. One
message is then sent. The calling task cannot queue more than one message. */
    while (uxQueueMessagesWaiting(commsSendQueue) > 0)
        xSemaphoreTake(commsEmptySemaphore,portMAX_DELAY);
/* Hold indefinitely as the message must not be abandoned */
    xSemaphoreTake(commsSendSemaphore,portMAX_DELAY);
    commsPrintString(ident);
    commsPrintString(",");
    commsPrintString(string);
    commsPrintString("\r\n");
    xSemaphoreGive(commsSendSemaphore);
}

/*--------------------------------------------------------------------------*/
/** @brief Print out the contents of a register (debug)

@param[in] uint32_t reg: full address of register.

*/

void print_register(uint32_t reg)
{
	commsPrintHex((reg >> 16) & 0xFFFF);
	commsPrintHex((reg >> 00) & 0xFFFF);
	commsPrintChar(" ");
}

/*--------------------------------------------------------------------------*/
/** @brief Print out a value in ASCII decimal form (ack Thomas Otto)

@param[in] int32_t value: integer value to be printed.
*/

void commsPrintInt(int32_t value)
{
	uint8_t i=0;
	char buffer[25];
    intToAscii(value, buffer);
	while (buffer[i] > 0)
	{
	    commsPrintChar(&buffer[i]);
        i++;
	}
}

/*--------------------------------------------------------------------------*/
/** @brief Print out a value in 16 bit ASCII hex form

@param[in] int32_t value: integer value to be printed.
*/

void commsPrintHex(uint32_t value)
{
	uint8_t i;
	char buffer[25];

	for (i = 0; i < 4; i++)
	{
		buffer[i] = "0123456789ABCDEF"[value & 0xF];
		value >>= 4;
	}
	for (i = 4; i > 0; i--)
	{
	    commsPrintChar(&buffer[i-1]);
	}
    commsPrintChar(" ");
}

/*--------------------------------------------------------------------------*/
/** @brief Print a String

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
to ensure it is sent in entirety.

@param[in] char *ch: pointer to character to be printed.
*/

void commsPrintChar(char *ch)
{
//    xSemaphoreTake(commsEmptySemaphore,0);  /* Flag as not empty. */
    commsEnableTxInterrupt(false);
  	xQueueSendToBack(commsSendQueue,ch,portMAX_DELAY);
    commsEnableTxInterrupt(true);
}

