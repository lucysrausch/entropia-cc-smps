/* STM32F1 SMPS Battery Charger

This header file contains defines and prototypes specific to the hardware
initialization and management.

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

#ifndef SMPS_BATTERYCHARGER_HARDWARE_H_
#define SMPS_BATTERYCHARGER_HARDWARE_H_

#include <stdint.h>
#include <stdbool.h>

/* Number of A/D converter channels available (STM32F103) */
#define NUM_CHANNEL 16

/* Timer parameters */
/* A PWM period of 50 microsec (20 kHz) */
#define PWM_PERIOD      14400/4
// Deadtime correction when PWM mode is changed
#define DEADTIME        100

/* USART */
#define BAUDRATE        115200

/*--------------------------------------------------------------------------*/
/* Interface Prototypes */
/*--------------------------------------------------------------------------*/
void prvSetupHardware(void);
uint32_t adcValue(uint8_t channel);
uint8_t adcEOC(void);
void pwmSetDutyCycle(uint8_t dutyCycleBuck,uint8_t dutyCycleBoost);
void setCurrentLimit(uint16_t limit);
void commsEnableTxInterrupt(uint8_t enable);

#endif

