SMPS AVR driven Circuit
-----------------------

This project aims to provide a generalized circuit board that can be used for a
variety of SMPS configurations up to full H bridge. Low speed switching is used
to simplify the design for experimental purposes.

The board includes discrete component totem pole MOSFET drivers that can be
adapted to include a charge pump for use with NMOS switching or to speed up
PMOS switching.

It also includes an on-board ATTiny481 microcontroller, as well as a
connector that can be used with off-board control.

AVR microcontrollers do not provide sufficient inbuilt hardware for SMPS work
without additional external components, but they have some advantages for rapid
prototyping. Notably missing is a D/A converter, provided here by smoothing
a PWM output. This will allow conventional analogue control at a higher speed
than possible with the firmware. The microcontroller also needs a regulated 5V
for power and voltage reference, and a serial interface for real-time control.

(c) K. Sarkies 14/09/2015

