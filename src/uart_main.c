//******************************************************************************
//   MSP430G2xx3 Demo - USCI_A0, 9600 UART Echo ISR, DCO SMCLK
//
//   Description: Echo a received character, RX ISR used. Normal mode is LPM0.
//   USCI_A0 RX interrupt triggers TX Echo.
//   Baud rate divider with 1MHz = 1MHz/9600 = ~104.2
//   ACLK = n/a, MCLK = SMCLK = CALxxx_1MHZ = 1MHz
//
//                MSP430G2xx3
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |     P1.2/UCA0TXD|------------>
//            |                 | 9600 - 8N1
//            |     P1.1/UCA0RXD|<------------
//
//   D. Dang
//   Texas Instruments Inc.
//   February 2011
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************

#include <uart.h>
#include <math.h>

#ifndef  __MSP430_INTRINSICS_H_
	const char string1[] = { "MSP430GCC (Linux/Mac OS)" };
#else
	const char string1[] = { "CCS (Windows)" };
#endif

int i='0';

int
main (int argc, char *argv[])
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
//TODO: Configurar para receber qualquer velocidade
  BCSCTL1 = CALBC1_16MHZ;                    // Set DCO
  DCOCTL = CALDCO_16MHZ;

  char buff[20];

	P1DIR |= BIT0;						//output
	P1OUT &= ~BIT0;
	
	startup(9600);

	printf("\n\rFrequencia: %x\r\n",CALDCO_16MHZ_);
	printf("Valor: %x\r\n",BCSCTL1);
	printf("Some float (int) value : %d\r\n", (int)(1000*sin(30)));
	printf("Hexadecimal: %x[%d]\r\n",65,65);
	itoa (65,buff,2);
	printf("Binary: %s[%d]\r\n",buff,65);
	printf("Size do buff: %d \n\r",sizeof(buff));
	printf("Size do string1: %d [%s]\n\rEcho:\n\r>>",sizeof(string1),string1);
	
	while(1)
	{
		printf("%c",getchar());
	}

	IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
	  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
	  
	return 0;
}


//  Echo back RXed character, confirm TX buffer is ready first
//#pragma vector=USCIAB0RX_VECTOR
//__interrupt void USCI0RX_ISR(void)
interrupt(USCIAB0RX_VECTOR) USCI0RX_ISR (void)
{
  while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
  UCA0TXBUF = UCA0RXBUF;                    // TX -> RXed character
}
