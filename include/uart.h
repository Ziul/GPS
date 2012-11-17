/**
 * @file
 * @author  Luiz Oliveira <ziuloliveira@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This firmware will allow any MSP430 with USCI to send/recive data under RS232;
 * It was tested only under MSP430G2553
 */

#ifdef __cplusplus
extern "C" {
#endif

//#include  "msp430g2553.h"
#include <legacymsp430.h>
#include <errno.h>
#include <myerrno.h>   /* If without the errors defined in errno */
#include <stdio.h>
#include <stdlib.h>


#ifdef __MSP430_HAS_USCI__

#ifndef __GPS__
#define __GPS__


#define PROMP_OUT		//print on output the input of gets while reading


/**
 * Start up the UART
 *
 * @param	long int baud	The baud rate of the UART
 * @return			SUCCESS if the UART is Started up right
 */
extern
int startup (long int baud);


/**
 * Function used to send a char to the standard output.
 *
 * @param	const char *msg	 String which will be print.
 * @param	int size	 Size from the string.
 * @return			SUCCESS if printed. EINVAL if *msg is NULL
 */
extern
int send (const char *msg,int size);


/**
 * Function used to recive a char to the standard input. 
 *
 * @param[i]	char *msg	 String which will be read froms inpust standard.
 * @param		int size	 Size max of the string.
 * @return				How many characters it got.
 */
extern
int recive (char *msg, int size);


/**
 * Function used to get a char from the standard input. Used by the functions from stdio.h.
 * This function define which will be the standard input
 *
 * @return			The char got as a integer.
 */
extern
int getchar(void);


/**
 * Function used to recive a char to the standard input. 
 *
 * @param[i]	char *s	 String which will be read froms inpust standard.
 * @param		int size	 Size max of the string.
 * @return				How the string got.
 */
extern
char *gets(char *s,int size);

#endif /*ends GPS functions*/

#endif /* ends USCI test*/
#ifdef __cplusplus
}
#endif
