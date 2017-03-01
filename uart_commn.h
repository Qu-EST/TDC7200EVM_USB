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
//******************************************************************************
#ifndef UART_H_
#define UART_H_

#define ACLK_FREQ 32768 // ACLK frequency of MCU, in Hz
#define SMCLK_FREQ 4000000 // SMCLK frequency of MCU, in Hz
#define USCI_A_MODULE  1     // Select which USCI_A module to use [0-1] 
#define UART_BAUDRATE   38400 //Specify baudrate
//#define UART_BAUDRATE  230400 //Specify baudrate
#define UART_BAUDRATE_REG  (unsigned short)(SMCLK_FREQ/UART_BAUDRATE)

//define uart registers
#define UART_PxIN      P4IN
#define UART_PxOUT     P4OUT
#define UART_PxDIR     P4DIR
#define UART_PxIFG     P4IFG
#define UART_PxIES     P4IES
#define UART_PxIE      P4IE
#define UART_PxSEL     P4SEL
#define UART_PxREN     P4REN

#define UART_RX_PAD    BIT5 // select uart pins P4.5: GPIO4
#define UART_TX_PAD    BIT4 // select uart pins P4.4: GPIO3


#include "TI_USCI_UART_Regs.h"
//Global Functions
extern void InitUART(void);
extern void StartUARTRx(void);
extern void StopUARTRx(void);
extern void WriteUART(unsigned char data);
extern void putsUART(unsigned char *buffer, unsigned short StringSize);


#endif /*UART_H_*/
