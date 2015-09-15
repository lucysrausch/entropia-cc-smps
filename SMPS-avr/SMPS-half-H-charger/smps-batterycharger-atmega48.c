/**
@mainpage SMPS Fast SLA Battery Charger Project on ATMega48
@version 1.0.0
@author Ken Sarkies (www.jiggerjuice.net)
@date 29 June 2010
@brief Code for an SMPS half H-bridge buck-boost Battery Charger on ATMega48

This is an implementation of an SMPS half H-bridge buck-boost regulator in
which the ATMega48 16 bit timer is used in phase correct PWM mode with two
compare outputs to generate the two MOSFET enable signals. The voltage and
current feedback is converted internally and used to vary the duty cycle of
the enable signals. This is not a synchronous circuit so the two signals do
not need to be synchronized.

A range of protection functions is implemented, including current overload,
short-circuit detection, brown-out and watchdog timer resets.

The A/D converter is operated at its maximum rate of 1MHz which means that
the resolution is limited to only 8 bits. Conversion time is 13 microseconds.
The reference voltage used is the internal analogue 5V supply.

The feedback circuits attenuate the input and output voltages by 0.267, and
provide 1V per ampere output current. For the sealed lead-acid battery it
limits current ot 0.4C and the voltage to 2.5V per cell until the current
drops to 0.05C at which time it switches to 2.3V per cell. This low trigger
point is about the lowest possible for the A/D converter resolution, being
6 bits for a 6AH cell.

The program uses the following ATMega48 ports for the user interface part:

PIND0,1 is a battery capacity selection switch (4 values TBD)
PIND3 is the charge start/restart switch

LEDs
PD4 tells if in stage 0 or 1
PB6 tells if 6V battery
PB7 tells if 12V battery
PD5 tells if current limit exceeded (bulk charge phase).
PD6 tells if voltage limit exceeded (absorption charge phase).
PD7 tells if the charging has finished (float charge stage).

PD2 is a control output for the reversed battery blocking MOSFET.

@note
Software: AVR-GCC 3.4.5
@note
Target:   ATMega AVR with PWM and ADC functions
@note
Tested:   ATMega48 at 8MHz.
 */
/***************************************************************************
 *   Copyright (C) 2010 by Ken Sarkies                                     *
 *   ksarkies@trinity.asn.au                                               *
 *                                                                         *
 *   This file is part of smps-batterycharger-atmega48                     *
 *                                                                         *
 *   This is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This software is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with the software. If not, write to the:                        *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA.             *
 ***************************************************************************/

#include <avr/io.h>
#include <avr/wdt.h>

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

/* Set the TOP value to 64 (6 bits)
TOPSWITCH allows the A/D output to be scaled to match the PWM cycle period */
#define  TOPSHIFT 2
#define  TOP _BV(8-TOPSHIFT)

/* 8 cycles of deadtime needed to ensure charge pump remains active.*/
#define  DEADTIME (TOP >> 3)

/*****************************************************************************/

uint8_t buttonPressed(void);

/*****************************************************************************/
/* Global Variables */

/* State variables for sequencing operations */
    uint8_t stage = 0;          /* Charging stage - setup, charge, float */

/* Set desired voltage and current to an initial state */
    uint8_t voltage = 103;      /* 7.5V initial limit */
    uint8_t current = 82;       /* 1.6A for a 4AH battery */
    uint8_t boostmax = 32;      /* Limit to prevent excessive voltages */
    uint8_t buckmax = TOP - DEADTIME;   /* cannot turn fully on */

/* Initialise the main control variables. Start with all power off */
    uint8_t buck = 0;
    uint8_t boost = 0;
/* Measured voltages and output current */
    uint8_t vin = 0xA4;         /* Set vin to about 12V */
    uint8_t iout = 0;
    uint8_t vout = 0;

/* The counter allows A/D conversions of input voltage and output current
to be made every second cycle, thus reducing the number of conversions
required each cycle to be two rather than three. The output voltage
is sampled every cycle to keep ripple down. The counter also is used
to slow down certain operations, such as increasing boost voltage.*/
    uint16_t counter = 0;

/* Button debounce */
    uint8_t debounceCounter = 0;
    uint8_t buttonDown = 0;

/****************************************************************************/
/** @brief Main Program */

int main(void)
{
/* Stop watchdog timer while setting up */
    cbi(MCUSR,WDRF);
    wdt_disable();
/* Set first three ADC port C pins (analogue inputs) as inputs */
    outb(DDRC,inb(DDRC) & 0xF8);
    outb(ADCSRB,0x00);
/* Setup the PWM ports and output compares to default values */
    outb(DDRB,inb(DDRB) | 0xC6);/* Outputs for OC1 ports, battery LEDs */
    outb(TCCR1A,0xF2);          /* Set OC1A, Set OC1B on upcount, set Phase- */
    outb(TCCR1B,0x11);          /*  -Correct Mode using ICR1, clock with no prescale*/
    outb(TCCR1C,0x00);          /* not used */
    outb(ICR1H,0x00);           /* Set TOP value */
    outb(ICR1L,TOP);
    outb(OCR1AH,0x00);          /* Set output compares */
    outb(OCR1AL,buck);          /* Buck duty cycle */
    outb(OCR1BH,0x00);
    outb(OCR1BL,boost);         /* Boost duty cycle */
/* Setup Port D as control inputs (0,1,3) status outputs (4-7) and blocking control (2)*/
    outb(DDRD,0xF4);
/* Power down Timers 0,2, TWI, SPI and USART */
    outb(PRR,0xE6);
/* Turn off the reversed battery blocking MOSFET */
    cbi(PORTD, 2);
/* Start the Watchdog Timer */
    wdt_enable(WDTO_15MS);

/****************************************************************************/
/* Main Loop */
/* The variable "stage" puts the loop into a precharge (0) or charging (1) state.
Start off in precharge to detect the battery presence and determine its voltage.
The capacity switches are read and the current limit is set.
The A/D converters are read. */
/****************************************************************************/
    for(;;)
    {
        wdt_reset();            /* Reset the watchdog timer */

/* Read PORTD for switch status. The capacity switch is always monitored to allow
it to be changed on the fly. */

        uint8_t capacity = inb(PIND) & 0x03;
        current = 32;                           /* 1AH */
        if (capacity == 1) current = 70;        /* 4AH */
        else if (capacity == 2) current =104;   /* 6AH */
        else if (capacity == 3) current =156;   /* 9AH */

/* Start A/D for first conversion (output voltage feedback) every cycle */
        outb(ADMUX,0x60);       /* MUX AD0, AVcc reference 5V, left adjust */
        outb(ADCSRA,0xC3);      /* Start Conversion, clock divider 8 (1MHz) */
        while ((inb(ADCSRA) & _BV(ADSC)) > 0);  /* poll busy on ADC */
        vout = inb(ADCH);

        if ((counter & 0x01) == 0)
        {
/* Start A/D for second conversion (output current feedback) alternate cycle */
            outb(ADMUX,0x61);   /* MUX AD1, AVcc reference 5V, left adjust */
            outb(ADCSRA,0xC3);  /* Start Conversion, clock divider 8 (1MHz) */
            while ((inb(ADCSRA) & _BV(ADSC)) > 0);  /* poll busy on ADC */
            iout = inb(ADCH);
        }
        else
        {
/* Start A/D for third conversion (input voltage feedforward) alternate cycle */
            outb(ADMUX,0x62);   /* MUX AD2, AVcc reference 5V, left adjust */
            outb(ADCSRA,0xC3);  /* Start Conversion, clock divider 8 (1MHz) */
            while ((inb(ADCSRA) & _BV(ADSC)) > 0);  /* poll busy on ADC */
            vin = inb(ADCH);
        }

/****************************************************************************/
/**** Stage 0 is a pre-charging state used to compute battery presence and
terminal voltage. */
        if (stage == 0)
        {
            buck = 0;
            boost = 0;          /* Turn off the power */

/* Keep the reversed battery blocking MOSFET off */
            cbi(PORTD, 2);

/* Flash the pre-charger state indicator */
            if ((counter & 0x4000) == 0) cbi(PORTD, 4); else sbi(PORTD, 4);

/* Clear the charging indicators */
            cbi(PORTD, 5);
            cbi(PORTD, 6);
            cbi(PORTD, 7);

/* Select 12V charging if battery voltage is greater than 8V */
            if (vout > 110)
            {
                voltage = 206;
                sbi(PORTB, 7);  /* 12V */
                cbi(PORTB, 6);
            }
/* Select 6V charging if battery voltage is between 4V and 8V */
            else if (vout > 55)
            {
                voltage = 103;
                sbi(PORTB, 6);  /* 6V */
                cbi(PORTB, 7);
            }

/* Otherwise we probably have no battery present. Force to stay in
stage 0 and flash the voltage LEDs*/
            else
            {
                voltage = 0;
                if ((counter & 0x2000) == 0) cbi(PORTB, 6); else sbi(PORTB, 6);
                if ((counter & 0x2000) == 0) sbi(PORTB, 7); else cbi(PORTB, 7);
            }

/* Test the start switch and if pressed and the battery is OK, proceed to
stage 1 to start charging.*/
            if ((vout > 55) && ((inb(PIND) & _BV(3)) == 0))
            {
                stage = 1;
                counter = 0;
                sbi(PORTD, 2); /* Turn on the reversed battery blocking MOSFET */
            }
        }

/****************************************************************************/
/**** Stage 1 is the charging phase, and stage 2 is the final float stage. */
        else
        {
/* Check for the output voltage very high and battery current very low.
The battery maximum, for a 12V battery, is going to be 15V (206). When
disconnected the voltage should rise well above this. We can detect this to
determine disconnect and so drop back out to the pre-charge stage. */
            if ((vout > 250) && (iout < 0x02)) stage = 0;
            else
            {
/* Set the charger state indicator fully on*/
                sbi(PORTD, 4);

/**** This is the buck-boost control algorithm with voltage/current limits */

/* Start by computing a maximum boost duty cycle according to the input
voltage. This will be used when the buck is less than full on */
                boostmax = TOP-(vin >> 2);  /* 64 - vin/4 */

/* The algorithm increases buck to its maximum as long as the limit
trigger is not satisfied. Then it increases boost to its maximum. If the
condition is still not satisfied, it continues to increase boost, but
only very slowly. If the condition is satisfied, start reducing boost
till it hits a small value. We do not reduce this to zero so that when
the battery is disconnected the output voltage will rise to a high enough
value to detect that the battery is gone. After that reduce the buck.
However we won't reduce buck too far lest it turn off the current. */
                if ((vout < voltage) && (iout < current))
                {
                    if (buck < buckmax) buck++;
                    else if ((boost < boostmax) && ((counter & 0x0F) == 0)) boost++;
                }
                else
                {
                    if (boost > 1) boost--;
                    else if (buck > 0x10) buck--;
                }

/* Set the appropriate indicators if still in the bulk or absorption stage */
                if (stage == 1)
                {
/* Keep the float charge indicator clear */
                    cbi(PORTD, 7);
/* If overvoltage then we are in absorption phase */
                    if (vout > voltage) sbi(PORTD, 6); else cbi(PORTD, 6);
/* If overcurrent we are in bulk charge phase */
                    if (iout > current) sbi(PORTD, 5); else cbi(PORTD, 5);
                }

/**** This is the SLA charger algorithm. */

/* If the average current drops to less than 0.05C, enter the float stage. Drop the 
voltage target to 2.3V per cell. Start checking after startup has settled. */
                if ((counter > 0xEFFF) && (iout <= (current >> 4)))
                {
                    stage = 2;          /* Move to float stage and stay there */
                    cbi(PORTD, 5);
                    cbi(PORTD, 6);
                    sbi(PORTD, 7);
                    voltage = 94;       /* drop to 2.3V per cell */
                }
            }
        }

/**** Adjust the computed duty cycle */
        outb(OCR1AH,0x00);              /* Set the upper PWM bytes to zero */
        outb(OCR1BH,0x00);
        outb(OCR1AL,buck);
        outb(OCR1BL,boost);

        counter++;
 
   }
}
/****************************************************************************/
/* Run a debounce check on the button */

uint8_t buttonPressed(void)
{
    uint8_t buttonSense = (inb(PIND) & _BV(3));
    if ((buttonDown == 0) != (buttonSense == 0))
    {
        if (debounceCounter++ > 100)
        {
            debounceCounter = 0;
            if (buttonDown == 0) buttonDown = 1; else buttonDown = 0;
        }
    }
    else debounceCounter = 0;
    return buttonDown;
}
/****************************************************************************/

