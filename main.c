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

#include <intrinsics.h>
#include <string.h>
//
#include <stdint.h>
#include <cstring>
#include <stdio.h>

#include "TI_TDC720x.h"
#include "TI_MSP430.h"
#include "TI_MSP430_hardware_board.h"
#include "TI_MSP430_spi.h"
#include "timer_queue.h"
#include "host_interface.h"
#include "tdc720x_calculation.h"
#include "uart_commn.h"

#include "USB_API/USB_Common/device.h"
#include "USB_API/USB_Common/types.h"               //Basic Type declarations
#include "USB_API/USB_Common/usb.h"                 //USB-specific functions

#include "F5xx_F6xx_Core_Lib/HAL_UCS.h"
#include "F5xx_F6xx_Core_Lib/HAL_PMM.h"

#include "USB_API/USB_CDC_API/UsbCdc.h"
#include "usbConstructs.h"
#include "USB_config/descriptors.h"

//******************************************************************************
#define MAX_STR_LENGTH 32
//******************************************************************************
void InitMCU(void);
void Init_Ports (void);
void Init_Clock (void);
void USBCommunicationTask(void);
void TI_TDC720x_reg_init(uint8_t);
extern void timer_microsec_delay(uint16_t);
extern void tdc_trigger_measure(void);

// Globals & Externs
volatile uint8_t TDC720x_XYBufferReady = 0;
volatile uint8_t TDC720x_PQBufferReady = 0;

volatile BYTE bCDCDataReceived_event = FALSE;   //Indicates data has been received without an open rcv operation
WORD w;
uint8_t bufferX[BUF_LENGTH];
uint8_t bufferY[BUF_LENGTH];
uint8_t *mbuf=bufferX;
uint8_t *ubuf;

uint8_t bufferP[BUF_LENGTH];
uint8_t bufferQ[BUF_LENGTH];
uint8_t *nbuf=bufferP;
uint8_t *vbuf;

uint8_t cmdResponseString[MAX_STR_LENGTH] = "";
uint8_t single_shot_measure_state=0;
uint8_t tof_graph_state=0;
uint8_t graph_delay_sweep_state=0;
uint8_t double_resolution=0;
uint8_t current_tdc = TDC720x_TDC1;
uint8_t TDC720xEVM_Device = TDC7200;//change default to TDC7200 instead of TDC7201;
/*----------------------------------------------------------------------------*/
 
uint32_t err_count = 0;
 

extern volatile uint8_t next_trigger_time;
extern uint8_t TDC720x_UART_Stream;
extern uint8_t Measure_LT12ns_Method;

//******************************************************************************

void main(void)
{
  
  WDTCTL = WDTPW+WDTHOLD;                                                      // Stop WDT 
  
  TI_TDC720x_GLED_PxOUT |= TI_TDC720x_GLED_PIN;                                // Set GLED ON
  TI_TDC720x_GLED_PxDIR |= TI_TDC720x_GLED_PIN;                                // Set pin direction is output

  TI_TDC720x_OSCENABLE_PxOUT |= TI_TDC720x_OSCENABLE_PIN;                    // Set pin high: enable osc
  //TI_TDC720x_OSCENABLE_PxOUT &= ~TI_TDC720x_OSCENABLE_PIN;                    // Set pin low: disable osc  
  TI_TDC720x_OSCENABLE_PxDIR |= TI_TDC720x_OSCENABLE_PIN;                    // Set pin direction is output
  
  __delay_cycles(50000);

  TI_TDC720x_ENABLE1_PxOUT |= TI_TDC720x_ENABLE1_PIN;                          // Enable1 device
  TI_TDC720x_ENABLE1_PxDIR |= TI_TDC720x_ENABLE1_PIN;                          // Set pin direction output

  TI_TDC720x_ENABLE2_PxOUT |= TI_TDC720x_ENABLE2_PIN;                          // Enable2 device
  TI_TDC720x_ENABLE2_PxDIR |= TI_TDC720x_ENABLE2_PIN;                          // Set pin direction output

  TI_TDC720x_MUX1_CTRL_PxOUT &= ~TI_TDC720x_MUX1_CTRL_PIN;                     // Set MUX1 low
  TI_TDC720x_MUX1_CTRL_PxDIR |= TI_TDC720x_MUX1_CTRL_PIN;                      // Set pin direction output 
  
  TI_TDC720x_MUX2_CTRL_PxOUT &= ~TI_TDC720x_MUX2_CTRL_PIN;                     // Set MUX1 low
  TI_TDC720x_MUX2_CTRL_PxDIR |= TI_TDC720x_MUX2_CTRL_PIN;                      // Set pin direction output  

  // Set msp430 to send start pulse
  TI_TDC720x_START_MSP430_PxOUT &= ~TI_TDC720x_START_MSP430_PIN;                  
  TI_TDC720x_START_MSP430_PxDIR |= TI_TDC720x_START_MSP430_PIN;                            
  
  // configure Port Pin to handle Interrupt Bar Output (INTB1) from TDC720x
  TI_TDC720x_INTB_PxDIR &= ~TI_TDC720x_INTB1_PIN;                              // Set up port pin for INTB
  TI_TDC720x_INTB_PxOUT |= TI_TDC720x_INTB1_PIN;                               // INTB Input Pull up enable  
  TI_TDC720x_INTB_PxREN |= TI_TDC720x_INTB1_PIN;                               // INTB Input Resistor enable  
  TI_TDC720x_INTB_PxIES |= TI_TDC720x_INTB1_PIN;                               // Interrupt Edge Select
  TI_TDC720x_INTB_PxIFG &= ~TI_TDC720x_INTB1_PIN;                              // Clear Interrupt Flag
//  TI_TDC720x_INTB_PxIE |= TI_TDC720x_INTB1_PIN;                              // Enable Port interrupt

  // configure Port Pin to handle Interrupt Bar Output (INTB2) from TDC720x
  TI_TDC720x_INTB_PxDIR &= ~TI_TDC720x_INTB2_PIN;                              // Set up port pin for INTB
  TI_TDC720x_INTB_PxOUT |= TI_TDC720x_INTB2_PIN;                               // INTB Input Pull up enable  
  TI_TDC720x_INTB_PxREN |= TI_TDC720x_INTB2_PIN;                               // INTB Input Resistor enable  
  TI_TDC720x_INTB_PxIES |= TI_TDC720x_INTB2_PIN;                               // Interrupt Edge Select
  TI_TDC720x_INTB_PxIFG &= ~TI_TDC720x_INTB2_PIN;                              // Clear Interrupt Flag
//  TI_TDC720x_INTB_PxIE |= TI_TDC720x_INTB2_PIN;                              // Enable Port interrupt

  // oscillator fail if LED keeps flashing after InitMCU     
  InitMCU();
  
  // Initilaize MSP430 UART A1 Block 
  InitUART();
  
  // Initilaize MSP430 SPI Block 
  TI_TDC720x_SPISetup();                                                        
  
  // init interval timer (timer1)
  timer1_A0_init();

  // init timer0 A0 for programmable external osc wake up time
  //timer0_A0_init();
  
  // init timer2 A0 to generate PWM
  timer2_A0_init();
  
  // default device is TDC7201 and default tdc is TDC1
  if (TDC720xEVM_Device == TDC7201)
  {
    TI_TDC720x_reg_init(TDC720x_TDC1);
    TI_TDC720x_reg_init(TDC720x_TDC2);
  } else
  {
    TI_TDC720x_reg_init(TDC720x_TDC1);
  }
  next_trigger_time = 0;
  
  while (1)
  {
//    __bis_SR_register(LPM0_bits + GIE);                                        // Enter LPM0, enable interrupts
    __no_operation();                                                          // For debugger 
    USBCommunicationTask();
    if (next_trigger_time)
    {
      next_trigger_time = 0;
      tdc_trigger_measure();
      __no_operation();                                                        // For debugger     
    }    
    __no_operation();                                                          // For debugger    
    }
}

//******************************************************************************
/**
* @brief  Local functions.                           
*/

/**
* @brief Function Name:  InitMCU.                                                
* @brief Description  :  Initializes the MSP430 peripherals and modules.
* @param parameters   :  none                                                   
* @return Value       :  none                                                   
*/   
//******************************************************************************
void InitMCU(void)
{
  
  __disable_interrupt();                                                       // Disable global interrupts 
//    Init_Ports();                                                              //Init ports (do first ports because clocks do change ports)
    SetVCore(3);
    Init_Clock();                                                              //Init clocks

#if 1
    USB_init();                 //Init USB

    USB_setEnabledEvents(
        kUSB_VbusOnEvent + kUSB_VbusOffEvent + kUSB_receiveCompletedEvent
        + kUSB_dataReceivedEvent + kUSB_UsbSuspendEvent + kUSB_UsbResumeEvent +
        kUSB_UsbResetEvent);

    //Check if we're already physically attached to USB, and if so, connect to it
    //This is the same function that gets called automatically when VBUS gets attached.
    if (USB_connectionInfo() & kUSB_vbusPresent){
        USB_handleVbusOnEvent();
    }
#endif
  __enable_interrupt();                                                        // enable global interrupts

}

//******************************************************************************
/**
* @brief Function Name: USBCommunicationTask     .                                             
* @brief Description  : Dumps data to the host over USB.
* @param parameters   : none                                                    
* @return Value       : none                                                    
*/   
//******************************************************************************
void USBCommunicationTask(void)
{
  WORD bytesSent, bytesReceived;
  uint16_t count;
  uint8_t send_error=0, receive_error=0, send_response, i;
  
        switch (USB_connectionState())
        {
            case ST_USB_DISCONNECTED:
//                __bis_SR_register(LPM3_bits + GIE);                     //Enter LPM3 w/interrupt
                _NOP();
                break;

            case ST_USB_CONNECTED_NO_ENUM:
                break;

            case ST_ENUM_ACTIVE:
                if (USBCDC_intfStatus(CDC0_INTFNUM,&bytesSent, &bytesReceived) & kUSBCDC_waitingForSend)
                    err_count++;
                // Exit LPM because of a data-receive event, and fetch the received data 
                if(bCDCDataReceived_event)
                {
                  bCDCDataReceived_event = FALSE;                                  // Clear flag early -- just in case execution breaks below because of an error
                  count = cdcReceiveDataInBuffer(cmdResponseString,MAX_STR_LENGTH,CDC0_INTFNUM);         // Count has the number of bytes received into dataBuffer    
                  send_response = handleHostCommand(cmdResponseString,count);
                  if (send_response)
                  {
                    if(cdcSendDataInBackground((BYTE*)cmdResponseString,MAX_STR_LENGTH,CDC0_INTFNUM,0))             // Send data to host
                    {
                      send_error = 1;                                            // Something went wrong -- exit
                      break;
                    }
                  }                  
                  for(i=0;i<MAX_STR_LENGTH;i++) 
                    cmdResponseString[i] = NULL;
                } else if ((tof_graph_state == 1) || (single_shot_measure_state == 1))         
                {
                  if (TDC720x_XYBufferReady)
                  {
                    TDC720x_XYBufferReady = 0;
                    if (cdcSendDataInBackground((BYTE*)ubuf,BUF_LENGTH,CDC0_INTFNUM,0))
                    {
                        bCDCDataReceived_event = FALSE;
                        USBCDC_abortSend(&w,CDC0_INTFNUM);              //Operation probably still open; cancel it
                    }
                    TI_TDC720x_GLED_PxOUT ^= TI_TDC720x_GLED_PIN;                      // Toggle LED
                    if (single_shot_measure_state == 1)
                    {
                      single_shot_measure_state = 0;   
                      TI_TDC720x_GLED_PxOUT |= TI_TDC720x_GLED_PIN;                        // Turn on LED
                    }
                    if (TDC720x_UART_Stream)
                      TDC720x_calc(ubuf, TDC720x_TDC1);
                  }
                  if (TDC720x_PQBufferReady)
                  {
                    TDC720x_PQBufferReady = 0;
                    if (cdcSendDataInBackground((BYTE*)vbuf,BUF_LENGTH,CDC0_INTFNUM,0))
                    {
                        bCDCDataReceived_event = FALSE;
                        USBCDC_abortSend(&w,CDC0_INTFNUM);              //Operation probably still open; cancel it
                    }
                    TI_TDC720x_GLED_PxOUT ^= TI_TDC720x_GLED_PIN;                      // Toggle LED
                    if (single_shot_measure_state == 1)
                    {
                      single_shot_measure_state = 0;   
                      TI_TDC720x_GLED_PxOUT |= TI_TDC720x_GLED_PIN;                        // Turn on LED
                    }
                    if (TDC720x_UART_Stream)
                      if (Measure_LT12ns_Method == TDC720x_MUX_METHOD)
                        TDC720x_calc(vbuf, TDC720x_TDC1);
                      else
                        TDC720x_calc(vbuf, TDC720x_TDC2);
                    
                  }                  
                }       
                break;

            case ST_ENUM_SUSPENDED:
//                __bis_SR_register(LPM3_bits + GIE);             //Enter LPM3 until a resume or VBUS-off event
                break;

            case ST_ENUM_IN_PROGRESS:
                break;

            case ST_NOENUM_SUSPENDED:
//                __bis_SR_register(LPM3_bits + GIE);
                break;

            case ST_ERROR:
                _NOP();
                break;

            default:;
        }                                                                            //end of switch-case sentence
        if(send_error || receive_error)
        { 
          //TO DO: User can place code here to handle error
        }        
}
//******************************************************************************
void Init_Clock(void)
{
 
  // Enable XT2 XIN/XOUT Pins
  P5SEL |= 0x0C;                            // Select XIN, XOUT on P5.3 and P5.2
  UCSCTL6 &= ~XT2OFF;                       // Enable XT2  
  UCSCTL6 |= XT2DRIVE_3; 
  
  P5SEL |= BIT4+BIT5;                       // Select XT1

  UCSCTL6 &= ~(XT1OFF);                     // XT1 On
  UCSCTL6 |= XCAP_3;                        // Internal load cap
  UCSCTL3 = 0;                              // FLL Reference Clock = XT1

  // Loop until XT1,XT2 & DCO stabilizes - In this case loop until XT1 and DCo settle
  do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                            // Clear XT2,XT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
    TI_TDC720x_RLED_PxOUT ^= TI_TDC720x_RLED_PIN;                              // Toggle LED
    __delay_cycles(250000);    
  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
  
  UCSCTL6 &= ~(XT1DRIVE_3);                 // Xtal is now stable, reduce drive strength

  UCSCTL4 |= SELA_0;                        // ACLK = LFTX1 (by default)

  __bis_SR_register(SCG0);                  // Disable the FLL control loop
  UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
  UCSCTL1 = DCORSEL_7;                      // Select DCO range 50MHz operation
  UCSCTL2 = FLLD_0 + 731;                   // Set DCO Multiplier(762/498) for 25MHz/16MHz
                                            // (N + 1) * FLLRef = Fdco
                                            // (762 + 1) * 32768 = 25MHz
                                            // Set FLL Div = fDCOCLK/2
  __bic_SR_register(SCG0);                  // Enable the FLL control loop

  // Worst-case settling time for the DCO when the DCO range bits have been
  // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
  // UG for optimization.
  // 32 x 32 x 25 MHz / 32,768 Hz ~ 780k MCLK cycles for DCO to settle
  __delay_cycles(782000);

  // Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
  do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                            // Clear XT2,XT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
    TI_TDC720x_RLED_PxOUT ^= TI_TDC720x_RLED_PIN;                              // Toggle LED
    __delay_cycles(250000);    
  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag  

  // 4/7: Select SMCLK = 4MHz 
  UCSCTL4 = SELA__XT1CLK + SELS__XT2CLK + SELM__DCOCLKDIV;                  // SMCLK=4MHz MCLK=24MHz

  // Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
  do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                            // Clear XT2,XT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
    TI_TDC720x_RLED_PxOUT ^= TI_TDC720x_RLED_PIN;                              // Toggle LED
    __delay_cycles(250000);    
  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag  
     
  TI_TDC720x_RLED_PxOUT &= ~TI_TDC720x_RLED_PIN;                                // turn off red  
           
}
//******************************************************************************

/*  
 * ======== UNMI_ISR ========
 */
#pragma vector = UNMI_VECTOR
__interrupt void UNMI_ISR (void)
{
    switch (__even_in_range(SYSUNIV, SYSUNIV_BUSIFG ))
    {
        case SYSUNIV_NONE:
            __no_operation();
            break;
        case SYSUNIV_NMIIFG:
            __no_operation();
            break;
        case SYSUNIV_OFIFG:
            UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT2OFFG); //Clear OSC flaut Flags fault flags
            SFRIFG1 &= ~OFIFG;                          //Clear OFIFG fault flag
            break;
        case SYSUNIV_ACCVIFG:
            __no_operation();
            break;
        case SYSUNIV_BUSIFG:
            SYSBERRIV = 0;                                      //clear bus error flag
            USB_disable();                                      //Disable
    }
}
//******************************************************************************
void TI_TDC720x_reg_init(uint8_t dev)
{

  TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CONFIG1_REG, 0x00, dev); // Default Mode 2
  TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CONFIG2_REG, 0x40, dev); // cal2 period = 10 clocks
  TI_TDC720x_SPIByteWriteReg(TI_TDC720x_INTRPT_STATUS_REG, 0x0B, dev); // clear interrupt status
  TI_TDC720x_SPIByteWriteReg(TI_TDC720x_INTRPT_MASK_REG, 0x07, dev); // interrupts enabled
  TI_TDC720x_SPIByteWriteReg(TI_TDC720x_COARSE_COUNTER_OVH_REG, 0xFF, dev); // default
  TI_TDC720x_SPIByteWriteReg(TI_TDC720x_COARSE_COUNTER_OVL_REG, 0xFF, dev); // default
  TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CLOCK_COUNTER_OVH_REG, 0xFF, dev); // default
  TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CLOCK_COUNTER_OVL_REG, 0xFF, dev); // default
  TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CLOCK_COUNTER_STOP_MASKH_REG, 0x00, dev); // default
  TI_TDC720x_SPIByteWriteReg(TI_TDC720x_CLOCK_COUNTER_STOP_MASKL_REG, 0x00, dev); // default
}
//******************************************************************************
//EOF
