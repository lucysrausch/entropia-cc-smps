/**
@mainpage SMPS Project on ATTiny461
@version 1.0.0
@author Ken Sarkies (www.jiggerjuice.net)
@date 29 June 2010
@brief Code for an SMPS full H-bridge buck-boost Battery Charger on ATTiny261/461/861

This is an implementation of an SMPS synchronous buck regulator in the full
H-bridge configuration, using p-type MOSFETs for the high-side switches.
A timer is used in fast PWM mode. The PWM output and inverted output for two
compare registers control the four MOSFETs with deadtime. The voltage
and current feedback is converted internally and used to vary the duty cycle
of the enable signals.

To be used with the high PMOS version.

A range of protection functions is implemented, including current overload,
short-circuit detection, brown-out and watchdog timer resets.

The A/D converter is operated at its maximum rate of 1MHz which means that
the resolution is limited to only 8 bits. Conversion time is 13 microseconds.
The reference voltage used is the internal analogue 5V supply.

A 2.56V internal reference is used on all measurement channels. Differential
measurement would be very flexible but they require twice the settling time
of the single ended channels, so that the high conversion speed is effectively
nullified.

The feedback circuits attenuate the input and output voltages by 0.145.
The load current is amplified by 10 to give 1V per ampere. So the scaling
factor is 14.5 for voltages and 100 for currents.

For the sealed lead-acid battery, current is limited to 0.3C and the voltage
to 2.5V per cell until the current drops to 0.05C at which time it switches
to 2.3V per cell.

The software is specifically designed for a 12V 7.5AH Diamec cell.
When a load current is detected, the charger turns off and the load is served
by the battery alone. The charger is reconnected when the load is disconnected.

ADC3 Vin
ADC4 Vbatt
ADC5 Ibatt
ADC6 Iload

PB2 Buck Upper  iOC1B
PB3 Buck Lower   OC1B
PB4 Boost Upper iOC1D
PB6 Boost Lower  OC1D

The algorithm starts by measuring the battery voltage and then sets the voltage
and current to charge at a low rate (1 amp) with the boost upper MOSFET turned
off, being replaced by the Schottky diode. When the output voltage rises above
the measured battery voltage, and the current increases above 500mA, the MOSFET
is switched in to bypass the diode, thus reducing dissipated power. The normal
charging algorithm is implemented after that, but for safety if the output drops
below the initial measured battery voltage and the current drops below 100mA the
MOSFET is turned off again.

@note
Software: AVR-GCC 3.4.5
@note
Target:   ATTiny AVR with deadtime PWM and ADC functions
@note
Tested:   ATTiny461 at 8MHz.
 */
/***************************************************************************
 *   Copyright (C) 2010 by Ken Sarkies                                     *
 *   ksarkies@trinity.asn.au                                               *
 *                                                                         *
 *   This file is part of smps                                             *
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

#define F_CPU 8000000

#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>

#define TRUE 1
#define FALSE 0
/* Convenience macros (we don't use them all) */
#define  _BV(bit) (1 << (bit))
#define  inb(sfr) _SFR_BYTE(sfr)
#define  inw(sfr) _SFR_WORD(sfr)
#define  outb(sfr, val) (_SFR_BYTE(sfr) = (val))
#define  outw(sfr, val) (_SFR_WORD(sfr) = (val))
#define  cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define  sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#define  high(x) ((uint8_t) (x >> 8) & 0xFF)
#define  low(x) ((uint8_t) (x & 0xFF))

/* Set the TOP value to 128 (7 bits)
TOPSHIFT allows the A/D output to be scaled to match the PWM cycle period */
#define  TOPSHIFT 1
#define  TOP _BV(8-TOPSHIFT)

/* Define the float,and absorption phase voltages, the bulk current and the
float trigger current for a 7.5AH 12V Diamec SLA battery.
Currents are rescaled by 0.7 to match experimental observations. */
#define voltageScale 14.5
// Load current triggers to turn charger on and off
#define loadTrigHi      20
#define loadTrigLo      10
// Float and absorption phase voltage limits
#define floatVoltage   200
#define absVoltage     217
// Battery current trigger to go to float stage
#define floatTrigger    20
// Bulk phase current limit
#define bulkCurrent    157
// Current limit while boost upper MOSFET is off
#define slowCurrent     50
// Battery current trigger to turn on the boost upper MOSFET 
#define currentTrigger  20
// Deadtime correction when PWM mode is changed
#define deadtime        10

/*****************************************************************************/
/* Global Variables */

/* State variables for sequencing operations */
    uint8_t stage = 0;          /* Charging stage - setup, charge, float */
    uint8_t triggered = FALSE;

/* Set desired voltage and current to an initial state */
    uint8_t voltage = absVoltage;
    uint8_t current = bulkCurrent;
    uint8_t boostmax = (TOP >> 1);  /* Limit to prevent excessive voltages */
    uint8_t buckmax = TOP;      /* Fully on */

/* Initialise the main control variables. Start with all power off */
    uint8_t buck = 0;
    uint8_t boost = 0;
    uint8_t boostModified = 0;
/* Measured voltages and output current */
    uint8_t vbattOriginal = 0;
    uint8_t vbatt = 0;
    uint8_t vin = 0;
    uint8_t iload = 0;
    uint8_t ibatt = 0;
    uint8_t absDiff = 0;

/* Delay counters */
    uint16_t counter = 0;
    uint8_t secondCounter = 0;

/****************************************************************************/
/** @brief Main Program */

int main(void)
{
/* Stop watchdog timer while setting up */
    cbi(MCUSR,WDRF);
    wdt_disable();
/* Set ADC port pins PA4-7, PB6 (analogue inputs) as inputs */
    outb(DDRA,inb(DDRA) & 0x0F);
    outb(DDRB,(inb(DDRB) | 0x3C) & 0xBF); /* Outputs for four OC1 ports PB2-5 */
    outb(PORTB,(inb(PORTB) & 0xC3)); /* Clear output registers on PWM ports */
/* Setup the PWM ports and output compares to default values */
    uint8_t buck = 0;
    uint8_t boost = 0;
    outb(TCCR1A,0x11);      /* OC1A not used, OC1B as PWM with both outputs */
    outb(TCCR1B,0x01);      /* Clock with no prescale, noninverted PWM */
    outb(TCCR1C,0x19);      /* OC1D as PWM with secondary output suppressed */
    outb(TCCR1D,0x00);      /* Fast PWM */
    outb(TCCR1E,0x00);      /* Output Overrides */
    outb(OCR1C,TOP);        /* Set TOP value */
    outb(OCR1B,buck);
    outb(OCR1D,boost);
    outb(DT1,deadtime+(deadtime<<4));   /* Deadtime settings */
    outb(DIDR0,0xF8);       /* Disable digital input on AD3-6 and AREF */
    outb(DIDR1,0x40);       /* Disable digital input on AD9 */
/* Power down Timer 0 and USI */
    outb(PRR,0x06);
/* Start the Watchdog Timer */
    wdt_enable(WDTO_15MS);

/* Start A/D to get the battery voltage before we start. Wait for settled values. */
    do
    {
        outb(ADMUX,0xE4);       /* MUX ADC4, Reference 2.56V, left adjust */
        outb(ADCSRB,0x10);      /* Additional REFS2 bit */
        outb(ADCSRA,0xC3);      /* Start Conversion, clock divider 8 (1MHz) */
        while ((inb(ADCSRA) & _BV(ADSC)) > 0);  /* poll busy on ADC */
        vbattOriginal = inb(ADCH);
        if (vbattOriginal > vbatt) absDiff = vbattOriginal - vbatt;
        else absDiff = vbatt - vbattOriginal;
        vbatt = vbattOriginal;
    }
    while (absDiff == 0);

/* Main Loop */
    for(;;)
    {
        wdt_reset();            /* Reset the watchdog timer */

/* Start A/D for first conversion (output voltage feedback) every cycle */
        outb(ADMUX,0xE4);       /* MUX ADC4, Reference 2.56V, left adjust */
        outb(ADCSRB,0x10);      /* Additional REFS2 bit */
        outb(ADCSRA,0xC3);      /* Start Conversion, clock divider 8 (1MHz) */
        while ((inb(ADCSRA) & _BV(ADSC)) > 0);  /* poll busy on ADC */
        vbatt = inb(ADCH);

        if ((counter & 0x0F) == 1)
        {
/* Start A/D for second conversion (load current feedback) every 16 cycles */
            outb(ADMUX,0xE6);   /* MUX ADC6, reference 2.56V, left adjust */
            outb(ADCSRB,0x10);  /* Additional REFS2 bit */
            outb(ADCSRA,0xC3);  /* Start Conversion, clock divider 8 (1MHz) */
            while ((inb(ADCSRA) & _BV(ADSC)) > 0);  /* poll busy on ADC */
            iload = inb(ADCH);
        }
        if ((counter & 0x0F) == 0x08)
        {
/* Start A/D for third conversion (input voltage feedforward) every 16 cycles */
            outb(ADMUX,0xE3);   /* MUX ADC3, Reference 2.56V, left adjust */
            outb(ADCSRB,0x10);  /* Additional REFS2 bit */
            outb(ADCSRA,0xC3);  /* Start Conversion, clock divider 8 (1MHz) */
            while ((inb(ADCSRA) & _BV(ADSC)) > 0);  /* poll busy on ADC */
            vin = inb(ADCH);
        }
        if ((counter & 0x01) == 0)
        {
/* Start A/D for fourth conversion (battery current feedback) every 2 cycles */
            outb(ADMUX,0xE5);   /* MUX AD5, reference 2.56V, left adjust */
            outb(ADCSRB,0x10);  /* Higher gain plus REFS2 and mux bits*/
            outb(ADCSRA,0xC3);  /* Start Conversion, clock divider 8 (1MHz) */
            while ((inb(ADCSRA) & _BV(ADSC)) > 0);  /* poll busy on ADC */
            ibatt = inb(ADCH);
        }

/**** This is the buck-boost control algorithm with voltage/current limits */

/* The SMPS algorithm increases buck to its maximum as long as the limit
trigger is not satisfied. Then it increases boost to its maximum. If the
condition is still not satisfied, it continues to increase boost, but
only very slowly. If the condition is satisfied, start reducing boost
till it hits a small value. After that reduce the buck.
However we won't reduce buck too far lest it turn off the current. */
        if ((vbatt < voltage) && (ibatt < current))
        {
            if (buck < buckmax) buck++;
            else if ((boost < boostmax) && ((counter & 0x1F) == 0)) boost++;
        }
        else
        {
            if (boost > 0) boost--;
            else if (buck > 0x10) buck--;
        }

/* This is the charger algorithm */

/* If the output voltage exceeds the original measured battery voltage, and the
current starts to flow into the battery, switch on the boost upper MOSFET.
Otherwise set to isolate it. The current is limited in this case to protect
the Schottky diode until the MOSFET is switched on.
Don't do this if the load has triggered the charger off, otherwise it comes back on. */
        boostModified = boost;
        if (! triggered)
        {
            if ((vbatt > vbattOriginal) && (ibatt > currentTrigger))
            {
                outb(TCCR1C,0x15);      /* OC1D as PWM with both outputs */
                current = bulkCurrent;  /* Set back to full current limit */
            }
            else
            {
                outb(TCCR1C,0x19);      /* OC1D as PWM with secondary output suppressed */
                current = slowCurrent;  /* limit the current until ready to go */
/* When boost upper MOSFET is suppressed, deadtime is not applied, so we must do it here. */
                if (boostModified > deadtime) boostModified -= deadtime;
                else boostModified = 0;
            }
        }

/* If the battery current drops to less than 0.05C, enter the float stage. Drop the
voltage target to 2.3V per cell. Start checking after startup has settled. */
        if ((counter > 0xEFFF) && (ibatt <= floatTrigger))
        {
            voltage = floatVoltage; /* drop to 2.3V per cell */
        }

/* If the load starts to draw current, turn off the charger. The battery will supply
the load. When the load current drops again, turn the charger back on to full charge. */
        if ((iload > loadTrigHi) && ((counter & 0x1F) == 0))
        {
            if (! triggered)
            {
                outb(TCCR1A,0x01);  /* All PWMs are off */
                outb(TCCR1C,0x01);
                buck = 0;           /* Turn off charger altogether */
                boost = 0;
                boostModified = 0;
                counter = 0;        /* Prevent excessive switching */
                sbi(PORTB,6);
            }
            triggered = TRUE;
        }
        if ((iload < loadTrigLo) && ((counter & 0x1F) == 0))
        {
            if (triggered)
            {
                outb(TCCR1A,0x11);  /* OC1A not used, OC1B as PWM with both outputs */
                outb(TCCR1C,0x19);  /* OC1D as PWM with boost upper MOSFET suppressed */
                voltage = absVoltage;   /* Put charging back to absorption phase */
                counter = 0;        /* Prevent excessive switching */
                cbi(PORTB,6);
            }
            triggered = FALSE;
        }

/**** Adjust the computed duty cycle */
        outb(OCR1B,buck);
        outb(OCR1D,boostModified);

        counter++;
 
   }

}
/****************************************************************************/
