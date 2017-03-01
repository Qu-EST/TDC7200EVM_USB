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
#include <stdint.h>
#include <cstring>
#include <stdio.h>
#include "TI_TDC720x.h"  
#include "TI_MSP430.h"
#include "TI_MSP430_hardware_board.h"
#include "TI_MSP430_spi.h"
#include "timer_queue.h"
#include "tdc720x_calculation.h"
#include "uart_commn.h"
//******************************************************************************
/**
* @brief Function Name:     InitUART.                                                 
* @brief  Description: This function initialized the USCI-UART.
* @param none                                                   
* @return none                                                    
*/  
//******************************************************************************
void InitUART(void){ 
  
  UCAxCTL1 |= UCSWRST;                      // **Stops USCI state machine** 
  
  UART_PxREN &= ~(UART_TX_PAD|UART_RX_PAD); //Disable port pull resistors
  UART_PxDIR &= ~UART_RX_PAD;               // Configure Px.X as input
  UART_PxDIR |= UART_TX_PAD;                // Configure Px.X as output
  UART_PxSEL |= (UART_TX_PAD|UART_RX_PAD);  // Px.x & Px.X = USCI_Ax TXD & RXD
  
  UCAxCTL0 = 0x00;                          // No parity, 8-N-1 mode
  UCAxCTL1 |= UCSSEL__SMCLK;                // Select 12MHz SMCLK as clock source for the UART
  UCAxBR = UART_BAUDRATE_REG;               // defines baudrate
  UCAxMCTL =  (UCBRF_0|UCBRS_3);            //    38400  baudrate selection @ 4MHz
 // UCAxMCTL =  (UCBRF_0|UCBRS_6);            //    9600 baudrate selection @ 4MHz
//  UCAxMCTL =  (UCBRF_0|UCBRS_4);            //  57600  baudrate selection @ 4MHz  
  UCAxIFG &= ~UCAxRXIFG;          //Clear RX IFG
  
  UCAxCTL1 &= ~UCSWRST;           // **Initialize USCI state machine**
//  UCAxIE |= UCAxRXIE;             // Enable USCI_A1 RX interrupt
//  UCAxIE |= UCAxTXIE;             // Enable USCI_A1 TX interrupt
  
}  
//******************************************************************************
/**
* @brief Function Name: StopUARTRx.                                                 
* @brief  Description: This function disables the USCI-UART interrupts in order
*                     to not acknowledge byte sent by the TX.
* @param none                                                   
* @return none                                                    
*/
//******************************************************************************
void StopUARTRx(void){

  UCAxIE &= ~UCAxRXIE;             // Disable USCI RX interrupt 
  
}
//******************************************************************************
/**
* @brief Function Name: StartUARTRx.                                                 
* @brief  Description: This function enables the USCI-UART interrupts in order
*                     to be ready to receive USBID messages.
* @param none                                                   
* @return none                                                    
*/
//******************************************************************************
void StartUARTRx(void){
  
  UCAxIFG &= ~UCAxRXIFG;           // Clear RX IFG
  UCAxIE |= UCAxRXIE;              // Enable USCI RX interrupt
}
//******************************************************************************
/**
* @brief Function Name: WriteUART.                                      
* @brief  Description: This function writes data into the UART TX REG.
* @param unsigned char data the data to be written      
* @return None                                          
*/
//******************************************************************************
void WriteUART(unsigned char data)
{  
  unsigned char TimeOutCounter = 0;
  // USCI_A0 TX buffer ready?
  while (!(UCAxIFG & UCAxTXIFG)){ 
  	    __delay_cycles(1000);
        TimeOutCounter++;
        if(TimeOutCounter >= 200)
          return ;    
                     
  }
  //if so...then send byte
  UCAxTXBUF = data;

}
//******************************************************************************
/**
* @brief Function Name: putsUART .                                           
* @brief  Description: This function puts the data string to be transmitted 
*                     into the transmit buffer (till NULL character)  .     
* @param unsigned char * address of the string buffer to be   
*                     transmitted                                          
* @return None                                                
*/
//******************************************************************************
void putsUART(unsigned char *buffer, unsigned short StringSize)
{
  unsigned short i;
  unsigned char TimeOutCounter = 0;
  
  // transmit till NULL character is encountered 
  for(i=0;i<StringSize;i++)
  {
  // USCI_A0 TX buffer ready?
  while (!(UCAxIFG & UCAxTXIFG)){ 
  	    __delay_cycles(1000);
        TimeOutCounter++;
        if(TimeOutCounter >= 200)
          return ;    
                      
  }                    //wait until Tx buffer is empty      
  UCAxTXBUF = *buffer++;    // transfer data word to TX reg
            
  }
}
//******************************************************************************
