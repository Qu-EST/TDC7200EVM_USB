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
#include <cstring>
#include <stdio.h>
#include "TI_TDC720x.h"  
#include "TI_MSP430.h"
#include "TI_MSP430_hardware_board.h"
#include "TI_MSP430_spi.h"
#include "timer_queue.h"
#include "tdc720x_mode1_lt12ns.h"

//******************************************************************************
 
extern uint8_t bufferX[], bufferY[];
extern uint8_t *mbuf, *ubuf;
extern volatile uint8_t TDC720x_XYBufferReady;

extern uint8_t bufferP[], bufferQ[];
extern uint8_t *nbuf, *vbuf;
extern volatile uint8_t TDC720x_PQBufferReady;

extern uint8_t TDC720x_Measure_LT12ns;
extern uint8_t Measure_LT12ns_Method;


//******************************************************************************
void TDC720x_mode1_lt12ns(void)
{
  if (Measure_LT12ns_Method == TDC720x_TWO_DEVICE_METHOD)
  {
    handle_two_TDC720x();
  } else if (Measure_LT12ns_Method == TDC720x_MUX_METHOD)
  {    
    handle_TDC720x_plus_mux(); 
  }      
}
//******************************************************************************
void handle_two_TDC720x(void)
{
  uint8_t byte_data1, byte_data2;
  
  // read config1 register of TDC1 and TDC2
  byte_data1 = TI_TDC720x_SPIByteReadReg(TI_TDC720x_CONFIG1_REG, TDC720x_TDC1);
  byte_data2 = TI_TDC720x_SPIByteReadReg(TI_TDC720x_CONFIG1_REG, TDC720x_TDC2); 
    
  // set start measurement bit for both TDC720x devices
  byte_data1 |= 0x01;
  TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CONFIG1_REG, byte_data1, TDC720x_TDC1); 
  byte_data2 |= 0x01;
  TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CONFIG1_REG, byte_data2, TDC720x_TDC2);

  // send REF_START: could be used as DTG_TRIG also
  TI_TDC720x_REF_START_PxOUT |= TI_TDC720x_REF_START_PIN;
  TI_TDC720x_REF_START_PxOUT &= ~TI_TDC720x_REF_START_PIN;    
  TI_TDC720x_DTG_TRIG_PxOUT |= TI_TDC720x_DTG_TRIG_PIN;
  TI_TDC720x_DTG_TRIG_PxOUT &= ~TI_TDC720x_DTG_TRIG_PIN;    
    
  //wait for INTB1 pin to go low
  wait_on_intb_pin(TI_TDC720x_INTB1_PIN);      
  TI_TDC720x_SPIAllReadReg(mbuf, TDC720x_TDC1); 

  // switch buffer and set flag for usb to send the filled buffer
  ubuf = mbuf;
  if (mbuf == bufferX)
    mbuf = bufferY;
  else
    mbuf = bufferX; 
  TDC720x_XYBufferReady = 1;
    
  //wait for INTB2 pin to go low
  wait_on_intb_pin(TI_TDC720x_INTB2_PIN);
  TI_TDC720x_SPIAllReadReg(nbuf, TDC720x_TDC2);    
    
  // switch buffer and set flag for usb to send the filled buffer
  vbuf = nbuf;
  if (nbuf == bufferP)
    nbuf = bufferQ;
  else
    nbuf = bufferP; 
  TDC720x_PQBufferReady = 1;
  
}
//******************************************************************************
void handle_TDC720x_plus_mux(void)
{
  uint8_t byte_data1;

  // read config1 register of TDC1
  byte_data1 = TI_TDC720x_SPIByteReadReg(TI_TDC720x_CONFIG1_REG, TDC720x_TDC1);
  
  // set the mux to measure REF_START to START
  TI_TDC720x_MUX1_CTRL_PxOUT |= TI_TDC720x_MUX1_CTRL_PIN;
  TI_TDC720x_MUX2_CTRL_PxOUT |= TI_TDC720x_MUX2_CTRL_PIN;
  
  // set start measurement bit & use default mode or set by user
  byte_data1 |= 0x01;
  TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CONFIG1_REG, byte_data1, TDC720x_TDC1);
    
  // send REF_START: could be used as DTG_TRIG also
  TI_TDC720x_REF_START_PxOUT |= TI_TDC720x_REF_START_PIN;
  TI_TDC720x_REF_START_PxOUT &= ~TI_TDC720x_REF_START_PIN;    
  TI_TDC720x_DTG_TRIG_PxOUT |= TI_TDC720x_DTG_TRIG_PIN;
  TI_TDC720x_DTG_TRIG_PxOUT &= ~TI_TDC720x_DTG_TRIG_PIN;    
    
  //wait for INTB1 pin to go low
  wait_on_intb_pin(TI_TDC720x_INTB1_PIN);      
  TI_TDC720x_SPIAllReadReg(mbuf, TDC720x_TDC1); 

  // switch buffer and set flag for usb to send the filled buffer
  ubuf = mbuf;
  if (mbuf == bufferX)
    mbuf = bufferY;
  else
    mbuf = bufferX;
  TDC720x_XYBufferReady = 1;    
    
  // below is the second measurement to measure REF_START to STOP
  // set the mux to measure REF_START to STOP
  TI_TDC720x_MUX1_CTRL_PxOUT |= TI_TDC720x_MUX1_CTRL_PIN;
  TI_TDC720x_MUX2_CTRL_PxOUT &= ~TI_TDC720x_MUX2_CTRL_PIN;
      
  // set start measurement bit & use default mode or set by user
  byte_data1 |= 0x01;
  TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CONFIG1_REG, byte_data1, TDC720x_TDC1);      
       
  // send REF_START: could be used as DTG_TRIG also
  TI_TDC720x_REF_START_PxOUT |= TI_TDC720x_REF_START_PIN;
  TI_TDC720x_REF_START_PxOUT &= ~TI_TDC720x_REF_START_PIN;    
  TI_TDC720x_DTG_TRIG_PxOUT |= TI_TDC720x_DTG_TRIG_PIN;
  TI_TDC720x_DTG_TRIG_PxOUT &= ~TI_TDC720x_DTG_TRIG_PIN;       
        
  //wait for INTB1 pin to go low
  wait_on_intb_pin(TI_TDC720x_INTB1_PIN);
  TI_TDC720x_SPIAllReadReg(nbuf, TDC720x_TDC1);

  // switch buffer and set flag for usb to send the filled buffer
  vbuf = nbuf;
  if (nbuf == bufferP)
    nbuf = bufferQ;
  else
    nbuf = bufferP; 
  TDC720x_PQBufferReady = 1; 
}
//******************************************************************************
