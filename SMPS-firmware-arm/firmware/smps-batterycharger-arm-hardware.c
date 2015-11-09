/* STM32F1 SMPS Battery Charger

Hardware Setup

This configures all peripherals for the program and provides some
convenience functions for accessing the hardware.

Along with the file board-defs.h that provides specific defines for the
processor board in use, this file abstracts the hardware from the rest
of the program and allows rapid adaption to other board archictectures.

Note that the STM32F series of ARM microcontrollers are used because of
their provision of 12 bit A/D.

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

/* Set this define if the SWD debug is to be used. It will disable some ports
so must be commented out for normal use. */
#define USE_SWD

#include <stdint.h>
#include <stdbool.h>

#include "smps-batterycharger-arm-board-defs.h"
#include "smps-batterycharger-arm-hardware.h"

/* libopencm3 driver includes */
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/* Local Prototypes */
static void adcSetup(void);
static void dmaAdcSetup(void);
static void dacSetup(void);
static void gpioSetup(void);
static void usartSetup(void);
static void clockSetup(void);
static void systickSetup();
static void pwmSetup(void);

/* Local Variables */
static uint8_t pwmCount;
static uint32_t v[NUM_CHANNEL]; /* Buffer used by DMA to dump A/D conversions */
static uint8_t adceoc;          /* A/D end of conversion flag */

/* FreeRTOS queues and intercommunication variables defined in Comms */
extern xQueueHandle commsSendQueue, commsReceiveQueue, commsEmptySemaphore;

/*--------------------------------------------------------------------------*/
/** @brief Initialise the hardware

*/

void prvSetupHardware(void)
{
	clockSetup();
	gpioSetup();
	usartSetup();
    pwmSetup();
	dmaAdcSetup();
	adcSetup();
    dacSetup();
    systickSetup();
}

/*--------------------------------------------------------------------------*/
/** @brief Return the A/D Conversion Results

Note the channel must be less than the number of channels in the A/D converter.

@param[in] uint8_t channel: A/D channel to be retrieved from the DMA buffer.
@returns uint32_t : last value measured by the A/D converter.
*/

uint32_t adcValue(uint8_t channel)
{
    if (channel > NUM_CHANNEL) return 0;
    return v[channel];
}

/*--------------------------------------------------------------------------*/
/** @brief Return and Reset the A/D End of Conversion Flag

This must be retrieved from the ISR as the hardware EOC doesn't always change
at the end of a conversion (when multiple conversions take place).

@returns uint8_t: boolean true if the flag was set; false otherwise.
*/

uint8_t adcEOC(void)
{
    if (adceoc > 0)
    {
        adceoc = 0;
        return 1;
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
/** @brief PWM Timer set Duty Cycle

Both the buck and boost channel duty cycles are set. The OC value represent the
location in the cycle that the output turns on rather than finishing, so the
duty cycle value must be reversed. In that way, OC shows the duty cycle and
OCN is its inverse (apart from programmed deadtime).

Deadtime is set for both channels.

@parameter[in] uint8_t dutyCycleBuck: Duty cycle in percentage for buck section.
@parameter[in] uint8_t dutyCycleBoost: Duty cycle in percentage for boost section.
*/

void pwmSetDutyCycle(uint8_t dutyCycleBuck,uint8_t dutyCycleBoost)
{
	timer_enable_oc_preload(TIM8, TIM_OC1);
	timer_set_oc_value(TIM8, TIM_OC1,
            (uint32_t)((PWM_PERIOD*(uint16_t)(100-dutyCycleBuck))/100));    
	timer_generate_event(TIM8, TIM_EGR_UG);
	timer_enable_oc_preload(TIM8, TIM_OC2);
	timer_set_oc_value(TIM8, TIM_OC2,
            (uint32_t)((PWM_PERIOD*(uint16_t)(100-dutyCycleBoost))/100));
    timer_set_deadtime(TIM8, DEADTIME);
}

/*--------------------------------------------------------------------------*/
/** @brief Set the Current limit

Set the current limit by setting the DAC level.

@parameter[in] uint16_t limit: Current limit 0..4096.
*/

void setCurrentLimit(uint16_t limit)
{
    dac_load_data_buffer_single(limit,RIGHT12,CHANNEL_1);
}

/*--------------------------------------------------------------------------*/
/** @brief Enable/Disable USART Interrupt

@parameter[in] uint8_t enable: true to enable the interrupt, false to disable.
*/

void commsEnableTxInterrupt(uint8_t enable)
{
    if (enable) usart_enable_tx_interrupt(USART1);
    else usart_disable_tx_interrupt(USART1);
}

/*--------------------------------------------------------------------------*/

/* Hardware Setup */

/*--------------------------------------------------------------------------*/
/** @brief Clock Setup

The processor system clock is established and the necessary peripheral
clocks are turned on.
*/

static void clockSetup(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
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
/** @brief PWM Timer Setup

Setup GPIO Port A8 for TIM8_CH1 PWM output to generate a signal of a given  duty cycle.
No interrupt is required.
*/

static void pwmSetup(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_AFIO);
    rcc_periph_clock_enable(RCC_TIM8);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO6 | GPIO7);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO6 | GPIO7);

/* Reset TIM8 peripheral. */
	timer_reset(TIM8);

/* Set Timer global mode:
 - No division
 - Alignment centre mode 1 (up/down counting, interrupt on downcount only)
 - Direction up (when centre mode is set it is read only, changes by hardware)
*/
	timer_set_mode(TIM8, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_CENTER_1, TIM_CR1_DIR_UP);

/* Set Timer output compare mode:
 - Channel 1 and 2, PWM mode 2 (output low when CNT < CCR1, high otherwise)
 - OC1 is buck, OC2 is boost
 - set normal and invert outputs for synchronous SMPS.
*/
	timer_set_oc_mode(TIM8, TIM_OC1, TIM_OCM_PWM2);
	timer_enable_oc_output(TIM8, TIM_OC1);
	timer_enable_oc_output(TIM8, TIM_OC1N);
	timer_set_oc_mode(TIM8, TIM_OC2, TIM_OCM_PWM2);
	timer_enable_oc_output(TIM8, TIM_OC2);
	timer_enable_oc_output(TIM8, TIM_OC2N);
	timer_enable_break_main_output(TIM8);

/* The ARR (auto-preload register) sets the PWM period to 50 microseconds from the
72 MHz clock.*/
	timer_enable_preload(TIM8);
	timer_set_period(TIM8, PWM_PERIOD);

/* The CCR1 (capture/compare register 1) sets the PWM duty cycle to default 50% */
    pwmSetDutyCycle(50,0);

/* Force an update to load the shadow registers */
	timer_generate_event(TIM8, TIM_EGR_UG);

/* Start the Counter. */
	timer_enable_counter(TIM8);
}

/*--------------------------------------------------------------------------*/
/** @brief GPIO Setup

Setup GPIO Ports A, B, C for all peripherals.
*/

static void gpioSetup(void)
{
/* Enable all GPIO clocks. */
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_AFIO);

#ifndef USE_SWD
/* Disable SWD and JTAG to allow full use of the ports PA13, PA14, PA15 */
    gpio_primary_remap(AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_OFF,0);
#endif

/* PA inputs analogue for currents, voltages and ambient temperature */
#ifdef PA_ANALOGUE_INPUTS
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG,
			    PA_ANALOGUE_INPUTS);
#endif
/* PA inputs analogue for currents, voltages and ambient temperature */
#ifdef PC_ANALOGUE_INPUTS
    gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG,
			    PC_ANALOGUE_INPUTS);
#endif
/* PA outputs digital */
#ifdef PA_DIGITAL_OUTPUTS
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
			    PA_DIGITAL_OUTPUTS);
    gpio_clear(GPIOA, PA_DIGITAL_OUTPUTS);
#endif
/* PB outputs digital */
#ifdef PB_DIGITAL_OUTPUTS
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
			    PB_DIGITAL_OUTPUTS);
    gpio_clear(GPIOB, PB_DIGITAL_OUTPUTS);
#endif
/* PC outputs digital */
#ifdef PC_DIGITAL_OUTPUTS
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
			    PC_DIGITAL_OUTPUTS);
    gpio_clear(GPIOC, PC_DIGITAL_OUTPUTS);
#endif
/* PA inputs digital */
#ifdef PA_DIGITAL_INPUTS
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
			    PA_DIGITAL_INPUTS);
#endif
/* PB inputs digital */
#ifdef PB_DIGITAL_INPUTS
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
			    PB_DIGITAL_INPUTS);
#endif
/* PC inputs digital */
#ifdef PC_DIGITAL_INPUTS
    gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
			    PC_DIGITAL_INPUTS);
#endif
}

/*--------------------------------------------------------------------------*/
/** @brief DMA Setup

Enable DMA 1 Channel 1 to take conversion data from ADC 1, and also ADC 2 when
the ADC is used in dual mode. The ADC will dump a burst of data to memory each
time, and we need to grab it before the next conversions start. This must be
called after each transfer to reset the memory buffer to the beginning.
*/

static void dmaAdcSetup(void)
{
	/* Enable DMA1 Clock */
    rcc_periph_clock_enable(RCC_DMA1);
	dma_channel_reset(DMA1,DMA_CHANNEL1);
	dma_set_priority(DMA1,DMA_CHANNEL1,DMA_CCR_PL_LOW);
/* We want all 32 bits from the ADC to include ADC2 data */
	dma_set_memory_size(DMA1,DMA_CHANNEL1,DMA_CCR_MSIZE_32BIT);
	dma_set_peripheral_size(DMA1,DMA_CHANNEL1,DMA_CCR_PSIZE_32BIT);
	dma_enable_memory_increment_mode(DMA1,DMA_CHANNEL1);
	dma_set_read_from_peripheral(DMA1,DMA_CHANNEL1);
/* The register to target is the ADC1 regular data register */
	dma_set_peripheral_address(DMA1,DMA_CHANNEL1,(uint32_t) &ADC1_DR);
/* The array v[] receives the converted output */
	dma_set_memory_address(DMA1,DMA_CHANNEL1,(uint32_t) v);
	dma_set_number_of_data(DMA1,DMA_CHANNEL1,NUM_CHANNEL);
	dma_enable_channel(DMA1,DMA_CHANNEL1);
}

/*--------------------------------------------------------------------------*/
/** @brief ADC Setup

ADC1 is setup for scan mode. Single conversion does all selected
channels once through then stops. DMA enabled to collect data.
*/

static void adcSetup(void)
{
	/* Enable clocks for ADCs */
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_AFIO);
    rcc_periph_clock_enable(RCC_ADC1);
/* ADC clock should be maximum 14MHz, so divide by 8 from 72MHz. */
    rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV8);
	nvic_enable_irq(NVIC_ADC1_2_IRQ);
	/* Make sure the ADC doesn't run during config. */
	adc_off(ADC1);
	/* Configure ADC1 for multiple conversion. */
	adc_enable_scan_mode(ADC1);
	adc_set_single_conversion_mode(ADC1);
	adc_enable_external_trigger_regular(ADC1, ADC_CR2_EXTSEL_SWSTART);
	adc_set_right_aligned(ADC1);
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_28DOT5CYC);
	adc_enable_dma(ADC1);
	adc_enable_eoc_interrupt(ADC1);
/* Power on and calibrate */
	adc_power_on(ADC1);
	/* Wait for ADC starting up. */
	uint32_t i;
	for (i = 0; i < 800000; i++)    /* Wait a bit. */
		__asm__("nop");
	adc_reset_calibration(ADC1);
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
/** @brief Systick Setup

Setup SysTick Timer for 1 millisecond interrupts, also enables Systick and
Systick-Interrupt
*/

static void systickSetup()
{
	/* 72MHz / 8 => 9,000,000 counts per second */
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);

	/* 9000000/9000 = 1000 overflows per second - every 1ms one interrupt */
	/* SysTick interrupt every N clock pulses: set reload to N-1 */
	systick_set_reload(8999);

	systick_interrupt_enable();

	/* Start counting. */
	systick_counter_enable();
}

/*--------------------------------------------------------------------------*/
                /* ISRs */
/*--------------------------------------------------------------------------*/
/** @brief USART Interrupt

Find out what interrupted and get or send data as appropriate.
*/

void usart1_isr(void)
{
	char data;
    portBASE_TYPE ok;

/* Check if we were called because of RXNE. */
	if (usart_get_flag(USART1,USART_SR_RXNE))
	{
/* Pull in received character. If buffer full we'll just drop it */
        char inCharacter = (char) usart_recv(USART1);
        xQueueSendToBackFromISR(commsReceiveQueue,&inCharacter,NULL);
	}
/* Check if we were called because of TXE. */
	if (usart_get_flag(USART1,USART_SR_TXE))
	{
/* If the queue is empty, disable the tx interrupt until something is sent.
Also release the commsEmptySemaphore to signal that low priority messages
may be sent. */
		ok = xQueueReceiveFromISR(commsSendQueue,&data,NULL);
		if (ok) usart_send(USART1, data);
		else
        {
            int wokenTask;
            usart_disable_tx_interrupt(USART1);
            xSemaphoreGiveFromISR(commsEmptySemaphore,&wokenTask);    /* Flag as empty */
//            if (wokenTask) portYIELD_FROM_ISR(wokenTask);
        }
	}
}

/*--------------------------------------------------------------------------*/
/** @brief ADC ISR

Respond to ADC EOC at end of scan.

The EOC status is lost when DMA reads the data register, so use a global
variable.
*/

void adc1_2_isr(void)
{
    adceoc = 1;
/* Clear DMA to restart at beginning of data array */
    dmaAdcSetup();
}

