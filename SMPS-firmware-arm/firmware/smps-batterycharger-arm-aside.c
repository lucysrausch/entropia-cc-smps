/**
@mainpage SMPS Project on ARM
@version 1.0.0
@author Ken Sarkies (www.jiggerjuice.net)
@date 2 May 2012
@brief Code for an SMPS full H-bridge buck-boost Battery Charger on ARM

This is an implementation of an SMPS synchronous buck regulator in the full
H-bridge configuration, using p-type MOSFETs for the high-side switches.
A timer is used in fast PWM mode. The PWM output and inverted output for two
compare registers control the four MOSFETs with deadtime. The voltage
and current feedback is converted internally and used to vary the duty cycle
of the enable signals.

To be used with the high PMOS version.

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

Ports PA8 (OC1), PA9 (OC2), PB13 (OC1N), PB14 (OC2N) are used for the for PWM.
Port PB12 is a BKIN signal used by the overcurrent protection.
PB8 is a reference signal.

Available pins: PA0-15, PB0-15, PC0-15, PD2

PA0 ADC0 Vin
PA1 ADC1 Vbatt
PA2 ADC2 Ibatt
PA3 ADC3 Iload

PB13 Timer 1 CH1N:  Buck Upper
PA8  Timer 1 CH1:   Buck Lower
PB14 Timer 1 CH2N:  Boost Upper
PA9  Timer 1 CH2:   Boost Lower

The algorithm starts by measuring the battery voltage and then sets the voltage
and current to charge at a low rate (1 amp) with the boost upper MOSFET turned
off, being replaced by the Schottky diode. When the output voltage rises above
the measured battery voltage, and the current increases above 500mA, the MOSFET
is switched in to bypass the diode, thus reducing dissipated power. The normal
charging algorithm is implemented after that, but for safety if the output drops
below the initial measured battery voltage and the current drops below 100mA the
MOSFET is turned off again.

@note
Software: ARM-GCC 4.5.2
@note
Target:   ARM ST32F103RET6
@note
Tested:   ST32F103RET6 at 72MHz
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

#include <libopencm3/stm32/iwdg.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#include "buffer.h"

#define TRUE 1
#define FALSE 0

#define PWM_PERIOD     1152*5

/* USART */
#define BAUDRATE        115200

/* Define the float,and absorption phase voltages, the bulk current and the
float trigger current for a 7.5AH 12V Diamec SLA battery.
Currents are rescaled by 0.7 to match experimental observations. */
#define VOLTAGE_SCALE	 14.5
// Load current triggers to turn charger on and off
#define LOAD_TRIG_HI     20
#define LOAD_TRIG_LO     10
// Float and absorption phase voltage limits
#define FLOAT_VOLTAGE   200
#define ABS_VOLTAGE     217
// Battery current trigger to go to float stage
#define FLOAT_TRIGGER    20
// Bulk phase current limit
#define BULK_CURRENT    157
// Current limit while boost upper MOSFET is off
#define SLOW_CURRENT     50
// Battery current trigger to turn on the boost upper MOSFET 
#define CURRENT_TRIGGER  20
// Deadtime correction when PWM mode is changed
#define DEADTIME        100
// Filter coefficient power of 2
#define K                 8

/*****************************************************************************/
/* Local Prototypes */

/****************************************************************************/

static void parseCommand(char* line);
static void clockSetup(void);
static void watchdogSetup(void);
static void adcSetup(void);
static void dacSetup(void);
static void timerSetup(void);
static void usartSetup(void);
void usart_print_string(char *ch);
void usart_print_hex(uint16_t value);
void usart_print_int(int value);
void print_register(uint32_t reg);
void intToAscii(int32_t value, char* buffer);
int32_t asciiToInt(char* buffer);

/*****************************************************************************/
/* Global Variables */

/****************************************************************************/

#define BUFFER_SIZE 128

uint8_t send_buffer[BUFFER_SIZE+3];
uint8_t receive_buffer[BUFFER_SIZE+3];
uint16_t dutyCycle = 0;
uint16_t period = 0;

/****************************************************************************/
/** @brief Main Program */

int main(void)
{
    static char line[80];
    static uint8_t characterPosition = 0;
    typedef enum {off, full, half} State;
    State state = full;
/* Initialise the main control variables. Start with all power off */
/* buck and boost are the controls for the buck and boost regulator parts,
set in the timer output comparators, and are upper limited by the period. */
    uint16_t buck = 0;
    uint16_t boost = 0;
/* Measured voltages and output current */
    uint8_t vbatt = 0;
    uint8_t vin = 0;
    uint8_t iload = 0;
    uint8_t ibatt = 0;
/* Filtered average estimators */
    uint16_t cvbattAvg = 0;
    uint16_t cvinAvg = 0;
    uint16_t ciloadAvg = 0;
    uint16_t cibattAvg = 0;
    uint8_t vbattAvg = 0;
    uint8_t vinAvg = 0;
    uint8_t iloadAvg = 0;
    uint8_t ibattAvg = 0;

/* INITIALIZATION PHASE */

    period = PWM_PERIOD;
    dutyCycle = period/2;
    buffer_init(receive_buffer,64);
    buffer_init(send_buffer,64);
    clockSetup();
    watchdogSetup();
    adcSetup();
    timerSetup();
    usartSetup();
    dacSetup();

    usart_print_string("SMPS Test");
	usart_enable_tx_interrupt(USART1);

    dac_load_data_buffer_single(4096/2,RIGHT12,CHANNEL_1);

/* MAIN LOOP */

    for(;;)
    {
        iwdg_reset();              /* Reset the watchdog timer */

/* Build a command line string before actioning. the task will block
indefinitely waiting for input. */
        if (buffer_input_available(receive_buffer))
        {
            char character = buffer_get(receive_buffer);
            if ((character == 0x0D) || (characterPosition > 78))
            {
                line[characterPosition] = 0;
                characterPosition = 0;
                parseCommand(line);
            }
            else line[characterPosition++] = character;
        }
        usart_print_string(line);
    	usart_enable_tx_interrupt(USART1);

/* MEASUREMENT OF QUANTITIES */
/* Battery current is critical and is measured each cycle. The remaining 3 are
measured every fourth cycle, with computations done in the remaining slot.*/

/* Start A/D for battery current feedback */

	    uint8_t channel[1];
        channel[0] = ADC_CHANNEL0;
        adc_set_regular_sequence(ADC1, 1, channel);
        adc_start_conversion_direct(ADC1);
        while (! adc_eoc(ADC1));
        ibatt = adc_read_regular(ADC1);

/* Start A/D for load current feedback */

        channel[0] = ADC_CHANNEL1;
        adc_set_regular_sequence(ADC1, 1, channel);
        adc_start_conversion_direct(ADC1);
        while (! adc_eoc(ADC1));
        iload = adc_read_regular(ADC1);

/* Start A/D for input voltage feedforward */

        channel[0] = ADC_CHANNEL2;
        adc_set_regular_sequence(ADC1, 1, channel);
        adc_start_conversion_direct(ADC1);
        while (! adc_eoc(ADC1));
        ibatt = adc_read_regular(ADC1);

/* Start A/D for output voltage feedback */

        channel[0] = ADC_CHANNEL3;
        adc_set_regular_sequence(ADC1, 1, channel);
        adc_start_conversion_direct(ADC1);
        while (! adc_eoc(ADC1));
        ibatt = adc_read_regular(ADC1);

/* SMOOTHING OF MEASURED QUANTITIES */
/* Smoothing is done with an exponentially weighted moving average filter. This
is an IIR filter which doesn't require memory of past samples.
The filter coefficient is a power of 8 to facilitate multiply and divide
operations.

nth average estimator avn, nth sample xn, filter coefficient c = 2^k, all 8 bit.
The quantity cavn = c*avn is maintained at 16 bits to avoid roundoff errors.

cavn += (xn - cavn/c)

*/
        ibattAvg = (cibattAvg >> K);
        cibattAvg += ibatt - ibattAvg;
        vbattAvg = (cvbattAvg >> K);
        cvbattAvg += vbatt - vbattAvg;
        iloadAvg = (ciloadAvg >> K);
        ciloadAvg += iload - iloadAvg;
        vinAvg = (cvinAvg >> K);
        cvinAvg += vin - vinAvg;

/* Compute duty cycles */

        buck = dutyCycle;
        boost = 0;

/* PWM with both outputs */
        if (state == full)
        {
//            timer_enable_oc_output(TIM1, TIM_OC1);
  //          timer_disable_oc_output(TIM1, TIM_OC1N);
        }

/**** Adjust the computed duty cycle */
	    timer_set_oc_value(TIM1, TIM_OC1, buck);
	    timer_set_oc_value(TIM1, TIM_OC2, boost);

        gpio_toggle(GPIOB, GPIO8);     /* reference signal on spare port */
    }

}

/*--------------------------------------------------------------------------*/
/* Parse a USART command line and take actions */

static void parseCommand(char* line)
{
/* Change the current limit */
    if (line[0] == 'a')
    {
        switch (line[1])
        {
        case 'I':
            {
                uint16_t currentLimit = (uint16_t)asciiToInt((char*)line+2);
                dac_load_data_buffer_single(currentLimit,RIGHT12,CHANNEL_1);
                break;
            }
/* Change the PWM period */
        case 'P':
            {
                period = (uint16_t)asciiToInt((char*)line+2);
            	timer_set_period(TIM1, period);
            	timer_enable_counter(TIM1);
                break;
            }
/* Change the PWM duty cycle */
        case 'D':
            {
                dutyCycle = (uint16_t)asciiToInt((char*)line+2);
                break;
            }
        }
    }
}
/*--------------------------------------------------------------------------*/

static void clockSetup(void)
{
/* Set the clock to 72MHz from the 8MHz external crystal */
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
}

/*--------------------------------------------------------------------------*/

static void watchdogSetup(void)
{
/* Start the Watchdog Timer for 15ms */
    iwdg_set_period_ms(15);
	iwdg_start();
}

/*--------------------------------------------------------------------------*/
/* Setup ADC

Set the ADC to convert four ports in single conversion mode.
*/

static void adcSetup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
/* Set ADC port pins PA0-3 as analogue inputs */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
		      GPIO_CNF_INPUT_ANALOG, GPIO0 | GPIO1 | GPIO2 | GPIO3);
/* Setup A/D Converter */

    rcc_periph_clock_enable(RCC_ADC1);
    adc_power_on(ADC1);
    adc_calibration(ADC1);
}

/*--------------------------------------------------------------------------*/
/* Setup DAC

Set the DAC1 on PA4.
*/

static void dacSetup(void)
{
/* Enable the DAC clock on APB1 */
	rcc_periph_clock_enable(RCC_DAC);
/* Set port PA4 for DAC1 output to 'alternate function'. Output driver mode is irrelevant. */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO4);
	dac_enable(CHANNEL_D);
}

/*--------------------------------------------------------------------------*/
/* Setup the Timer

The timer 1 is configured to PWM on four ports with deadtime.
*/

static void timerSetup(void)
{
/* Enable GPIOA and GPIOB clocks. */
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_AFIO);
/* Setup the PWM ports and output compares to default values */
/* Set ports PA8, PA9, PB13, PB14 for PWM, to 'alternate function output
push-pull', with PB8 as a reference signal. */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO8 | GPIO9);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO13 | GPIO14);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO8);

/* Enable TIM1 clock. */
	rcc_periph_clock_enable(RCC_TIM1);

/* Reset TIM1 peripheral. */
	timer_reset(TIM1);

/* Set Timer global mode:
 * - No division
 * - Alignment centre mode 1 (up/down counting, interrupt on downcount only)
 * - Direction up (when centre mode is set it is read only, changes by hardware)
 */
	timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_CENTER_1, TIM_CR1_DIR_UP);

/* Set Timer output compare mode:
 * - Channel 1
 * - PWM mode 2 (output low when CNT < CCR1, high otherwise)
 * OC1 is buck, OC2 is boost
 */
	timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_PWM2);
	timer_enable_oc_output(TIM1, TIM_OC1);
	timer_enable_oc_output(TIM1, TIM_OC1N);
	timer_set_oc_mode(TIM1, TIM_OC2, TIM_OCM_PWM2);
	timer_enable_oc_output(TIM1, TIM_OC2);
	timer_enable_oc_output(TIM1, TIM_OC2N);
	timer_enable_break_main_output(TIM1);

/* The ARR (auto-preload register) sets the PWM period to 62.5kHz from the
72 MHz clock.*/
	timer_enable_preload(TIM1);
	timer_set_period(TIM1, PWM_PERIOD);

/* The CCR1 (capture/compare register 1) sets PWM duty cycle to default 50% */
	timer_enable_oc_preload(TIM1, TIM_OC1);
	timer_set_oc_value(TIM1, TIM_OC1, PWM_PERIOD/2);
	timer_enable_oc_preload(TIM1, TIM_OC2);
	timer_set_oc_value(TIM1, TIM_OC2, 0);
    timer_set_deadtime(TIM1, DEADTIME);

/* Force an update to load the shadow registers */
	timer_generate_event(TIM1, TIM_EGR_UG);

/* Start the Counter. */
	timer_enable_counter(TIM1);
}

/*--------------------------------------------------------------------------*/
/** @brief USART Setup

USART 1 is configured for 115200 baud, no flow control, and interrupt.
*/

static void usartSetup(void)
{
/* Enable clocks for GPIO port A (for GPIO_USART1_TX) and USART1. */
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_AFIO);
    rcc_periph_clock_enable(RCC_USART1);
/* Enable the USART1 interrupt. */
	nvic_enable_irq(NVIC_USART1_IRQ);
/* Setup GPIO pin GPIO_USART1_RE_TX on GPIO port A for transmit. */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
/* Setup GPIO pin GPIO_USART1_RE_RX on GPIO port A for receive. */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
		      GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);
/* Setup UART parameters. */
	usart_set_baudrate(USART1, BAUDRATE);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART1, USART_MODE_TX_RX);
/* Enable USART1 receive interrupts. */
	usart_enable_rx_interrupt(USART1);
	usart_disable_tx_interrupt(USART1);
/* Finally enable the USART. */
	usart_enable(USART1);
}

/*--------------------------------------------------------------------------*/

void print_register(uint32_t reg)
{
	usart_print_hex((reg >> 16) & 0xFFFF);
	usart_print_hex((reg >> 00) & 0xFFFF);
	buffer_put(send_buffer, ' ');
}

/*--------------------------------------------------------------------------*/

/* Print out a value in ASCII decimal form (ack Thomas Otto). */
void usart_print_int(int value)
{
	uint8_t i;
	uint8_t nr_digits = 0;
	char buffer[25];

	if (value < 0)
	{
		buffer_put(send_buffer, '-');
		value = value * -1;
	}
	if (value == 0) buffer[nr_digits++] = '0';
	else while (value > 0)
	{
		buffer[nr_digits++] = "0123456789"[value % 10];
		value /= 10;
	}
	for (i = nr_digits; i > 0; i--)
	{
		buffer_put(send_buffer, buffer[i-1]);
	}
	buffer_put(send_buffer, ' ');
}

/*--------------------------------------------------------------------------*/

/* Print out a value in ASCII hex form. */
void usart_print_hex(uint16_t value)
{
	uint8_t i;
	char buffer[25];

	for (i = 0; i < 4; i++)
	{
		buffer[i] = "0123456789ABCDEF"[value & 0xF];
		value >>= 4;
	}
	for (i = 4; i > 0; i--)
	{
		buffer_put(send_buffer, buffer[i-1]);
	}
	buffer_put(send_buffer, ' ');
}

/*--------------------------------------------------------------------------*/

/* Print a String. */
void usart_print_string(char *ch)
{
  	while(*ch)
	{
     	buffer_put(send_buffer, *ch);
     	ch++;
  	}
}

/*--------------------------------------------------------------------------*/
/** @brief Convert an ASCII decimal string to an integer

The conversion stops without error when the first non-numerical character
is encountered.

@param[in] char* buffer: externally defined buffer with the string.
@returns int32_t: integer value to be converted to ASCII form.
*/

int32_t asciiToInt(char* buffer)
{
    int32_t number = 0;
    while ((*buffer >= '0') && (*buffer <= '9'))
    {
        number = number*10+(*buffer - '0');
        buffer++;
    }
    return number;
}
/*--------------------------------------------------------------------------*/
/** @brief Convert an Integer to ASCII decimal form

@param[in] int32_t value: integer value to be converted to ASCII form.
@param[in] char* buffer: externally defined buffer to hold the result.
*/

void intToAscii(int32_t value, char* buffer)
{
	uint8_t nr_digits = 0;
    uint8_t i = 0;
    char temp_buffer[25];

/* Add minus sign if negative, and form absolute */
	if (value < 0)
	{
		buffer[nr_digits++] = '-';
		value = value * -1;
	}
/* Stop if value is zero */
	if (value == 0) buffer[nr_digits++] = '0';
	else
    {
/* Build string in reverse */
        while (value > 0)
	    {
		    temp_buffer[i++] = "0123456789"[value % 10];
		    value /= 10;
	    }
/* Copy across correcting the order */
        while (i > 0)
        {
            buffer[nr_digits++] = temp_buffer[--i];
        }
    }
    buffer[nr_digits] = 0;
}
/*--------------------------------------------------------------------------*/

/* Find out what interrupted and get or send data as appropriate */
void usart1_isr(void)
{
	static uint16_t data;

	/* Check if we were called because of RXNE. */
	if (usart_get_flag(USART1,USART_SR_RXNE))
	{
		/* If buffer full we'll just drop it */
		buffer_put(receive_buffer, (uint8_t) usart_recv(USART1));
	}
	/* Check if we were called because of TXE. */
	if (usart_get_flag(USART1,USART_SR_TXE))
	{
		/* If buffer empty, disable the tx interrupt */
		data = buffer_get(send_buffer);
		if ((data & 0xFF00) > 0) usart_disable_tx_interrupt(USART1);
		else usart_send(USART1, (data & 0xFF));
	}
}

