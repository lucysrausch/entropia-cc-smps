/* STM16F1 SMPS Battery Charger

This file defines the CANopen object dictionary variables made available to an
external PC and to other processing modules which may be CANopen devices or
tasks running on the same microcontroller.

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

#include "FreeRTOS.h"

#ifndef SMPS_BATTERYCHARGER_OBJDIC_H_
#define SMPS_BATTERYCHARGER_OBJDIC_H_

typedef enum {wetT=0, gelT=1, agmT=2} battery_Type;
typedef enum {normalF=0, lowF=1, criticalF=2} battery_Fl_States;
typedef enum {loadedO=0, chargingO=1, isolatedO=2, missingO=3} battery_Op_States;
typedef enum {bulkC=0, absorptionC=1, floatC=2, equalizationC=3} battery_Ch_States;

/* Represent the measured data arrays in a union as separate or combined */
struct Interface
{
    int16_t battery;
};

/* These offsets are for battery 1-3, load 1-2 and panel, in order */
union InterfaceGroup
{
    int16_t data;
    struct Interface dataArray;
};

/*--------------------------------------------------------------------------*/
/****** Object Dictionary Items *******/
/* Configuration items, updated externally, are stored to NVM */

/*--------------------------------------------------------------------------*/
struct Config
{
/* Battery characteristics and model parameters */
    uint16_t batteryCapacity;
    battery_Type batteryType;
    int16_t absorptionVoltage;
    int16_t floatVoltage;
    int16_t floatStageCurrentScale;
    int16_t bulkCurrentLimitScale;
    int16_t alphaR;         /* forgetting factor for battery resistance estimator */
    int16_t alphaV;         /* forgetting factor for battery voltage smoothing */
    int16_t alphaC;         /* forgetting factor for battery current smoothing */
/* Delay Variables */
    portTickType measurementDelay;
    portTickType monitorDelay;
    portTickType calibrationDelay;
    portTickType chargerDelay;
};

/* Map the configuration data also as a block of words.
Block size equal to a FLASH page (2048 bytes) to avoid erase problems.
Needed for reading and writing from Flash. */
#define CONFIG_BLOCK_SIZE       2048
union ConfigGroup
{
    uint8_t data[CONFIG_BLOCK_SIZE];
    struct Config config;
};

/*--------------------------------------------------------------------------*/
/* Prototypes */
/*--------------------------------------------------------------------------*/

void setGlobalDefaults(void);
uint32_t writeConfigBlock(void);

battery_Type getBatteryType(void);
int16_t getBatteryCapacity(void);
int16_t getBulkCurrentLimit(void);
int16_t getFloatStageCurrent(void);
int16_t getAbsorptionVoltage(void);
int16_t getFloatVoltage(void);
int16_t getAlphaV(void);
int16_t getAlphaC(void);
int16_t getAlphaR(void);
portTickType getChargerDelay(void);
portTickType getMonitorDelay(void);
portTickType getMeasurementDelay(void);

#endif

