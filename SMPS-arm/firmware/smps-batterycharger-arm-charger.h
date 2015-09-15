/* STM32F1 SMPS Battery Charger

This header file contains defines and prototypes specific to the charging
task.

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

#ifndef SMPS_BATTERYCHARGER_CHARGER_H_
#define SMPS_BATTERYCHARGER_CHARGER_H_

#include <stdint.h>
#include <stdbool.h>

/*--------------------------------------------------------------------------*/
/* Prototypes */
/*--------------------------------------------------------------------------*/
void prvChargerTask(void *pvParameters);

battery_Ch_States getBatteryChargingState(void);

#endif

