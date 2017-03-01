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
extern uint8_t TDC720x_reg_local_copy[];
// default onboard clk in ns
float tdc_clk_period = 125.0;
float start2stop[MAX_STOPS];
uint8_t outString[128];
int32_t meas_result_regrs[MEAS_RESULT_REG_NUM];
float norm_lsb;
float ccnt;
//------------------------------------------------------------------------------
void TDC720x_calc(uint8_t *buf, uint8_t dev)
{
  TDC720x_build_mreg(buf);
  TDC720x_start2stopn(dev);
  print_start2stopn();

}
//******************************************************************************
void print_start2stopn(void)
{
  uint8_t i;
#if 0  
  float tmp;
  for(i=0; i < MEAS_RESULT_REG_NUM; i++)
  {
    tmp = (float) meas_result_regrs[i];
    sprintf((char *)outString, "mreg[%d]: %4.6f   ", i, tmp);    
    putsUART((unsigned char *)outString,strlen((char *)outString)); 
    if (((i+1) % 3) == 0) 
    {
      sprintf((char *)outString, " \r\n");
      putsUART((unsigned char *)outString,strlen((char *)outString));      
    }
      
  }
  sprintf((char *)outString, " \r\n");
  putsUART((unsigned char *)outString,strlen((char *)outString));  
  sprintf((char *)outString, "norm_lsb: %4.6f  ", norm_lsb);
  putsUART((unsigned char *)outString,strlen((char *)outString)); 
  sprintf((char *)outString, "cal_count: %4.6f \r\n", ccnt);
  putsUART((unsigned char *)outString,strlen((char *)outString));  
#endif
  for (i=0;i<MAX_STOPS;i++)
  {
    //sprintf((char *)outString, "Start_to_Stop[%d]: %4.6f    ", i+1, start2stop[i]);
    sprintf((char *)outString, "TOF: %4.6f    ", start2stop[i]); // only print out one stop - Lac
    putsUART((unsigned char *)outString,strlen((char *)outString));


  }
  sprintf((char *)outString, " \r\n");
  putsUART((unsigned char *)outString,strlen((char *)outString));  
}
//******************************************************************************
void TDC720x_start2stopn(uint8_t dev)
{
  uint8_t n;
  

  TDC720x_normlsb(dev);
  
  
  //mreg[0]  = Time1,      mreg[1] = Clk_count1, mreg[2] = Time2
  //mreg[3]  = Clk_count2, mreg[4] = Time3,      mreg[5] = Clk_count3
  //mreg[6]  = Time4,      mreg[7] = Clk_count4, mreg[8] = Time5
  //mreg[9]  = Clk_count5, mreg[10] = Time6,     mreg[11] = Calibration1
  //mreg[12] = Calibration2
  
  //st2sp[0] = nlsb * (Time1 - Time2) + tdc_clk_period * Clk_count1
  //st2sp[1] = nlsb * (Time1 - Time3) + tdc_clk_period * Clk_count2
  //st2sp[2] = nlsb * (Time1 - Time4) + tdc_clk_period * Clk_count3
  //st2sp[3] = nlsb * (Time1 - Time5) + tdc_clk_period * Clk_count4
  //st2sp[4] = nlsb * (Time1 - Time6) + tdc_clk_period * Clk_count5
  
  // do for 5 stops
  for(n=0; n < MAX_STOPS; n++)
    start2stop[n] = norm_lsb * (meas_result_regrs[0] - meas_result_regrs[2*(n+1)]) + 
                    tdc_clk_period * meas_result_regrs[2*(n+1)-1];

}
//******************************************************************************
void TDC720x_normlsb(uint8_t dev)
{
  uint8_t tdcbyte, cal2cy;
  

  // read config2 register - use local saved data
  tdcbyte = (TI_TDC720x_SPIByteReadReg(TI_TDC720x_CONFIG2_REG, dev) & CAL2MASK)>>CAL2SHFT;
  switch (tdcbyte)
  {
    case 0: 
      cal2cy = 2;
      break;
    case 1:
      cal2cy = 10;
      break;
    case 2:
      cal2cy = 20;
      break;
    case 3:
      cal2cy = 40;
    default:
      cal2cy = 10;
  }
  ccnt = (float) (meas_result_regrs[12]-meas_result_regrs[11])/(cal2cy-1);
  norm_lsb = tdc_clk_period / ccnt;  
}
//******************************************************************************
void TDC720x_build_mreg(uint8_t *buf)
{
  uint8_t i;
  //mreg[0]  = Time1,      mreg[1] = Clk_count1, mreg[2] = Time2
  //mreg[3]  = Clk_count2, mreg[4] = Time3,      mreg[5] = Clk_count3
  //mreg[6]  = Time4,      mreg[7] = Clk_count4, mreg[8] = Time5
  //mreg[9]  = Clk_count5, mreg[10] = Time6,     mreg[11] = Calibration1
  //mreg[12] = Calibration2
  for(i=0; i < MEAS_RESULT_REG_NUM; i++)
  {
    meas_result_regrs[i] = (((uint32_t) buf[3*i]) << 16) + 
                            (((uint32_t) buf[(3*i)+1]) << 8) + 
                             (((uint32_t) buf[(3*i)+2]));
  }
}
//******************************************************************************
