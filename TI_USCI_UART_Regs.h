/*  Copyright 2011-2012 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user who
  downloaded the software, his/her employer (which must be your employer) and
  Texas Instruments Incorporated (the "License"). You may not use this Software
  unless you agree to abide by the terms of the License. The License limits your
  use, and you acknowledge, that the Software may not be modified, copied or
  distributed unless embedded on a Texas Instruments microcontroller which is 
  integrated into your product. Other than for the foregoing purpose, you may 
  not use, reproduce, copy, prepare derivative works of, modify, distribute, 
  perform, display or sell this Software and/or its documentation for any 
  purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL TEXAS
  INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL
  EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT
  LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL
  DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS,
  TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT
  LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
*******************************************************************************/
//******************************************************************************
// CC430/LM57 Interface Code                 | 
//
// Vishy N Viswanathan
// Texas Instruments Inc.
// Feb 2013
// Built IAR Embedded Workbench Version:  5.5x
//******************************************************************************
// Change Log:
//******************************************************************************
// Version:  1.00
// Comments: Initial Release Version
//******************************************************************************
#ifndef USCI_UART_REGS
#define USCI_UART_REGS



#if (USCI_A_MODULE == 0)
  #ifdef   __MSP430_HAS_USCI_A0__
    #define UCAxCTL0		   UCA0CTL0		/* USCI Control Register 0 */	
    #define UCAxCTL1    	 UCA0CTL1		/* USCI Control Register 1 */
    #define UCAxBR         UCA0BRW
    #define UCAxBR0     	 UCA0BR0			/* USCI Baud Rate 0 */
    #define UCAxBR1     	 UCA0BR1			/* USCI Baud Rate 1 */
    #define UCAxMCTL   	   UCA0MCTL		/* USCI modulation control register*/
    #define UCAxSTAT    	 UCA0STAT		/* USCI Status Register */
    #define UCAxRXBUF   	 UCA0RXBUF		/* USCI Receive Buffer */
    #define UCAxTXBUF   	 UCA0TXBUF		/* USCI Transmit Buffer */
    #define UCAxABCTL   	 UCA0ABCTL		/* USCI auto baud control register*/
    #define UCAxIE 		     UCA0IE       		/* USCI Interrupt Enable Register */
    #define UCAxIFG		     UCA0IFG      		/* USCI Interrupt Flags Register */                        
    #define UCAxIV         UCA0IV  /* USCI Interrupt Vector Register */

    #define UCAxRX_VECTOR  USCI_A0_VECTOR

    #define UCAxRXIE       BIT0
    #define UCAxTXIE       BIT1
    #define UCAxRXIFG      BIT0
    #define UCAxTXIFG      BIT1
  #endif
#elif (USCI_A_MODULE == 1)
  #ifdef   __MSP430_HAS_USCI_A1__
    #define UCAxCTL0		   UCA1CTL0			/* USCI Control Register 0 */	
    #define UCAxCTL1    	 UCA1CTL1			/* USCI Control Register 1 */
    #define UCAxBR         UCA1BRW
    #define UCAxBR0     	 UCA1BR0			/* USCI Baud Rate 0 */
    #define UCAxBR1     	 UCA1BR1			/* USCI Baud Rate 1 */
    #define UCAxMCTL   	   UCA1MCTL			/* USCI modulation control register*/
    #define UCAxSTAT    	 UCA1STAT			/* USCI Status Register */
    #define UCAxRXBUF   	 UCA1RXBUF			/* USCI Receive Buffer  */
    #define UCAxTXBUF   	 UCA1TXBUF			/* USCI Transmit Buffer */
    #define UCAxABCTL   	 UCA1ABCTL			/* USCI auto baud control register*/
    #define UCAxIE 		     UCA1IE       		/* USCI Interrupt Enable Register */
    #define UCAxIFG		     UCA1IFG      		/* USCI Interrupt Flags Register  */                        
    #define UCAxIV         UCA1IV   /* USCI Interrupt Vector Register */
    
    #define UCAxRX_VECTOR  USCI_A1_VECTOR

    #define UCAxRXIE       BIT0
    #define UCAxTXIE       BIT1
    #define UCAxRXIFG      BIT0
    #define UCAxTXIFG      BIT1
  #endif
#else 
  #error "USCI Module not supported"
#endif  

#endif // USCI_UART_REGS


