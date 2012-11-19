/*---------------------------------------------------------------*/
/* Petit FAT file system module test program R0.02 (C)ChaN, 2009 */
/*---------------------------------------------------------------*/

/*---------------------------------------------------------------
 * 04-06-2012 rkimball - Initial msp430g2553 implementation done
 *            for msp430-gcc using USCI UART(USCA0) and SPI(USCB0)
 *---------------------------------------------------------------
 */

#include <msp430.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "spi.h"
#include "diskio.h"
#include "pff.h"
#include "serial.h"

const unsigned long SMCLK_FREQ = 16000000;
const unsigned long BAUD_RATE = 9600;

/*---------------------------------------------------------------*/
/* Work Area                                                     */
/*---------------------------------------------------------------*/

char Line[128];

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

static
void put_drc(long int res) {
    printf("rc=%lu\n", res);
}

static
void get_line(char *buff, BYTE len) {
    BYTE c, i;

    i = 0;
    for (;;) {
        c = getchar();
        if (c == '\r')
            break;
        if ((c == '\b') && i)
            i--;
        if ((c >= ' ') && (i < len - 1))
            buff[i++] = c;
    }
    buff[i] = 0;
    putchar('\n');
}

#if _USE_READ
static
void put_dump(const BYTE* buff, /* Pointer to the byte array to be dumped */
DWORD addr, /* Heading address value */
WORD len /* Number of bytes to be dumped */
) {
    int n;

    printf("%08lX ", addr); /* address */

    for (n = 0; n < len; n++) { /* data (hexdecimal) */
        printf(" %02X", buff[n]);
    }

    printf("  ");

    for (n = 0; n < len; n++) { /* data (ascii) */
        putchar(((buff[n] < 0x20) || (buff[n] >= 0x7F)) ? '.' : buff[n]);
        ;
    }

    putchar('\n');
}
#endif

/**
 * setup() - configure DCO clock system, UART and SPI.
 */

void setup() {
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

    serial_initialize((SMCLK_FREQ + (BAUD_RATE >> 1)) / BAUD_RATE);

    /*
     * configure USCI SPI B
     */
    spi_initialize();
}

int main(void) {
    char *ptr;
    FRESULT res;
#if _USE_READ
    WORD s1, s2;
    DWORD ofs;
#endif
#if _USE_DIR
    DIR dir; /* Directory object */
    FILINFO fno; /* File information */
#endif
#if _USE_WRITE
    WORD bw;
    WORD n = 0;
#endif
#if _USE_LSEEK
    DWORD p1;
#endif
    FATFS fs; /* File system object */

    setup();

    __enable_interrupt(); // don't really need this we do blocking reads

    printf("\nPFF test monitor\n");

    for (;;) {
        printf("> ");
        get_line(Line, sizeof(Line));
        ptr = Line;
        switch (*ptr++) {
        case 'd':
            switch (*ptr++) {
            case 'i': /* di - Initialize physical drive */
                printf("Disk Initialize...\n");
                res = disk_initialize();
                put_drc(res);
                break;
            }
            break;
        case 'f':
            switch (*ptr++) {
            case 'i': /* fi - Mount the volume */
                put_rc(pf_mount(&fs));
                break;

            case 'o': /* fo <file> - Open a file */
                while (*ptr == ' ')
                    ptr++;
                put_rc(pf_open(ptr));
                break;

#if _USE_READ
            case 'd': /* fd - Read the file 128 bytes and dump it */
                ofs = fs.fptr;
                res = pf_read(Line, sizeof(Line), &s1);
                if (res != FR_OK) {
                    put_rc(res);
                    break;
                }
                ptr = Line;
                while (s1) {
                    s2 = (s1 >= 16) ? 16 : s1;
                    s1 -= s2;
                    put_dump((BYTE*) ptr, ofs, s2);
                    ptr += 16;
                    ofs += 16;
                }
                break;

            case 't': /* ft - Type the file data via dreadp function */
                do {
                    res = pf_read(0, 32768, &s1);
                    if (res != FR_OK) {
                        put_rc(res);
                        break;
                    }
                } while (s1 == 32768);
                break;
#endif

#if _USE_LSEEK
            case 'e': /* fe - Move file pointer of the file */
                p1 = atol(ptr);
                res = pf_lseek(p1);
                put_rc(res);
                if (res == FR_OK) {
                    printf("fptr = %lu(0x%lX)\n", fs.fptr, fs.fptr);
                }
                break;
#endif

#if _USE_DIR
            case 'l': /* fl [<path>] - Directory listing */
                while (*ptr == ' ')
                    ptr++;
                res = pf_opendir(&dir, ptr);
                if (res) {
                    put_rc(res);
                    break;
                }
                s1 = 0;
                for (;;) {
                    res = pf_readdir(&dir, &fno);
                    if (res != FR_OK) {
                        put_rc(res);
                        break;
                    }
                    if (!fno.fname[0])
                        break;
                    if (fno.fattrib & AM_DIR)
                        printf("   <DIR>   %s\n", fno.fname);
                    else
                        printf("%9lu  %s\n", fno.fsize, fno.fname);
                    s1++;
                }
                printf("%u item(s)\n", s1);
                break;
#endif

#if _USE_WRITE
            case 'p': /* fp - Write console input to the file */
                printf(
                        "Enter lines to write. A blank line finalize the write operation.\n");
                for (;;) {
                    get_line(Line, sizeof(Line));
                    if (!Line[0])
                        break;
                    strcat(Line, "\r\n");
                    res = pf_write(Line, strlen(Line), &bw); /* Write a line to the file */
                    if (res)
                        break;
                }
                res = pf_write(0, 0, &bw); /* Finalize the write process */
                put_rc(res);
                break;

            case 'w': /* fw - write into write.txt, see write limitation in Petit FatFS documentation */
                printf(
                        "\nTrying to open an existing file for writing (write.txt)...\n");
                res = pf_open("WRITE.TXT");
                if (res != FR_OK) {
                    put_rc(res);
                } else {
                    printf("Writing 100 lines of text data\n");
                    do {
                        ltoa(++n, Line, 10);
                        strcat(Line,
                                " The quick brown fox jumps over the lazy dog. 1234567890\r\n");
                        res = pf_write(Line, strlen(Line), &bw);
                        if (res != FR_OK) {
                            put_rc(res);
                            break;
                        }
                        if (n & 0x10) {
                            printf(".");
                        }
                    } while (n < 100);
                    if (res == FR_OK) {
                        printf("\nFinalizing the file write process.\n");
                        res = pf_write(0, 0, &bw);
                        if (res != FR_OK)
                            put_rc(res);
                    }
                }
                break;
#endif

            }
            break;
        } /* end switch */
    } /* end for */

    return 0;
}
