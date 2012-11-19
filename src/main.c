
#include <legacymsp430.h>
#include <stdio.h>
#include <string.h>
#include "spi.h"
#include "diskio.h"
#include "pff.h"
//#include "serial.h"
#include <uart.h>

const unsigned long SMCLK_FREQ = 16000000;
const unsigned long BAUD_RATE = 9600;


static
void put_rc(FRESULT rc) {
    const char *p;
    static const char str[] =
            "OK\0" "DISK_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
                    "NOT_OPENED\0" "NOT_ENABLED\0" "NO_FILE_SYSTEM\0";
    FRESULT i;

    for (p = str, i = 0; i != rc && *p; i++) {
        while (*p++)
            ;
    }
    printf("rc=%lu FR_%s\n", (long int) rc, p);
}

int
main (int argc, char *argv[])
{

char *ptr;
char Line[128];
FRESULT res;
FATFS fs; /* File system object */
WORD bw;

	WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

	DCOCTL = 0x00; // Set DCOCLK to 16MHz
	BCSCTL1 = CALBC1_16MHZ;
	DCOCTL = CALDCO_16MHZ;
	//DCOCTL += 6; // the default calibrated clock on my MCU is a little slow

	P1DIR |= BIT4;
	P1SEL |= BIT4; // output SMCLK for measurement on P1.4

	/*
	* Configure USCI serial TX on P1.2, RX on P1.1. You need to change
	* turn your Launchpad TX/RX jumpers 90 degrees from the factory settings.
	*/

//	serial_initialize((SMCLK_FREQ + (BAUD_RATE >> 1)) / BAUD_RATE);
	startup (BAUD_RATE);

	/*
	* configure USCI SPI B
	*/
	spi_initialize();

	printf("Disk Initialize...\n");
	res = disk_initialize();
	
	put_rc(pf_mount(&fs));
	put_rc(pf_open(ptr));
	
//	get_line(Line, sizeof(Line));
	gets(Line,sizeof(Line));
	if (Line[0])
	{
		strcat(Line, "\r\n");
		if(!pf_write(Line, strlen(Line), &bw)); /* Write a line to the file */
			put_rc( pf_write(0, 0, &bw)); /* Finalize the write process */
	}
	else
		printf("Erro ao pegar string");
		
	put_rc(pf_mount(&fs)); /*umount device*/
	
	return 0;
}
