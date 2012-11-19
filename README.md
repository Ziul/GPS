GPS
===
Tested on
-------------

* UART is working even for debug or recive files.

* Compillers
	* MSP430
		* msp430-gcc -- GNU		( OK )
		* Code Compose -- CCS	(    )
		
# Inicializando a UART

## uart.h

	Writed by Luiz. Just include the files on */module/uart* inside the library and start it up with :
	
	'startup (BAUD_RATE);'
	
	Where **BAUD_RATE** is the frequency that is wanted for RS232.
	
## usci_serial.h / serial.h

	Writed by Kimballr (rick@kimballsoftware.com). Include the file */module/usci_serial.h* inside the library and start it up with:
	
	'serial_initialize((SMCLK_FREQ + (BAUD_RATE >> 1)) / BAUD_RATE);'
	
	Where **BAUD_RATE** is the frequency that is wanted for RS232 and **SMCLK_FREQ** is the frequency of the MSP430.
