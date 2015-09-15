SMPS ARM driven Circuit
-----------------------

This project aims to provide a general PCB for building SMPS circuits for a
variety of configurations up to full H bridge. Initially low speed switching is
used to simplify the design for experimental purposes.

The use of recent Cortex-M ARM processors allows access to highly configurable
inbuilt hardware that is better suited to SMPS without additional external
components. STM32F3 and some STM32F05 have built-in configurable comparators
while all STM32F series and some LPC series have D/A converters that can feed
an external comparator. Some TI Stellaris processors have a comparator but no
D/A converter.

(c) K. Sarkies 29/06/2015
