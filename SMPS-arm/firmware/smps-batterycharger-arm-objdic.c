/* STM32F1 SMPS Battery Charger

This file defines the CANopen object dictionary variables made available to an
external PC and to other processing modules which may be CANopen devices or
tasks running on the same microcontroller.

Note these definitions cannot be placed in the header file.
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

#include "FreeRTOS.h"

#include "smps-batterycharger-arm-objdic.h"
#include "smps-batterycharger-arm-hardware.h"

/* The rate at which the charger algorithm is updated (1ms ticks) */
#define CHARGER_DELAY               ( ( portTickType ) 500 / portTICK_RATE_MS )

/* The rate at which the monitoring is updated (1ms ticks) */
#define MONITOR_DELAY               ( ( portTickType ) 1000*1 / portTICK_RATE_MS )

/* The default rate at which the samples are taken (1ms ticks) */
#define MEASUREMENT_DELAY           ( ( portTickType ) 500 / portTICK_RATE_MS )

union ConfigGroup configData;

/*--------------------------------------------------------------------------*/
/** @brief Initialise Global Configuration Variables

This determines if configuration variables are present in NVM, and if so
reads them in. The first entry is checked against a preprogrammed value to
determine if the block is a valid configuration block. This allows the program
to determine whether to use the block stored in FLASH or to use defaults.
*/

void setGlobalDefaults(void)
{
    configData.config.measurementDelay = MEASUREMENT_DELAY;
    configData.config.chargerDelay = CHARGER_DELAY;
    configData.config.monitorDelay = MONITOR_DELAY;
    configData.config.alphaR = 100;           /* about 0.4 */
    configData.config.alphaV = 256;           /* No Filter */
    configData.config.alphaC = 180;           /* about 0.7, only for detecting float state. */
    configData.config.batteryCapacity = 7;
    configData.config.batteryType = agmT;
/* Set default battery parameters */
    if (configData.config.batteryType == wetT)
    {
        configData.config.absorptionVoltage = 3712;        /* 14.5V */
        configData.config.floatVoltage = 3379;             /* 13.2V */
    }
    else if (configData.config.batteryType == agmT)
    {
        configData.config.absorptionVoltage = 3660;        /* 14.3V */
        configData.config.floatVoltage = 3481;             /* 13.6V */
    }
    else if (configData.config.batteryType == gelT)
    {
        configData.config.absorptionVoltage = 3610;        /* 14.1V */
        configData.config.floatVoltage = 3532;             /* 13.8V */
    }
    configData.config.floatStageCurrentScale = 50;
    configData.config.bulkCurrentLimitScale = 5;
}

/*--------------------------------------------------------------------------*/
/** @brief Provide the Battery Type

@param[in] battery: 0..NUM_BATS-1
*/

battery_Type getBatteryType(void)
{
    return configData.config.batteryType;
}

/*--------------------------------------------------------------------------*/
/** @brief Provide the Battery Capacity

@param[in] battery: 0..NUM_BATS-1
*/

int16_t getBatteryCapacity(void)
{
    return configData.config.batteryCapacity;
}

/*--------------------------------------------------------------------------*/
/** @brief Provide the Battery Bulk Current Limit

@param[in] battery: 0..NUM_BATS-1
*/

int16_t getBulkCurrentLimit(void)
{
    return configData.config.batteryCapacity*256/
            configData.config.bulkCurrentLimitScale;
}

/*--------------------------------------------------------------------------*/
/** @brief Provide the Battery Float Current Cutoff

@param[in] battery: 0..NUM_BATS-1
*/

int16_t getFloatStageCurrent(void)
{
    return configData.config.batteryCapacity*256/
            configData.config.floatStageCurrentScale;
}

/*--------------------------------------------------------------------------*/
/** @brief Provide the Absorption Phase Voltage Limit

@param[in] battery: 0..NUM_BATS-1
*/

int16_t getAbsorptionVoltage(void)
{
    return configData.config.absorptionVoltage;
}
/*--------------------------------------------------------------------------*/
/** @brief Provide the Float Phase Voltage Limit

@param[in] battery: 0..NUM_BATS-1
*/

int16_t getFloatVoltage(void)
{
    return configData.config.floatVoltage;
}

/*--------------------------------------------------------------------------*/
/** @brief Provide the Forgetting factor for Charger Voltage Smoothing

*/

int16_t getAlphaV(void)
{
    return configData.config.alphaV;
}

/*--------------------------------------------------------------------------*/
/** @brief Provide the Forgetting factor for Charger Current Smoothing

*/

int16_t getAlphaC(void)
{
    return configData.config.alphaC;
}

/*--------------------------------------------------------------------------*/
/** @brief Provide the Forgetting factor for Resistance Estimator Smoothing

*/

int16_t getAlphaR(void)
{
    return configData.config.alphaR;
}

/*--------------------------------------------------------------------------*/
/** @brief Provide the Charging Time Interval

This is the time between decision updates.
*/

portTickType getChargerDelay(void)
{
    return configData.config.chargerDelay;
}

/*--------------------------------------------------------------------------*/
/** @brief Provide the Monitor Time Interval

This is the time between decision updates.
*/

portTickType getMonitorDelay(void)
{
    return configData.config.monitorDelay;
}

/*--------------------------------------------------------------------------*/
/** @brief Provide the Measurement Time Interval

This is the time between measurement updates.
*/

portTickType getMeasurementDelay(void)
{
    return configData.config.measurementDelay;
}

