SMPS Firmware for ARM
---------------------

This is STM32F102 firmware used during the development of an SMPS full-H
buck-boost system for experimental purposes. The development platform is the
ET-STM32F103 board from Futurlec.

The initial code is for a simple asynchronous buck supply with PMOS high
switching. The code here contains tests for various aspects of the full system.

buck-pmos-PWM.c emits PWM suitable for synchronous buck operation, for waveform
   observation and testing.

buck-pmos-data-capture.c measures currents and voltages and sends back in a
   serial form. A primitive ASCII command-response interface is provided.

(c) K. Sarkies 05/12/2015

