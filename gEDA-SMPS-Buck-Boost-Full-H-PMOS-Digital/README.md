SMPS ARM driven Circuit
-----------------------

This project aims to provide a general PCB for building SMPS circuits for a
variety of configurations up to full H bridge. Initially low speed switching is
used to simplify the design for experimental purposes. It uses PMOS for high
side switching and NMOS for low side switching. The board includes discrete
component totem pole MOSFET drivers that include a charge pump to speed up PMOS
switching.

The board also includes an on-board ATTiny481 microcontroller, as well as a
connector that can be used with off-board control aimed at an ARM controller.
The microcontroller is required to perform all control functions using only the
measured output voltage. Input voltage and output current are also measured and
an overcurrent comparator is provided to provide instantaneous shutdown.

AVR microcontrollers do not provide sufficient inbuilt hardware for SMPS work
without additional external components, but they have some advantages for rapid
prototyping. Notably missing is a D/A converter. The microcontroller also needs
a regulated 5V for power and voltage reference, and a serial interface for
real-time control.

The use of recent Cortex-M ARM processors allows access to highly configurable
inbuilt hardware that is better suited to SMPS without additional external
components. STM32F3 and some STM32F05 have built-in configurable comparators
while all STM32F series and some LPC series have D/A converters that can feed
an external comparator. Some TI Stellaris processors have a comparator but no
D/A converter.

(c) K. Sarkies 29/10/2015
