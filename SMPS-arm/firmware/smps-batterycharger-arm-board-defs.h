/* STM32F1 SMPS Battery Charger

Definitions for the Selected Microcontroller Board.

Either the ET-STM32F103 or the ET-STAMP-STM board can be used.

Copyright K. Sarkies <ksarkies@internode.on.net>

Initial 1 February 2014

Ports used depend on the circuit configuration. The maximum is as follows:

Ports PC6 (OC1), PC7 (OC2), PA7 (OC1N), PB0 (OC2N) are used for the for PWM.
Port PA6 is a break signal used by the overcurrent protection.

PA0  ADC0 Vin
PA1  ADC1 Vbatt
PA2  ADC2 Temperature

PA4  DAC1 Current Limit

The timer ports are dedicated and are set in the hardware module.
Timer 8 is used because timer 1 clashes with USART 1.

PA7  Timer 8 CH1N:  Buck Upper
PC6  Timer 8 CH1:   Buck Lower
PB0  Timer 8 CH2N:  Boost Upper
PC7  Timer 8 CH2:   Boost Lower
PA6  Timer 8 BKin   PWM break input

USART ports also dedicated.

PA9  USART1_TX
PA10 USART1_RX
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

#ifndef BOARD_H_
#define BOARD_H_

/* A/D Converter Channels */

#if defined USE_ET_STM32F103

#define ADC_CHANNEL_BATTERY_CURRENT     0
#define ADC_CHANNEL_BATTERY_VOLTAGE     1
#define ADC_CHANNEL_TEMPERATURE         2

#elif defined USE_ET_STAMP_STM32

#define ADC_CHANNEL_BATTERY_CURRENT     0
#define ADC_CHANNEL_BATTERY_VOLTAGE     1
#define ADC_CHANNEL_TEMPERATURE         2

#endif

/* Controls and Status Signals */

#if defined USE_ET_STM32F103

#elif defined USE_ET_STAMP_STM32

#endif

/* GPIO Port Settings */

#if defined USE_ET_STM32F103

#define PA_ANALOGUE_INPUTS              GPIO0 | GPIO1 | GPIO2

#elif defined USE_ET_STAMP_STM32

#define PA_ANALOGUE_INPUTS              GPIO0 | GPIO1 | GPIO2

#endif

#endif

