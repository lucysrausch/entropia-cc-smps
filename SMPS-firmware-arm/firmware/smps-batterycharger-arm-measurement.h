/* STM32F1 SMPS Battery Charger

This header file contains defines and prototypes specific to the
measurement task.

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

#ifndef SMPS_BATTERYCHARGER_MEASUREMENT_H_
#define SMPS_BATTERYCHARGER_MEASUREMENT_H_

#include <stdint.h>
#include <stdbool.h>
#include "smps-batterycharger-arm-objdic.h"

/* Number of samples taken and averaged of each quantity measured */
#define N_SAMPLES 1024

/* Calibration */
/* For current the scaling factor gives a value in 1/256 Amp precision.
Subtract this from the measured value and scale by this factor.
Then after averaging scale back by 4096 to give the values used here.
Simply scale back further by 256 to get the actual (floating point)
current. Thus the results here are 256 times the current in amperes. */

#define CURRENT_OFFSET 0

/* Current scale: ref voltage 3.276 times 256 over amplifier transconductance
(6.8 times 0.33) */

#define CURRENT_SCALE 376

/* Voltage offset: */

#define VOLTAGE_OFFSET 0

/* Voltage scale: divider 48.7/10 times ref voltage 3.172 times 256 */

#define VOLTAGE_SCALE 3955

/* Temperature measurement via LM335 for reference voltage 3.280V.
Scale is 3.28V over 10mV per degree C times 256.
Offset is 2.732V at 0 degrees C over 3.280 times 4096. */

#define TEMPERATURE_SCALE   328*256
#define TEMPERATURE_OFFSET  3412

/*--------------------------------------------------------------------------*/
/* Prototypes */
/*--------------------------------------------------------------------------*/

void prvMeasurementTask(void *pvParameters);

/* Data access functions */
int16_t getBatteryResistanceAv(void);
int16_t getBatteryAccumulatedCharge(void);
int16_t getBatteryCurrent(void);
int16_t getBatteryVoltage(void);
int32_t getTemperature(void);

#endif

