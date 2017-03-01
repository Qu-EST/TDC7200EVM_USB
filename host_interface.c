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
#include "host_interface.h"
#include "timer_queue.h"
#include "uart_commn.h"

//******************************************************************************
extern uint8_t single_shot_measure_state;
extern uint8_t tof_graph_state;
extern uint8_t graph_delay_sweep_state;
extern uint8_t double_resolution;
extern uint16_t timer_trigger_freq;
extern uint8_t TDC720x_Measure_LT12ns;
extern uint8_t Measure_LT12ns_Method;
extern uint8_t current_tdc;
extern uint8_t TDC720x_Graph_Select;
extern uint8_t TDC720x_Parallel_Read;
extern float tdc_clk_period;
extern uint8_t USB_disable(void);
extern void TI_TDC720x_reg_init(uint8_t);
extern uint8_t outString[];
extern uint8_t TDC720xEVM_Device;
//******************************************************************************
void handle_reInit(uint8_t *, uint16_t);
uint8_t char2nibble(uint8_t); 
//******************************************************************************
// UART Stream control: disable by default
uint8_t TDC720x_UART_Stream = 1; /// LAC- always stream to UART1
uint8_t save_mode;
uint8_t all_data[TDC720x_ALL_DATA_SIZE];    
uint8_t TDC720x_address; 
uint16_t save_default_timer_period;
uint8_t save_timer_trig_freq;
uint8_t save1_tdc7200_mode;
uint8_t save2_tdc7200_mode;
//******************************************************************************
// Parse Command, then execute
uint8_t handleHostCommand(uint8_t *pBuf, uint16_t pSize)
{
  uint8_t host_response = 0;
  uint16_t word_data;
  uint8_t nxt_byte, byte_data;
  uint32_t long_data;
  
  nxt_byte = char2nibble(pBuf[0]);
  nxt_byte = (nxt_byte << 4) + char2nibble(pBuf[1]);
  //nxt_byte = Command_Set_SPI_DOUTx;
  switch(nxt_byte)
  {
    case Command_LoopPacket:  
    {
      host_response = 1;
      break;
    }
    case Command_ReInit: 
    {
      handle_reInit(pBuf, pSize);
      host_response = 1;
      break;
    }    
    case Command_TDC1000_SPI_Byte_Write:
    {
      host_response = 1;
      break;
    }
    case Command_TDC1000_SPI_Byte_Read:
    {
      host_response = 1;
      break;
    }
    case Command_Start_TOF_One_Shot:
    {
      single_shot_measure_state = 1;
      host_response = 1;
      break;
    }
    case Command_Start_TOF_Graph:
    {
      tof_graph_state = 1;
      host_response = 1;
      break;
    }
    case Command_End_TOF_Graph:
    {
      tof_graph_state = 0;
      TI_TDC720x_GLED_PxOUT |= TI_TDC720x_GLED_PIN;                            // Turn on LED
      host_response = 1;
      break;
    }
  case Command_Firmware_Version_Read:
    {
      pBuf[8] = Firmware_VersionA;
      pBuf[9] = Firmware_VersionB;
      pBuf[10] = Firmware_VersionC;
      pBuf[11] = Firmware_VersionD;
      host_response = 1;
      break;
    }
  case Command_LED_Toggle:
    {
      // toggle LED
      TI_TDC720x_GLED_PxOUT ^= TI_TDC720x_GLED_PIN;
      host_response = 1;
      break;
    }
  case Command_MSP430SPI_Config_Read:
    {
      if ((TDC720x_Parallel_Read == 1) && (current_tdc == TDC720x_TDC2))
      {
        pBuf[8] = UCB1CTL0 & 0xC0;
        pBuf[9] = UCB1BR0;
        pBuf[10] = UCB1BR1;        
      } else
      {
        pBuf[8] = UCB0CTL0 & 0xC0;
        pBuf[9] = UCB0BR0;
        pBuf[10] = UCB0BR1;
      }
      host_response = 1;
      break;
    }
  case Command_MSP430SPI_Config_Write:
    {
      byte_data = char2nibble(pBuf[2]);
      byte_data = (byte_data <<4) + char2nibble(pBuf[3]); 
      byte_data &= 0xC0;                                                       // make sure only top 2 bits are valid
      // low byte first
      word_data = char2nibble(pBuf[6]);
      word_data = (word_data <<4) + char2nibble(pBuf[7]);
      // high byte next
      word_data = (word_data <<4) + char2nibble(pBuf[4]);
      word_data = (word_data <<4) + char2nibble(pBuf[5]);      
      if (word_data < 1)
        word_data = 1;
      if ((TDC720x_Parallel_Read == 1) && (current_tdc == TDC720x_TDC2))
      {
        UCB1CTL1 |= UCSWRST;                                                     // **Disable USCI state machine**
        UCB1CTL0 &= 0x3F;                                                        // update 
        UCB1CTL0 |= byte_data;
        UCB1BRW = word_data;                                                     // UCLK divider;
        UCB1CTL1 &= ~UCSWRST;                                                    // **Initialize USCI state machine**        
      } else
      {
        UCB0CTL1 |= UCSWRST;                                                     // **Disable USCI state machine**
        UCB0CTL0 &= 0x3F;                                                        // update 
        UCB0CTL0 |= byte_data;
        UCB0BRW = word_data;                                                     // UCLK divider;
        UCB0CTL1 &= ~UCSWRST;                                                    // **Initialize USCI state machine**
      }
      host_response = 1;
      break;
    }
    case Command_TDC720x_SPI_Byte_Write:
    {                              
      TDC720x_address = char2nibble(pBuf[2]);
      TDC720x_address = (TDC720x_address << 4) + char2nibble(pBuf[3]);
      
      byte_data = char2nibble(pBuf[4]);
      byte_data = (byte_data <<4) + char2nibble(pBuf[5]);
      
      TI_TDC720x_SPIByteWriteReg(TDC720x_address, byte_data, current_tdc);
      
      host_response = 1;
      break;
    }
    case Command_TDC720x_SPI_Byte_Read:
    {
      TDC720x_address = char2nibble(pBuf[2]);
      TDC720x_address = (TDC720x_address << 4) + char2nibble(pBuf[3]);

      byte_data = TI_TDC720x_SPIByteReadReg(TDC720x_address, current_tdc);
      pBuf[8] = byte_data;
      host_response = 1;
      break;
    }
    case Command_TDC720x_SPI_Word_Read:
    {
      TDC720x_address = char2nibble(pBuf[2]);
      TDC720x_address = (TDC720x_address << 4) + char2nibble(pBuf[3]);

      long_data = TI_TDC720x_SPILongReadReg(TDC720x_address, current_tdc);
      pBuf[8] = long_data;
      pBuf[9] = long_data >> 8;
      pBuf[10] = long_data >> 16;      
      host_response = 1;
      break;
    }
   case Command_Set_Timer_Trigger_Freq:
    {                              
      // low byte first
      word_data = char2nibble(pBuf[4]);
      word_data = (word_data <<4) + char2nibble(pBuf[5]);
      // high byte next
      word_data = (word_data <<4) + char2nibble(pBuf[2]);
      word_data = (word_data <<4) + char2nibble(pBuf[3]); 

      timer_trigger_freq = word_data;
      
      host_response = 1;
      break;
    }
   case Command_Read_Timer_Trigger_Freq:
    {
      pBuf[8] = timer_trigger_freq;
      pBuf[9] = timer_trigger_freq >> 8;
      host_response = 1;
      break;
    }
  case Command_Enter_MSP430_BSL:
    {
      USB_disable();
      __disable_interrupt();
      ((void (*)())0x1000)();
      host_response = 1;
      break;
    }
  case Command_Reset_TDC720x:
    {
      // disable devices
      TI_TDC720x_ENABLE1_PxOUT &= ~TI_TDC720x_ENABLE1_PIN;                 
      TI_TDC720x_ENABLE2_PxOUT &= ~TI_TDC720x_ENABLE2_PIN;
      // wait for 1sec for the cap to discharge
      __delay_cycles(24000000);
      // enable devices 
      TI_TDC720x_ENABLE1_PxOUT |= TI_TDC720x_ENABLE1_PIN;                      // Enable device
      TI_TDC720x_ENABLE2_PxOUT |= TI_TDC720x_ENABLE2_PIN;
      // give atleast 500us
      __delay_cycles(1200000);
      TI_TDC720x_reg_init(current_tdc);
      host_response = 1;
      break;
    }
  case Command_TDC720x_Set_Double_Resolution:
    {
      uint8_t reg_address;
      reg_address = 0x0b;
      
      all_data[0] = 0;
      all_data[1] = 0x01;
      // Enable device
      //TI_TDC720x_ENABLE_PxOUT |= TI_TDC720x_ENABLE_PIN;                            
      
      // wait for TDC720x wakeup delay
      //timer_microsec_delay(TDC720x_WAKEUP_PERIOD);
      
      TI_TDC720x_SPIAutoIncWriteReg(reg_address, all_data, 2, current_tdc);
      
      // Disable device
      //TI_TDC720x_ENABLE_PxOUT &= ~TI_TDC720x_ENABLE_PIN;      
      double_resolution = 1;
      host_response = 1;
      break;
    }
  case Command_TDC720x_Clear_Double_Resolution:
    {
      uint8_t reg_address;
      reg_address = 0x0b;
      
      all_data[0] = 0;
      all_data[1] = 0;

      // Enable device
      //TI_TDC720x_ENABLE_PxOUT |= TI_TDC720x_ENABLE_PIN;                            
      
      // wait for TDC720x wakeup delay
      //timer_microsec_delay(TDC720x_WAKEUP_PERIOD);
      
      TI_TDC720x_SPIAutoIncWriteReg(reg_address, all_data, 2, current_tdc);
      
      // Disable device
      //TI_TDC720x_ENABLE_PxOUT &= ~TI_TDC720x_ENABLE_PIN;      
      double_resolution = 0;
      host_response = 1;
      break;
    }
  case Command_TDC720x_SPIAutoIncWrite:
    {      
      uint8_t i, j, count, reg_address;
      uint8_t wBuf[4];

      reg_address = char2nibble(pBuf[2]);
      reg_address = (reg_address << 4) + char2nibble(pBuf[3]);
      // Note count below is 1 less than num of bytes to write
      count = char2nibble(pBuf[4]);
      count = (count << 4) + char2nibble(pBuf[5]);
      
      for(i=6, j=0; j<=count; i=i+2, j++)
      {
        wBuf[j] = char2nibble(pBuf[i]);
        wBuf[j] = (wBuf[j] << 4) + char2nibble(pBuf[i+1]);
      }
      TI_TDC720x_SPIAutoIncWriteReg(reg_address, wBuf, count+1, current_tdc);
      host_response = 1;
      break;
    }
  case Command_TDC720x_SPIAutoIncRead:
    {      
      uint8_t i, count, reg_address;
      uint8_t wBuf[4];

      reg_address = char2nibble(pBuf[2]);
      reg_address = (reg_address << 4) + char2nibble(pBuf[3]);
      // Note count below is 1 less than num of bytes to read
      count = char2nibble(pBuf[4]);
      count = (count << 4) + char2nibble(pBuf[5]);
      // According to Angus Auto inc write of Test Registers doesn't work
      TI_TDC720x_SPIAutoIncReadReg(reg_address, wBuf, count+1, current_tdc);
      for(i=0; i<=count; i++)
        pBuf[8+i] = wBuf[i];
   
      host_response = 1;
      break;
    }
    case Command_Start_Graph_Delay_Sweep:
    {
      // Put Device in Mode 1
      byte_data = save_mode = TI_TDC720x_SPIByteReadReg(TI_TDC720x_CONFIG1_REG, current_tdc);
      byte_data &= 0xF8;
      TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CONFIG1_REG, byte_data, current_tdc);
      graph_delay_sweep_state = 1;
      host_response = 1;
      break;
    }
    case Command_Stop_Graph_Delay_Sweep:
    {
      uint8_t reg_address;
      uint8_t wBuf[2];
      
      if (graph_delay_sweep_state)
      {
        // Put back device in saved mode
        byte_data = save_mode;
        TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CONFIG1_REG, save_mode, current_tdc); 
      
        // clear delay sweep mode
        reg_address = 0x0B;
        wBuf[0] = 0x0;
        if (double_resolution)
          wBuf[1] = 0x01;
        else
          wBuf[1] = 0x00;
        TI_TDC720x_SPIAutoIncWriteReg(reg_address, wBuf, 2, current_tdc);  
      
        graph_delay_sweep_state = 0;
        TI_TDC720x_GLED_PxOUT |= TI_TDC720x_GLED_PIN;                            // Turn on LED
      }      
      host_response = 1;
      break;
    }
  case Command_Set_Fast_Trig:
    {
      // sparky wants delay sweep speeded up:save state
      save_timer_trig_freq = timer_trigger_freq;
      save_default_timer_period = TA1CCR0;
      // init timer TA1 period to 10ms (instead of usual 100ms)
      timer_trigger_freq = 0;
      TA1CCR0 = INTERVAL_10ms; 
      host_response = 1;
      break;
    }
  case Command_Clear_Fast_Trig:
    {  
      // sparky wants delay sweep speeded up: restore state
      timer_trigger_freq = save_timer_trig_freq;
      TA1CCR0 = save_default_timer_period;
      host_response = 1;
      break;      
    }
   case Command_Read_Tdc_Clk_Period:
     {
       float tc1, tc2, tc3, tc4;
       // calculate clock freq in MHz
       tc1 = 1000.0 / tdc_clk_period;
       pBuf[8] = (uint8_t) tc1;
       tc2 = (tc1 - pBuf[8]) * 100;
       pBuf[9] = (uint8_t) tc2;
       tc3 = (tc2 - pBuf[9]) * 100;
       pBuf[10] = (uint8_t) tc3;
       tc4 = (tc3 - pBuf[10]) * 100;
       pBuf[11] = (uint8_t) tc4;       
       host_response = 1;
       break;
     }
   case Command_Set_Tdc_Clk_Period:
    {
      float tmp_clk;
      // byte data (up to 2 digits max) gives integer MHz
      byte_data = char2nibble(pBuf[2]);
      byte_data = (byte_data <<4) + char2nibble(pBuf[3]);
      
      tmp_clk = (float) byte_data;

      // byte data (up to 4 digits: first 2 digits) gives fractional MHz
      byte_data = char2nibble(pBuf[8]);
      byte_data = (byte_data <<4) + char2nibble(pBuf[9]);
      
      tmp_clk += (((float) byte_data)/100.0);
      
      // byte data (up to 4 digits: next 2 digits) gives fractional MHz
      byte_data = char2nibble(pBuf[6]);
      byte_data = (byte_data <<4) + char2nibble(pBuf[7]);
      
      tmp_clk += (((float) byte_data)/10000.0);
      
      // byte data (up to 4 digits: last 2 digits) gives fractional MHz
      byte_data = char2nibble(pBuf[4]);
      byte_data = (byte_data <<4) + char2nibble(pBuf[5]);      
      
      tmp_clk += (((float) byte_data)/1000000.0);
      
      // calculate in ns
      tdc_clk_period = 1000.0 / tmp_clk;
  
      host_response = 1;
      break;
    }
  case Command_Measure_LT12ns_Enable:
    {
      byte_data = char2nibble(pBuf[2]);
      byte_data = (byte_data <<4) + char2nibble(pBuf[3]); 
      if (byte_data)
      {
        Measure_LT12ns_Method = TDC720x_TWO_DEVICE_METHOD;
        // make sure both devices are in mode1
        // read and save current config1 for device1, then update to mode1
        byte_data = TI_TDC720x_SPIByteReadReg(TI_TDC720x_CONFIG1_REG, TDC720x_TDC1);
        save1_tdc7200_mode = byte_data & MODE_MASK;     
        byte_data &= ~MODE_MASK;
        TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CONFIG1_REG, byte_data, TDC720x_TDC1);
        // read and save current config1 for device2, then update to mode1
        byte_data = TI_TDC720x_SPIByteReadReg(TI_TDC720x_CONFIG1_REG, TDC720x_TDC2);
        save2_tdc7200_mode = byte_data & MODE_MASK;     
        byte_data &= ~MODE_MASK;
        TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CONFIG1_REG, byte_data, TDC720x_TDC2);        
      } else
      {
        Measure_LT12ns_Method = TDC720x_MUX_METHOD;
        byte_data = TI_TDC720x_SPIByteReadReg(TI_TDC720x_CONFIG1_REG, current_tdc);
        save1_tdc7200_mode = byte_data & MODE_MASK;     
        // make sure device is in mode 1
        byte_data &= ~MODE_MASK;
        TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CONFIG1_REG, byte_data, current_tdc);        
      }
       
      TDC720x_Measure_LT12ns = 1;
      // enable REF_START and DTG_TRIG outputs
//      TI_TDC720x_REF_START_PxSEL |= TI_TDC720x_REF_START_PIN;                   
//      TI_TDC720x_DTG_TRIG_PxSEL |= TI_TDC720x_DTG_TRIG_PIN;  
      
      host_response = 1;
      break;      
    }
  case Command_Measure_LT12ns_Disable:
    {
      TDC720x_Measure_LT12ns = 0;
      byte_data = char2nibble(pBuf[2]);
      byte_data = (byte_data <<4) + char2nibble(pBuf[3]); 
      if (byte_data)
      {  
        // restore Mode for device1
        byte_data = (TI_TDC720x_SPIByteReadReg(TI_TDC720x_CONFIG1_REG, TDC720x_TDC1) & (~MODE_MASK)) | save1_tdc7200_mode;
        TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CONFIG1_REG, byte_data, TDC720x_TDC1);  
        // restore Mode for device2
        byte_data = (TI_TDC720x_SPIByteReadReg(TI_TDC720x_CONFIG1_REG, TDC720x_TDC2) & (~MODE_MASK)) | save2_tdc7200_mode;
        TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CONFIG1_REG, byte_data, TDC720x_TDC2);         
      } else
      {
        // restore Mode
        byte_data = (TI_TDC720x_SPIByteReadReg(TI_TDC720x_CONFIG1_REG, current_tdc) & (~MODE_MASK)) | save1_tdc7200_mode;
        TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CONFIG1_REG, byte_data, current_tdc);
      }
      // disable REF_START and DTG_TRIG outputs
      TI_TDC720x_REF_START_PxSEL &= ~TI_TDC720x_REF_START_PIN;                       
      TI_TDC720x_DTG_TRIG_PxSEL &= ~TI_TDC720x_DTG_TRIG_PIN;                          
      TI_TDC720x_MUX1_CTRL_PxOUT &= ~TI_TDC720x_MUX1_CTRL_PIN;                     // Set MUX1 low
      TI_TDC720x_MUX2_CTRL_PxOUT &= ~TI_TDC720x_MUX2_CTRL_PIN;                     // Set MUX2 low
      
      host_response = 1;
      break;      
    }
   case Command_Enable_UART_Stream:
    {      
      // enable UART Stream
      TDC720x_UART_Stream = 1; 
      // Init UART
      InitUART();
      sprintf((char *)outString, "Hello World \r\n");
      putsUART((unsigned char *)outString,strlen((char *)outString));      

      host_response = 1;
      break;
    }
   case Command_Disable_UART_Stream:
    {
      // disable UART Stream
      TDC720x_UART_Stream = 0;

      host_response = 1;
      break;
    }
   case Command_Set_Current_TDC_For_Access:
    {
      byte_data = char2nibble(pBuf[2]);
      byte_data = (byte_data <<4) + char2nibble(pBuf[3]); 
      if (byte_data)
        current_tdc = TDC720x_TDC2;
      else
        current_tdc = TDC720x_TDC1;

      host_response = 1;
      break;
    }
  case Command_Read_Current_TDC_For_Access:
    {
      pBuf[8] = current_tdc;
      host_response = 1;
      break;
    }
   case Command_Set_TDC_Graph_Select:
    {
      byte_data = char2nibble(pBuf[2]);
      byte_data = (byte_data <<4) + char2nibble(pBuf[3]);       
      
      if (byte_data == 0)
      {        
        if (current_tdc == TDC720x_TDC1)
          TDC720x_Graph_Select = TDC720x_TDC1;
        else
          TDC720x_Graph_Select = TDC720x_TDC2;
      }
      else if (byte_data == 1)
      {               
        TDC720x_Graph_Select = TDC720x_BOTH;
      }

      host_response = 1;
      break;
    }
   case Command_Set_SPI_DOUTx:
    {
      byte_data = char2nibble(pBuf[2]);
      byte_data = (byte_data <<4) + char2nibble(pBuf[3]); 
      
      if (byte_data)
      {      
        // enable parallel read of DOUT0 and DOUT1
        TDC720x_Parallel_Read = 1;      
      } else
      {
        // disable parallel read of DOUT0 and DOUT1
        TDC720x_Parallel_Read = 0;         
      }

      host_response = 1;
      break;
    }
   case Command_Read_SPI_DOUTx:
    {
      pBuf[8] = TDC720x_Parallel_Read;
      host_response = 1;
      break;
    }
   case Command_Set_TDC720xEVM_Device:
    {
      byte_data = char2nibble(pBuf[2]);
      byte_data = (byte_data <<4) + char2nibble(pBuf[3]); 
      if (byte_data)
      {
        TDC720xEVM_Device = TDC7201;
        TI_TDC720x_reg_init(TDC720x_TDC1);
        TI_TDC720x_reg_init(TDC720x_TDC2);
        current_tdc = TDC720x_TDC1;
        TDC720x_Parallel_Read = 0;
      } else
      {
        TDC720xEVM_Device = TDC7200;
        TI_TDC720x_reg_init(TDC720x_TDC1);
        current_tdc = TDC720x_TDC1;
        TDC720x_Parallel_Read = 0;
      }

      host_response = 1;
      break;
    }
  case Command_Read_TDC720xEVM_Device:
    {
      pBuf[8] = TDC720xEVM_Device;
      host_response = 1;
      break;
    }    
  default:
    {
      host_response = 1;
      break;
    }    
  }
  return (host_response);
}
//******************************************************************************
void handle_reInit(uint8_t *pBuf, uint16_t pSize)
{
}
//******************************************************************************      
uint8_t char2nibble(uint8_t db)
{
  if ((db >= '0') && (db <= '9'))
    return (db-'0');
  else if ((db >= 'A') && (db <= 'F'))
    return (db-'A'+0xa);
  else if ((db >= 'a') && (db <= 'f'))
    return (db-'a'+0xa);
  else
    return (db);
}
//******************************************************************************
