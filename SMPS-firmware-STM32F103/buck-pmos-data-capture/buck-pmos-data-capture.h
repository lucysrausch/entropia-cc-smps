/* STM32F1 Generation of PWM and Capture of voltages and currents in SMPS

This header file contains defines and prototypes.

Initial 7 December 2015
*/

/*
 * This file is part of the SMPS project.
 *
 * Copyright K. Sarkies <ksarkies@internode.on.net>
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

#ifndef BUCK_PMOS_DATA_CAPTURE_H_
#define BUCK_PMOS_DATA_CAPTURE_H_

#include <stdint.h>
#include <stdbool.h>

#define FIRMWARE_VERSION    "0.0"

#define LINE_SIZE           80
#define NUM_CHANNEL         2
#define BAUDRATE            230400
#define PERIOD              200
#define DEADTIME            30
#define DATA_BLOCK_SIZE     1024

/*--------------------------------------------------------------------------*/
/* Prototypes */
/*--------------------------------------------------------------------------*/
void timer1SetupPWM(void);
void timer2Setup(uint16_t count);
void adcSetup(void);
void dmaAdcSetup(void);
void gpioSetup(void);
void usartSetup(void);
void clockSetup(void);
void timer1PWMsettings(uint16_t period, int16_t buckDutyCycle,
                  int16_t boostDutyCycle);

void parseCommand(uint8_t* line);

#endif
