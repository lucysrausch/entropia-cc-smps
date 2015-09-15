/* STM32F1 Battery Charger

Monitoring Task

This task accesses the various measured and estimated parameters of the
batteries, loads and panel to make decisions about switch settings and
load disconnect/reconnect.

The decisions determine how to connect loads and solar panel to the different
batteries in order to ensure continuous service and long battery life. The
batteries are connected to the charger according to the low level of SoC,
to the loads according to the high level of SoC, and are isolated for a
period of time to obtain a reference measurement of the SoC from the open
circuit voltage. Loads are progressively disconnected as the batteries pass to
the low and critically low charge states.

On external command the interface currents and SoC of the batteries will be
calibrated.

On external command the task will track and manage battery to load and
battery charging automatically. Tracking will always occur but switches will
not be set until auto-tracking is enabled.

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
#include <stdlib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "smps-batterycharger-arm-board-defs.h"
#include "smps-batterycharger-arm-hardware.h"
#include "smps-batterycharger-arm-objdic.h"
#include "smps-batterycharger-arm-lib.h"
#include "smps-batterycharger-arm-comms.h"
#include "smps-batterycharger-arm-measurement.h"
#include "smps-batterycharger-arm-charger.h"
#include "smps-batterycharger-arm-monitor.h"

/* Local Prototypes */
static void initGlobals(void);
static int16_t computeSoC(uint32_t voltage, uint32_t temperature, battery_Type type);

/* Local Variables */

/*--------------------------------------------------------------------------*/
/* @brief Monitoring Task

This task runs over long times, monitoring the state of the batteries,
switching them out at intervals and resetting the state of charge estimation
algorithm during lightly loaded periods.
*/

void prvMonitorTask(void *pvParameters)
{
    initGlobals();

	vTaskDelay(MONITOR_STARTUP_DELAY );

	while (1)
	{

/*------------- RECORD AND REPORT STATE --------------*/
/* Send off the current set of measurements */
/* Send out battery terminal measurements. */
        dataMessageSendLowPriority("dB",getBatteryCurrent(),getBatteryVoltage());
/* Send out temperature measurement. */
        sendResponse("dT",getTemperature());

/* Wait until the next tick cycle */
		vTaskDelay(getMonitorDelay());
    }
}

/*--------------------------------------------------------------------------*/
/** @brief Initialise Global Variables to Defaults

*/

static void initGlobals(void)
{
}

/*--------------------------------------------------------------------------*/
/** @brief Compute SoC from OC Battery Terminal Voltage and Temperature

This model covers the Gel and Wet cell batteries.
Voltage is referred to the value at 48.9C so that one table can be used.
Refer to documentation for the model formula derived.

@param uint32_t voltage. Measured open circuit voltage. Volts times 256
@param uint32_t temperature. Temperature degrees C times 256
@return int16_t Percentage State of Charge times 256.
*/

static int16_t computeSoC(uint32_t voltage, uint32_t temperature, battery_Type type)
{
    int32_t soc;
    uint32_t v100, v50, v25;
    if (type == wetT) v100 = 3242;
    else v100 = 3280;
/* Difference between top temperature 48.9C and ambient, times 64. */
    uint32_t tDiff = (12518-temperature) >> 2;
/* Correction factor to apply to measured voltages, times 65536. */
    uint32_t vFactor = 65536-((42*tDiff*tDiff) >> 20);
/* Open circuit voltage referred to 48.9C */
    uint32_t ocv = (voltage*65536)/vFactor;
/* SoC for Wet cell and part of Gel cell */
    soc = (100*(65536 - 320*(v100-ocv))) >> 8;
    if (type == gelT)
    {
        v50 = 3178;
        if (ocv < v50)
        {
            v25 = 3075;
            if (ocv > v25) soc = (100*(65536 - 640*(v50-ocv))) >> 8;
            else soc = (100*(65536 - 320*(v25-ocv))) >> 8;
        }
    }
    if (soc > 100*256) soc = 100*256;
    return soc;
}

