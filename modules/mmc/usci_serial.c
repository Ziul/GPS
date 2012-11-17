/*
 * usci_serial.c - USCI implementations of serial_initialize(), getchar(), putchar() and puts()
 *
 *  Created on: Oct 30, 2011
 *      Author: kimballr - rick@kimballsoftware.com
 *
 *  04-06-2012 - switched to oPossum style bps setting.
 */

#include <msp430.h>
#include <stdint.h>
#include "serial.h"

/**
 *  Really simple use of the hardware UART so that it will be
 *  compatible with our equally simple software ones. We setup the
 *  hardware UART using the pins and bit duration provided.
 *
 *  This code doesn't use any interrupts. Just simple polling
 *  to decide when to receive or transmit.
 */

/**
 * serial_initialize(bitclk_divisor) - configure USCI UCA0 as async serial port
 *  uses SMCLK as bitclock source and fractional divisor with over sampling mode.
 *
 * @params:
 *   uint32_t bitclk_divisor - should be (SMCLK_FREQ + (BPS >> 1)) / BPS
 *
 * Thanks to Kevin for original code from 43oh.com:
 * @see http://www.43oh.com/forum/viewtopic.php?f=10&t=2493
 *
 * P1.1 - RX
 * P1.2 - TX
 *
 */
void serial_initialize(const uint32_t bitclk_divisor) {
    UCA0CTL1 = UCSWRST; // Hold USCI in reset to allow configuration
    UCA0CTL0 = 0; // No parity, LSB first, 8 bits, one stop bit, UART (async)
    UCA0BR1 = (bitclk_divisor >> 12) & 0xFF; // High byte of whole divisor
    UCA0BR0 = (bitclk_divisor >> 4) & 0xFF; // Low byte of whole divisor
    UCA0MCTL = ((bitclk_divisor << 4) & 0xF0) | UCOS16; // Fractional divisor, over sampling mode
    UCA0CTL1 = UCSSEL_2; // Use SMCLK for bit rate generator, release reset

    P1SEL = BIT1 | BIT2; // P1.1=RXD, P1.2=TXD
    P1SEL2 = BIT1 | BIT2; // P1.1=RXD, P1.2=TXD
}

/**
 * int getchar() - read next char from serial port rx pin
 *
 *
 */
int getchar(void) {
    // sit and spin waiting for baudot heh I make myself laugh

    while (!(IFG2 & UCA0RXIFG)) {
        ; // busywait until UCA0 RX buffer is ready
    }

    return UCA0RXBUF; // return RXed character
}

/**
 * putchar(int c) - write char to serial port
 *
 */
int putchar(int c) {

    // make sure previous character has been sent
    // before trying to send another character
    while (!(IFG2 & UCA0TXIFG)) {
        ; // busywait until UCA0 TX buffer is ready
    }

    UCA0TXBUF = (uint8_t) c; // TX character

    return 0;
}
