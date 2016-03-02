Switch Mode Power Supplies
--------------------------

This project aims to produce a number of SMPS circuits that use only a single
inductor. As such they cannot be isolating, and are oriented to simple dc-dc
conversions with a single output voltage or current. Low power circuits do
not require current boost. Circuits of slightly higher powers use MOSFET
current boost circuits. Switching frequencies are kept low to simplify PCB
design, at the expense of high EMI. Input voltages are constrained to less
than 30V for use in low voltage solar powered installations.

The circuits are intended to demonstrate basic SMPS operations and to
investigate a range of issues such as management of EMI. In time it is hoped
to provide useful working circuits.

- Buck Asynchronous MC34063. This is a very basic circuit taken mainly from the
  MC34063 datasheet.
- Buck Asynchronous MC34063 boosted with P-MOSFET. This extends the current
  range of the above circuit using a simple driver circuit.
- Buck Asynchronous MC34063 constant current circuit for driving LEDs.
- Buck Synchronous with digital ATMEL 8-bit microcontroller.
- Buck-Boost Synchronous full H with digital microcontroller control. This
  provides a template for building a range of experimental circuits up to
  moderately high currents.

More information is provided at [Jiggerjuice](http://www.jiggerjuice.info/electronics/projects/power/index.html

(c) K. Sarkies 24/10/2014

