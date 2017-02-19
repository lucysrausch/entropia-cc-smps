/* STM32F1 Generation of PWM and Capture of voltages and currents in SMPS

An ASCII based command-response interface is provided to control a SMPS
and to retrieve data captured during a run.

Data capture is via the ADC input channels 4 to 7 on PA4-7.
PWM is from timer 1: CH2 on PA9, CH3 on PA10, CH2N on PB14, CH3N on PB15.
CH1(N) is the buck synchronous PWM and CH2(N) is the boost synchronous PWM.
USART2 is the serial communications peripheral (USART1 clashes with timer 1
and the alternative advanced timer 8 doesn't exist).

- 'aE' Send back identifier string
- 'ac+' 'ac-' turn on/off data capture.
- set parameters for capture frequency, PWM duty cycle
- turn on/off power
- retrieve next data set

5 December 2015
*/

/*
 * This file is part of the SMPS project.
 *
 * Copyright K. Sarkies <ksarkies@internode.on.net>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include "buffer.h"
#include "stringlib.h"
#include "commslib.h"
#include "buck-pmos-data-capture.h"

/*--------------------------------------------------------------------------*/
/* Global Variables */
uint32_t v[NUM_CHANNEL];    /* Captured data array, one scan */
uint32_t data[NUM_CHANNEL]; /* Captured data array for the run */
uint8_t adceoc;             /* A/D end of conversion flag */
/* Settable Parameters */
uint16_t dataBlockSize;
uint8_t capture;         /* Activate and stop data capture */
uint16_t frequency;         /* PWM frequency in kHz */
int16_t ch1DutyCycle;   /* Duty cycle % for buck converter */
int16_t ch2DutyCycle; /* Duty cycle % for boost converter */
int32_t modifier = 0, isValue = 0, setValue = 0;

/*--------------------------------------------------------------------------*/

int main(void) {
  uint8_t i = 0;     /* Channel counter */
  uint8_t index = 0; /* index into storage array */

  uint8_t channelArray[NUM_CHANNEL];
  uint8_t characterPosition = 0;
  uint8_t line[LINE_SIZE];
  capture = false;
  dataBlockSize = DATA_BLOCK_SIZE;
  uint16_t comDelay = 0;
  uint16_t regDelay = 0;

  /* Initialize peripherals */
  clockSetup();
  gpioSetup();
  usartSetup();
  dmaAdcSetup();
  adcSetup();
  timer2Setup(0x8FFF);
  timer1SetupPWM();
  commsInit();

  /* Set initial PWM to safe values. */
  frequency = FREQUENCY;
  ch1DutyCycle = 0;
  ch2DutyCycle = 0;
  timer1PWMsettings(frequency, ch1DutyCycle, ch2DutyCycle);

  /* Setup array of selected channels for conversion and clear the data array
  for
  the first pass */
  for (i = 0; i < NUM_CHANNEL; i++) {
    channelArray[i] = i + 4;
    v[i] = 0;
  }
  adc_set_regular_sequence(ADC1, NUM_CHANNEL, channelArray);
  commsPrintString("\nAll meow!\n");
  gpio_clear(GPIOC, GPIO13); //debug LED
  while (1) {

    /* Build a command line string before actioning. */
    uint16_t character = commsNextCharacter();
    if (character < 0x100) /* returns 0x100 if no character received */
    {
      if ((character == 0x0D) || (character == 0x0A) ||
          (characterPosition > LINE_SIZE - 2)) {
        line[characterPosition] = 0;
        characterPosition = 0;
        parseCommand(line);
      } else
        line[characterPosition++] = character;
    }

    /* Activate the ADC conversions after the preset time in timer 2 has
    elapsed.
    Without timer 2 prescaler this has a maximum of 2 ms period. */
    if (timer_get_flag(TIM2, TIM_SR_CC1IF) && capture) {
      timer_clear_flag(TIM2, TIM_SR_CC1IF);

      if (regDelay++ > 10) {//every 10ms
        isValue = v[1];
        modifier = setValue - isValue;
        modifier = (int)(modifier / 50);

        if (modifier == 0 && setValue < isValue - 10) {
          modifier = -1;
        }

        if (modifier == 0 && setValue > isValue + 10) {
          modifier = 1;
        }

        ch1DutyCycle += modifier;

        if (ch1DutyCycle > 1000) {
          ch1DutyCycle = 1000;
        }

        if (ch1DutyCycle < 0) {
          ch1DutyCycle = 0;
        }

        timer1PWMsettings(frequency, ch1DutyCycle, ch2DutyCycle);
        regDelay = 0;
      }
      /* Delay a bit more to slow down comms to once per second */
      if (comDelay++ > 200) {
        /* Store previous conversion results, which should be well in by now. */
        sendResponse("Channel 1: ", v[1]);
        sendResponse("Channel 2: ", v[0]);

        sendResponse("isValue: ", isValue);
        sendResponse("setValue: ", setValue);

        sendResponse("modifier: ", modifier);
        sendResponse("PWM: ", ch1DutyCycle);

        comDelay = 0;
      }
      /* Reset timer and initiate next data capture */
      adc_start_conversion_regular(ADC1);
    }
  }

  return 0;
}

/*--------------------------------------------------------------------------*/
/** @brief Parse a command line and act on it.

Commands begin with a lower case letter a, d, p or f followed by an upper case
command letter. The line is terminated by a 0. If the line does not form a
recognizable command it is ignored. Any characters following a recognized
command are also ignored.

Unrecognizable messages should just be discarded.

@param[in] char *line: the command line in ASCII
*/

void parseCommand(uint8_t *line) {
  /* Action commands */
  if (line[0] == 'a') {
    switch (line[1]) {
    /* Start capture 'ac+' stop capture 'ac-' */
    case 'c': {
      capture = (line[2] == '+');
      break;
    }
    /* Send ident response */
    case 'i': {

      // char ident[35] = "SMPS, Firmware: ";
      // strcat(ident, FIRMWARE_VERSION);
      // sendString("dE",ident);
      commsPrintString("\nEntropia e.V. LED control\n");
      // commsPrintString(ident);
      // gpio_set(GPIOC, GPIO13);
      break;
    }
    }
  }
  /* Parameter setting commands */
  else if (line[0] == 'p') {
    /* Set the timer 2 count to set data sampling intervals, only if non zero.*/
    switch (line[1]) {
    case 'f': {
      uint16_t tempFrequency = asciiToInt((char *)line + 2);
      if (tempFrequency > 0 && tempFrequency < 1000) {
        frequency = tempFrequency;
        timer1PWMsettings(frequency, ch1DutyCycle, ch2DutyCycle);
      }
      sendResponse("Changing PWM frequncy interval to (kHz): ", frequency);
      break;
    }
    }
    /* Set the timer 1 PWM .*/
    switch (line[1]) {
    case 'p': {
      uint16_t ch1DutyCycle = asciiToInt((char *)line + 2);
      if (ch1DutyCycle <= 1000)
        timer1PWMsettings(frequency, ch1DutyCycle, ch2DutyCycle);
      sendResponse("Changeing Channel 1 PWM duty cycle to: ", ch1DutyCycle);
      break;
    }
    }

    /* Change setPoint .*/
    switch (line[1]) {
    case 's': {
      uint16_t tempSetValue = asciiToInt((char *)line + 2);
      if (tempSetValue <= 4095 && tempSetValue >= 0) {
        setValue = tempSetValue;
        sendResponse("Changeing setpoint of Channel 1 to: ", setValue);
      }

      break;
    }
    }
  }
  /* Send a single line of results */
  else if (line[0] == 'd') {
  }
}

/*--------------------------------------------------------------------------*/
/** @brief Clock Setup

The processor system clock is established and the necessary peripheral
clocks are turned on */

void clockSetup(void) { rcc_clock_setup_in_hse_8mhz_out_72mhz(); }

/*--------------------------------------------------------------------------*/
/** @brief GPIO Setup
*/

void gpioSetup(void) {
  /* Enable all GPIO clocks. */
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_GPIOC);
  rcc_periph_clock_enable(RCC_AFIO);

  /* Disable SWD and JTAG to allow full use of the ports PA13, PA14, PA15 */
  gpio_primary_remap(AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_OFF, 0);

  /* Setup GPIO pin GPIO_USART2_RE_TX on GPIO port A for transmit. */
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                GPIO_USART2_TX); // PA2
  /* Setup GPIO pin GPIO_USART2_RE_RX on GPIO port A for receive. */
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
                GPIO_USART2_RX); // PA3

  /* Set ports PA9 (TIM1_CH2), PA10 (TIM1_CH3), PB14 (TIM1_CH2N), PB15
  (TIM1_CH3N)
  for PWM, to 'alternate function output push-pull'. */
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                GPIO9 | GPIO10);
  //gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                //GPIO14 | GPIO15);

  /* PA inputs 4-7 as analogue for currents, voltages and ambient temperature */
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG,
                GPIO4 | GPIO5 | GPIO6 | GPIO7);

  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                GPIO13);

  gpio_set(GPIOC, GPIO13);
}

/*--------------------------------------------------------------------------*/
/** @brief USART Setup

USART 2 is configured for 38400 baud, no flow control, and interrupt.
*/

void usartSetup(void) {
  /* Enable clocks for GPIO port A (for GPIO_USART2_TX) and USART2. */
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_AFIO);
  rcc_periph_clock_enable(RCC_USART2);
  /* Enable the USART2 interrupt. */
  nvic_enable_irq(NVIC_USART2_IRQ);
  /* Setup USART parameters. */
  usart_set_baudrate(USART2, BAUDRATE);
  usart_set_databits(USART2, 8);
  usart_set_stopbits(USART2, USART_STOPBITS_1);
  usart_set_parity(USART2, USART_PARITY_NONE);
  usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
  usart_set_mode(USART2, USART_MODE_TX_RX);
  /* Enable USART2 receive interrupts. */
  usart_enable_rx_interrupt(USART2);
  /* Disable USART2 transmit interrupts. */
  usart_disable_tx_interrupt(USART2);
  /* Finally enable the USART. */
  usart_enable(USART2);
}

/*--------------------------------------------------------------------------*/
/** @brief DMA Setup

Enable DMA 1 Channel 1 to take conversion data from ADC 1, and also ADC 2 when
the ADC is used in dual mode. The ADC will dump a burst of data to memory each
time, and we need to grab it before the next conversions start. This must be
called after each transfer to reset the memory buffer to the beginning.
*/

void dmaAdcSetup(void) {
  /* Enable DMA1 Clock */
  rcc_periph_clock_enable(RCC_DMA1);
  dma_channel_reset(DMA1, DMA_CHANNEL1);
  dma_set_priority(DMA1, DMA_CHANNEL1, DMA_CCR_PL_LOW);
  /* We want all 32 bits from the ADC to include ADC2 data */
  dma_set_memory_size(DMA1, DMA_CHANNEL1, DMA_CCR_MSIZE_32BIT);
  dma_set_peripheral_size(DMA1, DMA_CHANNEL1, DMA_CCR_PSIZE_32BIT);
  dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL1);
  dma_set_read_from_peripheral(DMA1, DMA_CHANNEL1);
  /* The register to target is the ADC1 regular data register */
  dma_set_peripheral_address(DMA1, DMA_CHANNEL1, (uint32_t)&ADC_DR(ADC1));
  /* The array v[] receives the converted output */
  dma_set_memory_address(DMA1, DMA_CHANNEL1, (uint32_t)v);
  dma_set_number_of_data(DMA1, DMA_CHANNEL1, NUM_CHANNEL);
  dma_enable_channel(DMA1, DMA_CHANNEL1);
}

/*--------------------------------------------------------------------------*/
/** @brief ADC Setup

ADC1 is setup for scan mode. Single conversion does all selected
channels once through then stops. DMA enabled to collect data.
*/

void adcSetup(void) {
  /* Enable clocks for ADCs */
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_AFIO);
  rcc_periph_clock_enable(RCC_ADC1);
  /* ADC clock should be maximum 14MHz, so divide by 8 from 72MHz. */
  rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV8);
  nvic_enable_irq(NVIC_ADC1_2_IRQ);
  /* Make sure the ADC doesn't run during config. */
  adc_power_off(ADC1);
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
  for (i = 0; i < 800000; i++) /* Wait a bit. */
    __asm__("nop");
  adc_reset_calibration(ADC1);
  adc_calibration(ADC1);
}

/*--------------------------------------------------------------------------*/
/** @brief Timer 1 Setup

Setup timer 1 for PWM on outputs 2 and 3 and their inverse outputs, with
deadtime setting.

Note: Need to either select timer 8 (not available on all STM32F103, in
particular that in the ET-STM32F103 board) or timer 1 with USART1 pins mapped
to avoid pin clashes.
*/

void timer1SetupPWM(void) {
  /* Enable TIM1 clock. */
  rcc_periph_clock_enable(RCC_TIM1);

  /* Reset TIM1 peripheral. */
  timer_reset(TIM1);

  /* Set Timer global mode:
   * - No division
   * - Alignment centre mode 1 (up/down counting, interrupt on downcount only)
   * - Direction up (when centre mode is set it is read only, changes by
   * hardware)
   */
  timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_CENTER_1,
                 TIM_CR1_DIR_UP);

  /* Set Timer output compare mode:
   * - PWM mode 2 (output low when CNT < CCR1, high otherwise)
   * Channel 2 is the one to use for buck synchronous mode */
  timer_set_oc_mode(TIM1, TIM_OC2, TIM_OCM_PWM2);
  //timer_enable_oc_output(TIM1, TIM_OC2N);
  timer_enable_oc_output(TIM1, TIM_OC2);
  /* Channel 3 is the one used for boost synchronous mode */
  timer_set_oc_mode(TIM1, TIM_OC3, TIM_OCM_PWM2);
  //timer_enable_oc_output(TIM1, TIM_OC3N);
  timer_enable_oc_output(TIM1, TIM_OC3);
  timer_enable_break_main_output(TIM1);
  /* Set the polarity of OC1N to be low to match that of the OC1, for switching
  the low side MOSFET through an inverting level shifter */
  //timer_set_oc_polarity_low(TIM1, TIM_OC2N);
  /* Set the deadtime for OC1N. All deadtimes are set to this.
  Set to default as there is no need to change this on the fly. */
  //timer_set_deadtime(TIM1, DEADTIME);
}

/*--------------------------------------------------------------------------*/
/** @brief Timer 1 Set PWM Parameters

@param[in] uint16_t frequncy in kHz.
@param[in] uint16_t ch1DutyCycle: promille duty cycle
@param[in] uint16_t ch2DutyCycle.: promille duty cycle
*/

void timer1PWMsettings(uint16_t frequency, int16_t ch1DutyCycle,
                       int16_t ch2DutyCycle) {
  /* The ARR (auto-preload register) sets the PWM period to frequency (in kHz) from the
  72 MHz clock.*/
  timer_enable_preload(TIM1);
  uint16_t period = 72000 / frequency;
  timer_set_period(TIM1, period);

  ch1DutyCycle = 1000 - ch1DutyCycle;
  ch2DutyCycle = 1000 - ch2DutyCycle;

  /* The CCR1 (capture/compare register 1) sets PWM duty cycle to default 50% */
  timer_enable_oc_preload(TIM1, TIM_OC2);
  timer_set_oc_value(TIM1, TIM_OC2, (period * ch1DutyCycle) / 1000);
  timer_enable_oc_preload(TIM1, TIM_OC3);
  timer_set_oc_value(TIM1, TIM_OC3, (period * ch2DutyCycle) / 1000);

  /* Force an update to load the shadow registers */
  timer_generate_event(TIM1, TIM_EGR_UG);

  /* Start the Counter. */
  timer_enable_counter(TIM1);
}

/*--------------------------------------------------------------------------*/
/** @brief Timer 2 Setup

Setup timer 2 to run through a period, preset to the maximum 16 bit value,
and to set a compare flag when the count reaches a preset value based on the
output compare channel 1.

@param[in] uint16_t count.
*/

void timer2Setup(uint16_t count) {
  /* Enable TIM2 clock. */
  rcc_periph_clock_enable(RCC_TIM2);
  timer_reset(TIM2);
  /* Timer global mode - Divider 4, Alignment edge, Direction up */
  timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT_MUL_4, TIM_CR1_CMS_EDGE,
                 TIM_CR1_DIR_UP);
  timer_continuous_mode(TIM2);
  timer_set_period(TIM2, 0xFFFF);
  timer_enable_oc_output(TIM2, TIM_OC1);
  timer_disable_oc_clear(TIM2, TIM_OC1);
  timer_disable_oc_preload(TIM2, TIM_OC1);
  timer_set_oc_slow_mode(TIM2, TIM_OC1);
  timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_TOGGLE);
  timer_set_oc_value(TIM2, TIM_OC1, count);
  timer_disable_preload(TIM2);
  timer_enable_counter(TIM2);
}

/*--------------------------------------------------------------------------*/
/* ISRs */
/*--------------------------------------------------------------------------*/
/** @brief ADC ISR

Respond to ADC EOC at end of scan and send data block.
Print the result in decimal and separate with an ASCII dash.

The EOC status is lost when DMA reads the data register, so use a global
variable.
*/

void adc1_2_isr(void) {
  adceoc = 1;
  /* Clear DMA to restart at beginning of data array */
  dmaAdcSetup();
}

/*-----------------------------------------------------------*/
/*----       ISR Overrides in libopencm3     ----------------*/
/*-----------------------------------------------------------*/
/* These trap serious execution errors and reset the application by means
of the CM3 system reset command. */

/*-----------------------------------------------------------*/
void nmi_handler(void) { scb_reset_system(); }

/*-----------------------------------------------------------*/
void hard_fault_handler(void) { scb_reset_system(); }

/*-----------------------------------------------------------*/
void memory_manage_fault_handler(void) { scb_reset_system(); }

/*-----------------------------------------------------------*/
void bus_fault_handler(void) { scb_reset_system(); }

/*-----------------------------------------------------------*/
void usage_fault_handler(void) { scb_reset_system(); }
