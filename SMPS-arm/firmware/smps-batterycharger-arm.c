/**
@mainpage SMPS Battery Charger
@version 1.0.0
@author Ken Sarkies (www.jiggerjuice.info)
@date 1 February 2014
@brief An SMPS full H-bridge buck-boost Battery Charger on ARM

This is an implementation of an SMPS synchronous buck regulator in the full
H-bridge configuration, using p-type MOSFETs for the high-side switches.
A timer is used in fast PWM mode. The PWM output and inverted output for two
compare registers control the four MOSFETs with deadtime. The voltage
and current feedback is converted internally and used to vary the duty cycle
of the enable signals.

The implementation is designed to be configurable to a range of SMPS
architectures from the simplest single switch buck to full H-Bridge.

A range of protection functions is implemented, including current overload,
(which must be done in hardware using a comparator), short-circuit detection,
brown-out and watchdog timer resets.

The A/D converter is operated at its maximum rate of 14MHz which gives a
resolution of 12 bits. Conversion time is 1 microsecond.
The reference voltage used is the internal analogue 3.3V supply (already
connected on the STAMP-ST32F103RET6 LQFP64 package).

The feedback circuits attenuate the input and output voltages by 0.145.
The load current is amplified by 10 to give 1V per ampere. So the scaling
factor is 14.5 for voltages and 100 for currents.

For the sealed lead-acid battery, current is limited to 0.3C and the voltage
to 2.5V per cell until the current drops to 0.05C at which time it switches
to 2.3V per cell.

The software is specifically designed for a 12V 7.5AH Diamec cell.
When a load current is detected, the charger turns off and the load is served
by the battery alone. The charger is reconnected when the load is disconnected.

Using STAMP-ST32F103RET6, VREF is connected to VDDA and VSSA is taken
from the 3.3V power.


@note
Software: ARM-GCC 4.5.2
@note
Target:   ARM ST32F103RET6
@note
Tested:   ST32F103RET6 at 72MHz
 */
/***************************************************************************
 *   Copyright (C) 2014 by Ken Sarkies                                     *
 *   ksarkies@trinity.asn.au                                               *
 *                                                                         *
 *   This file is part of the smps-batterycharger project                  *
 *                                                                         *
 *   smps is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   smps is distributed in the hope that it will be useful,               *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with smps if not, write to the                                  *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA.             *
 ***************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/* libopencm3 driver includes */
#include <libopencm3/cm3/nvic.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "smps-batterycharger-arm-hardware.h"
#include "smps-batterycharger-arm-objdic.h"
#include "smps-batterycharger-arm-comms.h"
#include "smps-batterycharger-arm-measurement.h"
#include "smps-batterycharger-arm-monitor.h"
#include "smps-batterycharger-arm-charger.h"
#include "smps-batterycharger-arm.h"

/*--------------------------------------------------------------------------*/

int main(void)
{
    setGlobalDefaults();
    prvSetupHardware();
    initComms();

/* Start the communications task. */
	xTaskCreate(prvCommsTask, (signed portCHAR * ) "Communications", \
                configMINIMAL_STACK_SIZE, NULL, COMMS_TASK_PRIORITY, NULL);

/* Start the measurement task. */
	xTaskCreate(prvMeasurementTask, (signed portCHAR * ) "Measurement", \
                configMINIMAL_STACK_SIZE, NULL, MEASUREMENT_TASK_PRIORITY, NULL);

/* Start the monitor task. */
	xTaskCreate(prvMonitorTask, (signed portCHAR * ) "Monitor", \
                configMINIMAL_STACK_SIZE, NULL, MONITOR_TASK_PRIORITY, NULL);

/* Start the charger task. */
	xTaskCreate(prvChargerTask, (signed portCHAR * ) "Charger", \
                configMINIMAL_STACK_SIZE, NULL, CHARGER_TASK_PRIORITY, NULL);

/* Start the scheduler. */
	vTaskStartScheduler();

/* Will only get here if there was not enough heap space to create the
idle task. */
	return -1;
}

/*-----------------------------------------------------------*/
/*----       FreeRTOS ISR Overrides in libopencm3     ----------------*/
/*-----------------------------------------------------------*/

extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);
extern void vPortSVCHandler( void );

/*-----------------------------------------------------------*/
void sv_call_handler(void)
{
  	vPortSVCHandler();
}

/*-----------------------------------------------------------*/

void pend_sv_handler(void)
{
  	xPortPendSVHandler();
}

/*-----------------------------------------------------------*/
/** @Brief Systick Interrupt Handler

*/

void sys_tick_handler(void)
{
  	xPortSysTickHandler();
}

