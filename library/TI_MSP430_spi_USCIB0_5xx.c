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

#include <stdint.h>
#include "TI_TDC720x.h"
#include "TI_MSP430.h"
#include "TI_MSP430_hardware_board.h"
#include "TI_MSP430_spi.h"

//******************************************************************************
// Support for 552x USCI_B0
//******************************************************************************
#if TI_TDC720x_SER_INTF == TI_TDC720x_SER_INTF_USCIB0_5xx

extern uint8_t TDC720x_Parallel_Read;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void TI_TDC720x_SPISetup(void)
//
//  DESCRIPTION:
//  Configures the assigned interface to function as a SPI port and
//  initializes it.
//------------------------------------------------------------------------------
void TI_TDC720x_SPISetup(void)
{
  TI_TDC720x_CSB1_PxOUT |= TI_TDC720x_CSB1_PIN;
  TI_TDC720x_CSB1_PxDIR |= TI_TDC720x_CSB1_PIN;                                // /CS disable
  
  TI_TDC720x_CSB2_PxOUT |= TI_TDC720x_CSB2_PIN;
  TI_TDC720x_CSB2_PxDIR |= TI_TDC720x_CSB2_PIN;  

  UCB0CTL1 |= UCSWRST;                                                         // **Disable USCI state machine**
  UCB0CTL0 |= UCMST+UCMSB+UCCKPH+UCSYNC;                                       // 3-pin, 8-bit SPI master 
  UCB0CTL1 |= UCSSEL_2;                                                        // SMCLK
  //
  UCB0BR0 = 0x02;                                                              // UCLK/2 (should be 2MHz for 4MHz SMCLK)
  UCB0BR1 = 0;

  TI_TDC720x_SPI_USCIB0_PxSEL1 |= TI_TDC720x_SPI_USCIB0_SIMO
                                   | TI_TDC720x_SPI_USCIB0_SOMI;              
  TI_TDC720x_SPI_USCIB0_PxSEL2 |= TI_TDC720x_SPI_USCIB0_UCLK;
                                                                               // SPI option select
  TI_TDC720x_SPI_USCIB0_PxDIR1 |= TI_TDC720x_SPI_USCIB0_SIMO;
  TI_TDC720x_SPI_USCIB0_PxDIR2 |= TI_TDC720x_SPI_USCIB0_UCLK;

                                                                               // SPI TXD out direction
  
  UCB0CTL1 &= ~UCSWRST;                                                        // **Initialize USCI state machine**

  // Test reading DOUT1 and DOUT2 in parallel
  // initialize UCB1 as the second spi port  
  UCB1CTL1 |= UCSWRST;                                                         // **Disable USCI state machine**
  // make sure it is in slave mode
  UCB1CTL0 &= ~UCMST; 
  UCB1CTL0 |= UCMSB+UCCKPH+UCSYNC;                                             // 3-pin, 8-bit SPI slave 

  TI_TDC720x_SPI_USCIB1_PxSEL1 |= TI_TDC720x_SPI_USCIB1_SIMO
                                   | TI_TDC720x_SPI_USCIB1_SOMI;              
  TI_TDC720x_SPI_USCIB1_PxSEL2 |= TI_TDC720x_SPI_USCIB1_UCLK;
                                                                               // SPI option select
  TI_TDC720x_SPI_USCIB1_PxDIR1 |= TI_TDC720x_SPI_USCIB1_SIMO;
  TI_TDC720x_SPI_USCIB1_PxDIR2 |= TI_TDC720x_SPI_USCIB1_UCLK;
                                                                               // SPI TXD out direction
  
  UCB1CTL1 &= ~UCSWRST;                                                        // **Initialize USCI state machine**

}

//------------------------------------------------------------------------------
//  void TI_TDC720x_SPIByteWriteReg(uint8_t addr, uint8_t value, uint8_t dev)
//
//  DESCRIPTION:
//  Writes "value" to a single configuration register at address "addr". Note addr is
//  is 8-bit, data written is 8-bit 
//------------------------------------------------------------------------------
void TI_TDC720x_SPIByteWriteReg(uint8_t addr, uint8_t value, uint8_t dev)
{
  uint8_t inst;
 
  if (dev == TDC720x_TDC1)
  {
    TI_TDC720x_CSB1_PxOUT &= ~TI_TDC720x_CSB1_PIN;                             // /CS enable 
    TI_TDC720x_CSB2_PxOUT |= TI_TDC720x_CSB2_PIN;  
  } else
  {
    TI_TDC720x_CSB2_PxOUT &= ~TI_TDC720x_CSB2_PIN;
    TI_TDC720x_CSB1_PxOUT |= TI_TDC720x_CSB1_PIN;
  }
  // write mode and auto inc off
  inst = TDC720x_WRITE_BIT | addr;                                             
  inst = TDC720x_AUTOINC_OFF_BIT & inst;
   
  while (!(UCB0IFG&UCTXIFG));                                                  // Wait for TXBUF ready
  UCB0TXBUF = inst;                                                            // Send MSB of address

   
  while (!(UCB0IFG&UCTXIFG));                                                  // Wait for TXBUF ready
  UCB0TXBUF = value;                                                           // Send data value  
  
   
  while (UCB0STAT & UCBUSY);                                                   // Wait for TX complete

  TI_TDC720x_CSB1_PxOUT |= TI_TDC720x_CSB1_PIN;                                // /CS disable
  TI_TDC720x_CSB2_PxOUT |= TI_TDC720x_CSB2_PIN;

}

//------------------------------------------------------------------------------
//  uint8_t TI_TDC720x_SPIByteReadReg(uint8_t addr, uint8_t dev)
//
//  DESCRIPTION:
//  Reads a single configuration register byte at address "addr" and returns the
//  value read. Note addr is 8-bit, return data is 8-bit  
//------------------------------------------------------------------------------
uint8_t TI_TDC720x_SPIByteReadReg(uint8_t addr, uint8_t dev)
{
  uint8_t x, inst;
  
  if (dev == TDC720x_TDC1)
  {
    TI_TDC720x_CSB1_PxOUT &= ~TI_TDC720x_CSB1_PIN;                             // /CS enable 
    TI_TDC720x_CSB2_PxOUT |= TI_TDC720x_CSB2_PIN;  
  } else
  {
    TI_TDC720x_CSB2_PxOUT &= ~TI_TDC720x_CSB2_PIN;
    TI_TDC720x_CSB1_PxOUT |= TI_TDC720x_CSB1_PIN;
  }
  
  // read mode and auto_inc off
  inst = TDC720x_READ_BIT & addr;                                               
  inst = TDC720x_AUTOINC_OFF_BIT & inst;

  while (!(UCB0IFG&UCTXIFG));                                                  // Wait for TXBUF ready
  UCB0TXBUF = inst;                                                            // Send LSB of address
    
  while (!(UCB0IFG&UCTXIFG));                                                  // Wait for TXBUF ready
  UCB0TXBUF = 0;                                                               // Dummy write so we can read data
    
  while (UCB0STAT & UCBUSY);                                                   // Wait for TX complete  
  x = UCB0RXBUF;                                                               // Read data


  if ((dev == TDC720x_TDC2) && (TDC720x_Parallel_Read == 1))  
  {
    while (UCB1STAT & UCBUSY);
//    while (!(UCB1IFG&UCRXIFG));
    x = UCB1RXBUF;
    
  }
  
  TI_TDC720x_CSB1_PxOUT |= TI_TDC720x_CSB1_PIN;                                // /CS disable
  TI_TDC720x_CSB2_PxOUT |= TI_TDC720x_CSB2_PIN;
     
  return x;
}

//------------------------------------------------------------------------------
//  void TI_TDC720x_SPIAutoIncWriteReg(uint8_t addr, uint8_t *buffer, 
//                                     uint8_t size, uint8_t dev)
//
//  DESCRIPTION:
//  Writes "value" to a single configuration register at address "addr". Note addr is
//  is 8-bit, data written is 8-bit 
//------------------------------------------------------------------------------
void TI_TDC720x_SPIAutoIncWriteReg(uint8_t addr, uint8_t *buffer, 
                                   uint8_t size, uint8_t dev)
{
  uint8_t i, inst;
 

  if (dev == TDC720x_TDC1)
  {
    TI_TDC720x_CSB1_PxOUT &= ~TI_TDC720x_CSB1_PIN;                             // /CS enable 
    TI_TDC720x_CSB2_PxOUT |= TI_TDC720x_CSB2_PIN;  
  } else
  {
    TI_TDC720x_CSB2_PxOUT &= ~TI_TDC720x_CSB2_PIN;
    TI_TDC720x_CSB1_PxOUT |= TI_TDC720x_CSB1_PIN;
  }
  
  // write mode and auto inc on
  inst = TDC720x_WRITE_BIT | addr;                                             
  inst = TDC720x_AUTOINC_ON_BIT | inst;
   
  while (!(UCB0IFG&UCTXIFG));                                                  // Wait for TXBUF ready
  UCB0TXBUF = inst;                                                            // Send MSB of address

   
  for(i=0; i<size; i++)
  {
    while (!(UCB0IFG&UCTXIFG));                                                // Wait for TXBUF ready
    UCB0TXBUF = *(buffer+i);                                                   // Dummy write so we can read data
  }  
   
  while (UCB0STAT & UCBUSY);                                                   // Wait for TX complete

  TI_TDC720x_CSB1_PxOUT |= TI_TDC720x_CSB1_PIN;                                // /CS disable
  TI_TDC720x_CSB2_PxOUT |= TI_TDC720x_CSB2_PIN;

}
//------------------------------------------------------------------------------
//  void TI_TDC720x_SPIAutoIncReadReg(uint8_t addr, uint8_t *buffer, 
//                                    uint8_t size, uint8_t dev)
//
//  DESCRIPTION:
//  
//  
//------------------------------------------------------------------------------
void TI_TDC720x_SPIAutoIncReadReg(uint8_t addr, uint8_t *buffer, 
                                  uint8_t size, uint8_t dev)
{
  uint8_t i, inst;
 

  if (dev == TDC720x_TDC1)
  {
    TI_TDC720x_CSB1_PxOUT &= ~TI_TDC720x_CSB1_PIN;                             // /CS enable 
    TI_TDC720x_CSB2_PxOUT |= TI_TDC720x_CSB2_PIN;  
  } else
  {
    TI_TDC720x_CSB2_PxOUT &= ~TI_TDC720x_CSB2_PIN;
    TI_TDC720x_CSB1_PxOUT |= TI_TDC720x_CSB1_PIN;
  }
  
  // read mode and auto inc on
  inst = TDC720x_READ_BIT & addr;                                             
  inst = TDC720x_AUTOINC_ON_BIT | inst;
   
  while (!(UCB0IFG&UCTXIFG));                                                  // Wait for TXBUF ready
  UCB0TXBUF = inst;                                                            // Send address

   
  for(i=0; i<size; i++)
  {
    while (!(UCB0IFG&UCTXIFG));                                                // Wait for TXBUF ready
    UCB0TXBUF = 0;                                                             // Dummy write so we can read data
       
    while (UCB0STAT & UCBUSY);                                                 // Wait for TX complete  
    *(buffer+i) = UCB0RXBUF;                                                   // Read register data
    
    if ((dev == TDC720x_TDC2) && (TDC720x_Parallel_Read == 1))  
    {
      while (!(UCB1IFG&UCRXIFG));
      *(buffer+i) = UCB1RXBUF;
    }
  }  
   
  TI_TDC720x_CSB1_PxOUT |= TI_TDC720x_CSB1_PIN;                                // /CS disable
  TI_TDC720x_CSB2_PxOUT |= TI_TDC720x_CSB2_PIN;

}
//------------------------------------------------------------------------------
//  void TI_TDC720x_SPIAllReadReg(uint8_t *buffer, uint8_t dev)
//
//  DESCRIPTION:
//  Special read function for reading all measurement result registers.
//  Data read are deposited sequentially starting at address "buffer" 
//------------------------------------------------------------------------------
void TI_TDC720x_SPIAllReadReg(uint8_t *buffer, uint8_t dev)
{

  uint8_t i, inst;
  
  if (dev == TDC720x_TDC1)
  {
    TI_TDC720x_CSB1_PxOUT &= ~TI_TDC720x_CSB1_PIN;                             // /CS enable 
    TI_TDC720x_CSB2_PxOUT |= TI_TDC720x_CSB2_PIN;  
  } else
  {
    TI_TDC720x_CSB2_PxOUT &= ~TI_TDC720x_CSB2_PIN;
    TI_TDC720x_CSB1_PxOUT |= TI_TDC720x_CSB1_PIN;
  }
  
  // read mode and auto_inc on
  inst = 0x90;
  
  while (!(UCB0IFG&UCTXIFG));                                                  // Wait for TXBUF ready
  UCB0TXBUF = inst;                                                            // Send register address
  
 
  for(i=0; i<TDC720x_ALL_DATA_SIZE; i++)
  {
    while (!(UCB0IFG&UCTXIFG));                                                // Wait for TXBUF ready
    UCB0TXBUF = 0;                                                             // Dummy write so we can read data
    
   
    while (UCB0STAT & UCBUSY);                                                 // Wait for TX complete  
    *(buffer+i) = UCB0RXBUF;                                                   // Read register data

    if ((dev == TDC720x_TDC2) && (TDC720x_Parallel_Read == 1))
    {
      while (!(UCB1IFG&UCRXIFG));
      *(buffer+i) = UCB1RXBUF;
    }
    
  }
    
  TI_TDC720x_CSB1_PxOUT |= TI_TDC720x_CSB1_PIN;                                // /CS disable
  TI_TDC720x_CSB2_PxOUT |= TI_TDC720x_CSB2_PIN;

  // make sure UCB1 is reset
//  if (TDC720x_Parallel_Read)
  {
    UCB1CTL1  =  UCSWRST; 
    UCB1CTL1 &= ~UCSWRST; 
    TI_TDC720x_CSB2_PxOUT &= ~TI_TDC720x_CSB2_PIN;
    TI_TDC720x_CSB2_PxOUT |= TI_TDC720x_CSB2_PIN;
  }
}
//------------------------------------------------------------------------------
//  uint32_t TI_TDC720x_SPILongReadReg(uint8_t addr, uint8_t dev)
//
//  DESCRIPTION:
//  Reads a single measurement result register at address "addr" and returns the
//  value read. Note addr is 8-bit  
//  Return data is 32-bit, MSB is read first, then middle, finally LSB
//------------------------------------------------------------------------------
uint32_t TI_TDC720x_SPILongReadReg(uint8_t addr, uint8_t dev)
{
  uint8_t inst;
  uint32_t x, y;
  
  x = y = 0;

  if (dev == TDC720x_TDC1)
  {
    TI_TDC720x_CSB1_PxOUT &= ~TI_TDC720x_CSB1_PIN;                             // /CS enable 
    TI_TDC720x_CSB2_PxOUT |= TI_TDC720x_CSB2_PIN;  
  } else
  {
    TI_TDC720x_CSB2_PxOUT &= ~TI_TDC720x_CSB2_PIN;
    TI_TDC720x_CSB1_PxOUT |= TI_TDC720x_CSB1_PIN;
  }
  
  // read mode and auto_inc off
  inst = TDC720x_READ_BIT & addr;                                               
  inst = TDC720x_AUTOINC_OFF_BIT & inst;
  
  while (!(UCB0IFG&UCTXIFG));                                                  // Wait for TXBUF ready
  UCB0TXBUF = inst;                                                            // Send address
    
  while (!(UCB0IFG&UCTXIFG));                                                  // Wait for TXBUF ready
  UCB0TXBUF = 0;                                                               // Dummy write so we can read  
    
  while (UCB0STAT & UCBUSY);                                                   // Wait for TX complete  
  y = UCB0RXBUF;                                                               // Read MSB of data
  if ((dev == TDC720x_TDC2) && (TDC720x_Parallel_Read == 1))
  {
    while (!(UCB1IFG&UCRXIFG));
    y = UCB1RXBUF;
  }  
  x |= (y << 16);
  
  while (!(UCB0IFG&UCTXIFG));                                                  // Wait for TXBUF ready
  UCB0TXBUF = 0;                                                               // Dummy write so we can read  
    
  while (UCB0STAT & UCBUSY);                                                   // Wait for TX complete
  y = UCB0RXBUF;
  if ((dev == TDC720x_TDC2) && (TDC720x_Parallel_Read == 1))
  {
   y = UCB1RXBUF;
  }  
  x |= (y << 8);                                                               // Read middle byte of data  

  while (!(UCB0IFG&UCTXIFG));                                                  // Wait for TXBUF ready
  UCB0TXBUF = 0;                                                               // Dummy write so we can read  
    
  while (UCB0STAT & UCBUSY);                                                   // Wait for TX complete
  y = UCB0RXBUF;
  if ((dev == TDC720x_TDC2) && (TDC720x_Parallel_Read == 1))
  {
   y = UCB1RXBUF;
  }  
  x |= y;                                                                      // Read LSB of data  
  
  TI_TDC720x_CSB1_PxOUT |= TI_TDC720x_CSB1_PIN;                                // /CS disable
  TI_TDC720x_CSB2_PxOUT |= TI_TDC720x_CSB2_PIN;
   
  return x;
}
//------------------------------------------------------------------------------
//  void TI_TDC720x_SPIAllReadRegNoAutoInc(uint8_t *buffer, uint8_t dev)
//
//  DESCRIPTION:
//  Special read function for reading all measurement result registers.
//  Data read are deposited sequentially starting at address "buffer" 
//------------------------------------------------------------------------------
void TI_TDC720x_SPIAllReadRegNoAutoInc(uint8_t *buffer, uint8_t dev)
{

  uint8_t i, inst;
  
  
 
  // read mode and auto_inc off
  inst = TDC720x_READ_BIT & 0x10;                                               
  inst = TDC720x_AUTOINC_OFF_BIT & inst;

  for(i=0; i<TDC720x_ALL_DATA_SIZE; i=i+3)
  {
    if (dev == TDC720x_TDC1)
    {
      TI_TDC720x_CSB1_PxOUT &= ~TI_TDC720x_CSB1_PIN;                             // /CS enable 
      TI_TDC720x_CSB2_PxOUT |= TI_TDC720x_CSB2_PIN;  
    } else
    {
      TI_TDC720x_CSB2_PxOUT &= ~TI_TDC720x_CSB2_PIN;
      TI_TDC720x_CSB1_PxOUT |= TI_TDC720x_CSB1_PIN;
    }
  
    while (!(UCB0IFG&UCTXIFG));                                                  // Wait for TXBUF ready
    UCB0TXBUF = inst;                                                            // Send address
    inst++;                                                                      // increment for next read
    
    while (!(UCB0IFG&UCTXIFG));                                                  // Wait for TXBUF ready
    UCB0TXBUF = 0;                                                               // Dummy write so we can read  
    
    while (UCB0STAT & UCBUSY);                                                   // Wait for TX complete  
    *(buffer+i) = UCB0RXBUF;                                                     // Read MSB of data
    if ((dev == TDC720x_TDC2) && (TDC720x_Parallel_Read == 1))
    {
      while (!(UCB1IFG&UCRXIFG));
      *(buffer+i) = UCB1RXBUF;
    }
    
    while (!(UCB0IFG&UCTXIFG));                                                  // Wait for TXBUF ready
    UCB0TXBUF = 0;                                                               // Dummy write so we can read  
    
    while (UCB0STAT & UCBUSY);                                                   // Wait for TX complete
    *(buffer+i+1) = UCB0RXBUF;                                                   // Read middle byte of data  
    if ((dev == TDC720x_TDC2) && (TDC720x_Parallel_Read == 1))
    {
      while (!(UCB1IFG&UCRXIFG));
      *(buffer+i+1) = UCB1RXBUF;
    }
    
    while (!(UCB0IFG&UCTXIFG));                                                  // Wait for TXBUF ready
    UCB0TXBUF = 0;                                                               // Dummy write so we can read  
    
    while (UCB0STAT & UCBUSY);                                                   // Wait for TX complete
    *(buffer+i+2) = UCB0RXBUF;                                                   // Read LSB of data
    if ((dev == TDC720x_TDC2) && (TDC720x_Parallel_Read == 1))
    {
      while (!(UCB1IFG&UCRXIFG));
      *(buffer+i+2) = UCB1RXBUF;
    }
    
    TI_TDC720x_CSB1_PxOUT |= TI_TDC720x_CSB1_PIN;                                // /CS disable
    TI_TDC720x_CSB2_PxOUT |= TI_TDC720x_CSB2_PIN;
  }
}
//------------------------------------------------------------------------------
//  void TI_TDC720x_SPIAllReadReg_BothTDCs(uint8_t *buffer1, uint8_t *buffer2)
//
//  DESCRIPTION:
//  Special read function for reading all measurement result registers.
//  Data read are deposited sequentially starting at address "buffer" 
//------------------------------------------------------------------------------
void TI_TDC720x_SPIAllReadReg_BothTDCs(uint8_t *buffer1, uint8_t *buffer2)
{

  uint8_t i, inst;
    
  TI_TDC720x_CSB1_PxOUT &= ~TI_TDC720x_CSB1_PIN;                               // /CS enable 
  TI_TDC720x_CSB2_PxOUT &= ~TI_TDC720x_CSB2_PIN;
 
  // read mode and auto_inc on
  inst = 0x90;
  
  while (!(UCB0IFG&UCTXIFG));                                                  // Wait for TXBUF ready
  UCB0TXBUF = inst;                                                            // Send register address
  
 
  for(i=0; i<TDC720x_ALL_DATA_SIZE; i++)
  {
    while (!(UCB0IFG&UCTXIFG));                                                // Wait for TXBUF ready
    UCB0TXBUF = 0;                                                             // Dummy write so we can read data
    
   
    while (UCB0STAT & UCBUSY);                                                 // Wait for TX complete  
    *(buffer1+i) = UCB0RXBUF;                                                  // Read register data
    while (!(UCB1IFG&UCRXIFG));
    *(buffer2+i) = UCB1RXBUF;                                                  // Read register data    
  }
    

  TI_TDC720x_CSB1_PxOUT |= TI_TDC720x_CSB1_PIN;                                // /CS disable
  TI_TDC720x_CSB2_PxOUT |= TI_TDC720x_CSB2_PIN;
    
}
//------------------------------------------------------------------------------
#endif

