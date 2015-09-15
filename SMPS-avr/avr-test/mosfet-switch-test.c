/* Simple pulse generator for testing MOSFET switch */

#define F_CPU 8000000

#include <avr/io.h>
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

/****************************************************************************/
/** @brief Main Program */

int main(void)
{

    outb(DDRB,(inb(DDRB) | 0x3C) & 0xBF); /* Outputs for four1 ports PB2-5 */

/* 25us for 20kHz signal. 1250us for 400Hz signal */
    while (1)
    {
        outb(PORTB,inb(PORTB) | 0x10);             /* Turn on PB4 */
        _delay_us(1250);
        outb(PORTB,inb(PORTB) & ~0x10);           /* Turn off PB4 */
        _delay_us(1250);
    }
 
}
/****************************************************************************/
