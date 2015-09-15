/* STM32F1 SMPS Battery Charger

Defines the Measurement Task. This produces the basic measurements and avoids
all refined processing apart from scaling of raw measurements and computation
of battery parameters that need to be done in real time.

All measured quantities are stored locally for other tasks to collect.

The task performs all current and voltage measurements on the interfaces
and computes some battery parameters:
- battery resistance from step changes in current/voltage.
- accumulated charge passing to/from the batteries.

Each interface has a data structure holding the currents and voltages.

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

/* libopencm3 driver includes */
#include <libopencm3/stm32/adc.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "smps-batterycharger-arm-board-defs.h"
#include "smps-batterycharger-arm-hardware.h"
#include "smps-batterycharger-arm-objdic.h"
#include "smps-batterycharger-arm-measurement.h"

/* Local Prototypes */
static void initGlobals(void);

/* Local variables */
/* All variables times 256 except resistances times 65536. */
static int16_t weightAv;            /* Computed filter weights */
static int16_t y;                   /* Intermediate filter variable */
static int16_t lastBatteryCurrent;
static int16_t lastBatteryVoltage;
static int16_t batteryResistanceAv;
static int32_t accumulatedBatteryCharge;
static int16_t temperature;
static union InterfaceGroup currents;
static union InterfaceGroup voltages;

/*--------------------------------------------------------------------------*/
/* @brief Measurement Task

This measures the interface currents and voltages and passes them to other
tasks for processing and storage.

Certain other actions are needed at the fastest rate:
- Averaging of measurements;
- Detection of sudden current changes to find ohmic resistance;
- Estimation of battery parameters;
- Implementation of Coulomb Counting for SoC.

A/D measurements are performed in a single conversion and stored via DMA.
*/

void prvMeasurementTask(void *pvParameters)
{
    uint8_t i,j;
    int32_t av[3];
	uint8_t channel_array[3];
    initGlobals();

/* Setup the array of selected channels for conversion */
	channel_array[0] = ADC_CHANNEL_BATTERY_CURRENT;
	channel_array[1] = ADC_CHANNEL_BATTERY_VOLTAGE;
	channel_array[2] = ADC_CHANNEL_TEMPERATURE;
	adc_set_regular_sequence(ADC1, 3, channel_array);

/* Reset averages for first run */
	for (j = 0; j < 3; j++) av[j] = 0;

	while (1)
	{

/* A/D conversions */
/* Wait until the next tick cycle */
		vTaskDelay(getMeasurementDelay() );
/* Fire off a burst of conversions and average the results.
This averages out variations due to PWM provided they are not high frequency. */
        uint16_t burst = 0;
        for (burst=0; burst < N_SAMPLES; burst++)
        {
            adc_start_conversion_regular(ADC1);
/* Check if conversion ended */
            while (adcEOC() == 0) taskYIELD();
/* Sum over the sample set with scaling and offset for the interfaces and
temperature */
            av[0] += adcValue(0);
            av[1] += adcValue(1);
            av[2] += adcValue(2);
        }

/* Process the averaged results */
/* Compute averages from the burst, scale and offset to the real quantities,
and reset the averages for the next cycle. */
        currents.data = ((av[0]/N_SAMPLES-CURRENT_OFFSET)*CURRENT_SCALE)/4096;
        voltages.data = (av[1]/N_SAMPLES*VOLTAGE_SCALE+VOLTAGE_OFFSET)/4096;
        av[0] = 0;
        av[1] = 0;
        temperature = ((av[2]/N_SAMPLES-TEMPERATURE_OFFSET)*TEMPERATURE_SCALE)/4096;
        av[2] = 0;

/* Compute the battery's charge state by integration of current flow over time.
Currents are in amperes (x256). Divide by the measurement interval in seconds so
that charge is in Coulombs (x256). Limit charge to battery's capacity. Sign is
determined by the fact that positive current flows out of the batteries. */
        int32_t batteryCurrent = currents.dataArray.battery;
        accumulatedBatteryCharge -= (batteryCurrent*1000)/
                           (int32_t)(portTICK_RATE_MS*getMeasurementDelay());

/* Check if a significant change in battery current occurred (more than about
400mA) and use this to estimate the resistance of the battery.
Note that battery resistance is scaled by 65536 due to its low real value. */
        int16_t currentStep = abs(batteryCurrent-lastBatteryCurrent);
        int16_t batteryVoltage = voltages.dataArray.battery;
        if ((lastBatteryVoltage > 0) && (currentStep > 100))
        {
            uint16_t batteryResistance = 
                ((lastBatteryVoltage-batteryVoltage)<<16)
                 / (batteryCurrent-lastBatteryCurrent);
/* Apply a weighted IIR filter to estimate the average resistance value. */
/* First the weighted average of the weights (current steps) is needed. */
/* The forgetting factor getAlphaR() (<1) is multiplied by 256 to keep as an integer,
therefore the correction factors need to be divided back by 256. */
/* Seed the filter with the most recent measurements (rather than zero) */
            if (weightAv == 0) weightAv = currentStep;
            weightAv = weightAv +
                         ((getAlphaR()*(currentStep - weightAv)) >> 8);
/* Then apply the weighted IIR to the resistances, using y as the resistance
times the averaged weights. */
            int16_t sample = (currentStep * batteryResistance) >> 8;
            if (y == 0) y = sample;
            y = y + ((getAlphaR()*(sample - y)) >> 8);
            if (weightAv > 0)
                batteryResistanceAv = (256*y)/weightAv;
        }
        lastBatteryVoltage = batteryVoltage;
        lastBatteryCurrent = batteryCurrent;
    }
}

/*--------------------------------------------------------------------------*/
/** @brief Initialise Global Variables
*/

static void initGlobals(void)
{
    lastBatteryVoltage = 0;
    lastBatteryCurrent = 0;
    weightAv = 0;
    y = 0;
    batteryResistanceAv = 0;
    accumulatedBatteryCharge = 0;
}

/*--------------------------------------------------------------------------*/
/** @brief Access the Battery Resistance Average Estimate

@param[in] battery: 0..NUM_BATS-1
*/

int16_t getBatteryResistanceAv(void)
{
    return batteryResistanceAv;
}

/*--------------------------------------------------------------------------*/
/** @brief Access the Battery Accumulated Charge

When the charge accumulated has been read, it is reset to zero.
Need to protect this from interrupt and context switching.

Only one task should access this, otherwise there will be a major
synchronization problem. It is intended to pass the accumulation of charge
to the task that is monitoring the battery state.

@param[in] battery: 0..NUM_BATS-1
*/

int16_t getBatteryAccumulatedCharge(void)
{
    taskENTER_CRITICAL();
    int16_t accumulatedCharge = accumulatedBatteryCharge;
    accumulatedBatteryCharge = 0;
    taskEXIT_CRITICAL();
    return accumulatedCharge;
}

/*--------------------------------------------------------------------------*/
/** @brief Access the Measured Battery Current

@param[in] battery: 0..NUM_BATS-1
*/

int16_t getBatteryCurrent(void)
{
    return currents.dataArray.battery;
}

/*--------------------------------------------------------------------------*/
/** @brief Access the Measured Battery Voltage

@param[in] battery: 0..NUM_BATS-1
*/

int16_t getBatteryVoltage(void)
{
    return voltages.dataArray.battery;
}

/*--------------------------------------------------------------------------*/
/** @brief Access the Temperature

*/

int32_t getTemperature(void)
{
    return temperature;
}

