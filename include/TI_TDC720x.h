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

#ifndef HEADER_FILE_TI_TDC720x_H

#define HEADER_FILE_TI_TDC720x_H

/************************************************************
* TI TDC720x REGISTER SET ADDRESSES
************************************************************/

#define TI_TDC720x_CONFIG1_REG                         (0x00)                  //  
#define TI_TDC720x_CONFIG2_REG                         (0x01)                  //  
#define TI_TDC720x_INTRPT_STATUS_REG                   (0x02)                  //  
#define TI_TDC720x_INTRPT_MASK_REG                     (0x03)                  //  
#define TI_TDC720x_COARSE_COUNTER_OVH_REG              (0x04)                  //  
#define TI_TDC720x_COARSE_COUNTER_OVL_REG              (0x05)                  //  
#define TI_TDC720x_CLOCK_COUNTER_OVH_REG               (0x06)                  //  
#define TI_TDC720x_CLOCK_COUNTER_OVL_REG               (0x07)                  //  
#define TI_TDC720x_CLOCK_COUNTER_STOP_MASKH_REG        (0x08)                  //  
#define TI_TDC720x_CLOCK_COUNTER_STOP_MASKL_REG        (0x09)                  // 

#define TI_TDC720x_TIME1_REG                           (0x10)                  //  
#define TI_TDC720x_CLOCK_COUNT1_REG                    (0x11)                  //
#define TI_TDC720x_TIME2_REG                           (0x12)                  //  
#define TI_TDC720x_CLOCK_COUNT2_REG                    (0x13)                  // 
#define TI_TDC720x_TIME3_REG                           (0x14)                  //  
#define TI_TDC720x_CLOCK_COUNT3_REG                    (0x15)                  // 
#define TI_TDC720x_TIME4_REG                           (0x16)                  //  
#define TI_TDC720x_CLOCK_COUNT4_REG                    (0x17)                  //
#define TI_TDC720x_TIME5_REG                           (0x18)                  //  
#define TI_TDC720x_CLOCK_COUNT5_REG                    (0x19)                  // 
#define TI_TDC720x_TIME6_REG                           (0x1A)                  //  
#define TI_TDC720x_CALIBRATION1_REG                    (0x1B)                  //
#define TI_TDC720x_CALIBRATION2_REG                    (0x1C)                  // 



// Useful definitions
#define TDC720x_READ_BIT                              (0xBF)                   // 6th of address is 0 for read
#define TDC720x_WRITE_BIT                             (0x40)                   // 6th of address is 1 for write
#define TDC720x_AUTOINC_ON_BIT                        (0x80)                   // 7th of address is 1 for Auto Inc ON
#define TDC720x_AUTOINC_OFF_BIT                       (0x7F)                   // 7th of address is 0 for Auto Inc OFF
#define TDC720x_ALL_DATA_SIZE                         (0x27)                   // 13 measure registers each 3bytes

#define TIMEOUT_VALUE 200000

#define CAL2MASK (0xC0)
#define CAL2SHFT  (0x06)
// Start of change by Jeeva
//adjusting the Max_stop to 1 from 05
// #define MAX_STOPS (0x05)      //
#define MAX_STOPS (0x01)
// end of change by jeeva

#define MEAS_RESULT_REG_NUM (0x0D)
#define MODE_MASK (0x06)

// Adding support TDC7201 with 2 TDCs
#define TDC7200 0
#define TDC7201 1

#define TDC720x_TDC1 0
#define TDC720x_TDC2 1
#define TDC720x_BOTH 2

#endif                                                                         // HEADER_FILE_TI_TDC720x_H

