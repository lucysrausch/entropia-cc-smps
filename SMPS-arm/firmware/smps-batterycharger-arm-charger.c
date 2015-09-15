/* STM32F1 SMPS Battery Charger

Battery Charging Task Task

This task implements an algorithm to manage charging of batteries. Battery
current and terminal voltage are monitored constantly. If either rise beyond
the preset limits, charging is cut off and held off until the quantity drops
below the limit.

Average current is computed and used to determine when the algorithm is to
pass into the float stage.

The management task designates all batteries to be charged with different
priority levels. When charging is stopped on the priority battery, either
temporarily or when completed, the second priority battery is switched in.

The charge algorithm is:
1. Charge at maximum rate C/4 until the voltage reaches the specified limit.
2. Charge at the voltage limit until the current drops to the low
   threshold or timeout occurs.
3. For wet cells, charge briefly at the equalization voltage.

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

#include "smps-batterycharger-arm-hardware.h"
#include "smps-batterycharger-arm-measurement.h"
#include "smps-batterycharger-arm-charger.h"

/* Local Prototypes */
static void initGlobals(void);
static int16_t voltageLimit(uint16_t limitV);

/* Global Variables */
/* Sempahore for communications message synchronization */
extern xSemaphoreHandle commsSendSemaphore;

/* Local Variables */
static battery_Ch_States batteryChargingState;

/*--------------------------------------------------------------------------*/
/* @brief Charging Task

This task runs over long times, implementing the charging algorithm on a
battery that have been designated for charging.
*/

void prvChargerTask(void *pvParameters)
{
    uint16_t dutyCycle = 50*256;
    uint16_t dutyCycleMax = 100*256;
    static int16_t voltageAv;
    static int16_t currentAv;
    uint8_t batteryUnderCharge = 0;

    initGlobals();

    voltageAv = 0;
    currentAv = 0;

	while (1)
	{
/* Wait until the next tick cycle */
	    vTaskDelay(getChargerDelay());

    }
}

/*--------------------------------------------------------------------------*/
/** @brief Initialise Global Variables

Set the charger default parameters.
*/

static void initGlobals(void)
{
    batteryChargingState = bulkC;
}

/*--------------------------------------------------------------------------*/
/** @brief Correct the Voltage Limit from Temperature

*/

static int16_t voltageLimit(uint16_t limitV)
{
    int32_t voltageOffset = (1984*(6835-getTemperature())) >> 16;
    return limitV + voltageOffset;
}

/*--------------------------------------------------------------------------*/
/** @brief Access the Battery Charging State Variable

@param[in] battery: 0..NUM_BATS-1
*/

battery_Ch_States getBatteryChargingState(void)
{
    return batteryChargingState;
}


