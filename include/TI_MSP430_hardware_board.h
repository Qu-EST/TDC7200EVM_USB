/* --COPYRIGHT--,BSD
* Copyright (c) 2014, Texas Instruments Incorporated
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* * Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
*
* * Neither the name of Texas Instruments Incorporated nor the names of
* its contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* --/COPYRIGHT--*/

#ifndef HEADER_TI_MSP430_HARDWARE_BOARD_H

#define HEADER_FILE_TI_MSP430_HARDWARE_BOARD_H

//Select one of the following to define XCLK & also the pin# later down

#define TI_TDC720x_RLED_PxOUT       P1OUT
#define TI_TDC720x_RLED_PxDIR       P1DIR
#define TI_TDC720x_RLED_PIN         BIT0

#define TI_TDC720x_GLED_PxOUT       P4OUT
#define TI_TDC720x_GLED_PxDIR       P4DIR
#define TI_TDC720x_GLED_PIN         BIT7
                         

#define TI_TDC720x_CSB1_PxOUT            P2OUT
#define TI_TDC720x_CSB1_PxDIR            P2DIR
#define TI_TDC720x_CSB1_PIN              BIT2 

#define TI_TDC720x_CSB2_PxOUT            P1OUT
#define TI_TDC720x_CSB2_PxDIR            P1DIR
#define TI_TDC720x_CSB2_PIN              BIT5

#define TI_TDC720x_START_MSP430_PxOUT            P2OUT
#define TI_TDC720x_START_MSP430_PxDIR            P2DIR
#define TI_TDC720x_START_MSP430_PIN              BIT3 

#define TI_TDC720x_ENABLE1_PxOUT         P6OUT
#define TI_TDC720x_ENABLE1_PxDIR         P6DIR
#define TI_TDC720x_ENABLE1_PIN           BIT5

#define TI_TDC720x_ENABLE2_PxOUT         P3OUT
#define TI_TDC720x_ENABLE2_PxDIR         P3DIR
#define TI_TDC720x_ENABLE2_PIN           BIT4


// for new TDC720xEVM (Michael's) use P1.6 instead of P8.1
#define TI_TDC720x_OSCENABLE_PxOUT            P1OUT
#define TI_TDC720x_OSCENABLE_PxDIR            P1DIR
#define TI_TDC720x_OSCENABLE_PIN              BIT6


#define TI_TDC720x_INTB_PORTx           PORT_2
#define TI_TDC720x_INTB_PxDIR           P2DIR
#define TI_TDC720x_INTB_PxIE            P2IE
#define TI_TDC720x_INTB_PxIES           P2IES
#define TI_TDC720x_INTB_PxIFG           P2IFG
#define TI_TDC720x_INTB_PxREN           P2REN
#define TI_TDC720x_INTB_PxOUT           P2OUT
#define TI_TDC720x_INTB_PxIN            P2IN
#define TI_TDC720x_INTB1_PIN             BIT6
#define TI_TDC720x_INTB2_PIN             BIT3
#define TI_TDC720x_INTB_VECTOR          PORT2_VECTOR

#define TI_TDC720x_MUX1_CTRL_PxOUT         P8OUT
#define TI_TDC720x_MUX1_CTRL_PxDIR         P8DIR
#define TI_TDC720x_MUX1_CTRL_PIN           BIT1

#define TI_TDC720x_MUX2_CTRL_PxOUT         P1OUT
#define TI_TDC720x_MUX2_CTRL_PxDIR         P1DIR
#define TI_TDC720x_MUX2_CTRL_PIN           BIT5

#define TI_TDC720x_REF_START_PxOUT         P2OUT
#define TI_TDC720x_REF_START_PxDIR         P2DIR
#define TI_TDC720x_REF_START_PxSEL         P2SEL
#define TI_TDC720x_REF_START_PxDS          P2DS
#define TI_TDC720x_REF_START_PIN           BIT4

#define TI_TDC720x_DTG_TRIG_PxOUT         P2OUT
#define TI_TDC720x_DTG_TRIG_PxDIR         P2DIR
#define TI_TDC720x_DTG_TRIG_PxSEL         P2SEL
#define TI_TDC720x_DTG_TRIG_PxDS         P2DS
#define TI_TDC720x_DTG_TRIG_PIN           BIT5
//----------------------------------------------------------------------------
// Select which port will be used for interface to TDC720x
//----------------------------------------------------------------------------
//#define TI_TDC720x_SER_INTF            TI_TDC720x_SER_INTF_USCIA0_5xx   // 5xx, 6xx family USCIA0 SPI Interface to TDC720x
//#define TI_TDC720x_SER_INTF            TI_TDC720x_SER_INTF_USCIA1_5xx   // 5xx, 6xx family USCIA1 SPI Interface to TDC720x
//#define TI_TDC720x_SER_INTF            TI_TDC720x_SER_INTF_USCIA2_5xx   // 5xx, 6xx family USCIA2 SPI Interface to TDC720x
//#define TI_TDC720x_SER_INTF            TI_TDC720x_SER_INTF_USCIA3_5xx   // 5xx, 6xx family USCIA3 SPI Interface to TDC720x
#define TI_TDC720x_SER_INTF            TI_TDC720x_SER_INTF_USCIB0_5xx   // 5xx, 6xx family USCIB0 SPI Interface to TDC720x
//#define TI_TDC720x_SER_INTF            TI_TDC720x_SER_INTF_USCIB1_5xx   // 5xx, 6xx family USCIB1 SPI Interface to TDC720x
//#define TI_TDC720x_SER_INTF            TI_TDC720x_SER_INTF_USCIB2_5xx   // 5xx, 6xx family USCIB2 SPI Interface to TDC720x
//#define TI_TDC720x_SER_INTF            TI_TDC720x_SER_INTF_USCIB3_5xx   // 5xx, 6xx family USCIB3 SPI Interface to TDC720x
//#define TI_TDC720x_SER_INTF            TI_TDC720x_SER_INTF_USART0       // USART0 SPI Interface to TDC720x
//#define TI_TDC720x_SER_INTF            TI_TDC720x_SER_INTF_USART1       // USART1 SPI Interface to TDC720x
//#define TI_TDC720x_SER_INTF            TI_TDC720x_SER_INTF_USCIA0       // 2xx, 4xx family USCIA0 SPI Interface to TDC720x
//#define TI_TDC720x_SER_INTF            TI_TDC720x_SER_INTF_USCIA1       // 2xx, 4xx family USCIA1 SPI Interface to TDC720x
//#define TI_TDC720x_SER_INTF            TI_TDC720x_SER_INTF_USCIB0       // 2xx, 4xx family USCIB0 SPI Interface to TDC720x
//#define TI_TDC720x_SER_INTF            TI_TDC720x_SER_INTF_USCIB1       // 2xx, 4xx family USCIB1 SPI Interface to TDC720x
//#define TI_TDC720x_SER_INTF            TI_TDC720x_SER_INTF_USI          // G2xx value seris SPI Interface to TDC720x

#endif                                                                         // HEADER_FILE_TI_MSP430_HARDWARE_BOARD_H
