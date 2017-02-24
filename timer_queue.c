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
//#include <cstdio>
#include <stdio.h>
#include "TI_TDC720x.h"  
#include "TI_MSP430.h"
#include "TI_MSP430_hardware_board.h"
#include "TI_MSP430_spi.h"
#include "timer_queue.h"
#include "tdc720x_mode1_lt12ns.h"
#include "tdc720x_measurement_control.h"
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
 
extern uint8_t bufferX[], bufferY[];
extern uint8_t *mbuf, *ubuf;
extern volatile uint8_t TDC720x_XYBufferReady;

extern uint8_t bufferP[], bufferQ[];
extern uint8_t *nbuf, *vbuf;
extern volatile uint8_t TDC720x_PQBufferReady;

extern uint8_t single_shot_measure_state;
extern uint8_t tof_graph_state;

uint8_t timer_done_flag = 0;
uint16_t timer_trigger_freq = 1;
volatile uint8_t next_trigger_time = 0;
uint8_t TDC720x_Measure_LT12ns = 0;
uint8_t Measure_LT12ns_Method = TDC720x_MUX_METHOD;
uint8_t TDC720x_Graph_Select = TDC720x_TDC1;


//******************************************************************************

//------------------------------------------------------------------------------
//  void timerA0_init(void)
//
//  DESCRIPTION:
//  Initialize timer0_A0 for capture    
//  Note: This function is for MSP430F5528 & might need modifications for other MSP430s
//------------------------------------------------------------------------------
void timer0_A0_init(void)
{
  // set initial timeout period of 3000us: equal to extl osc max wakeup period
  TA0CCR0 = 5000;     
  // disable timer0 A0 interrupt
  TA0CCTL0 &= ~CCIE;                                                             
  // Note: TACLR clears both counter and clock divider: setup divider later                                                        
  // SMCLK (4MHz), clear TAR, stop mode, divide by 4, no overflow intrpt TAIE
  TA0CTL |= TACLR;
  TA0CTL = TASSEL_2 + MC_0 + ID_2;  
  // Further divide by 3: 1us timer clock period
  //TA0EX0 = TAIDEX_2;
}
//******************************************************************************
// Timer0 A0 interrupt service routine
//------------------------------------------------------------------------------
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR (void)
{
  timer_done_flag = 1;         
//  __bic_SR_register_on_exit(LPM0_bits);                                        // Exit LPM0  
  
}
//******************************************************************************
//------------------------------------------------------------------------------
//  void timer1_A0_init(void)
//
//  DESCRIPTION:
//  Initialize timer1_A0 for timed AFE trigger    
//  Note: This function is for MSP430F5528 & might need modifications for other MSP430s
//------------------------------------------------------------------------------
void timer1_A0_init(void)
{
  TA1CCR0 = INTERVAL_100ms;      
  // enable timer interrupt
  TA1CCTL0 |= CCIE;                                                            // TA1CCR0 interrupt enabled       
  // Note: TACLR clears both counter and clock divider: setup divider later
  TA1CTL |= TACLR;
  TA1CTL = TASSEL_2 + MC_1 + ID_2;                                     // SMCLK (4MHz), clear TAR, conts mode, divide by 4, no overflow intrpt TAIE 
  TA1EX0 = TAIDEX_7;                                                           // further divide by 8
    
}
//******************************************************************************
//------------------------------------------------------------------------------
//  void timer2_A0_init(void)
//
//  DESCRIPTION:
//  Initialize timer2_A0 for timed AFE trigger    
//  Note: This function is for MSP430F5528 & might need modifications for other MSP430s
//------------------------------------------------------------------------------
void timer2_A0_init(void)
{
  uint8_t pwm_period;
  
  //SMCLK assumed to be 4MHz
  pwm_period = 11; // default freq = 4MHz/12 = 0.333MHz
  // Select PWM output pins: DTG_TRIG and REF_START pins
  TI_TDC720x_DTG_TRIG_PxOUT &= ~TI_TDC720x_DTG_TRIG_PIN;                       // Set pin output low
  TI_TDC720x_DTG_TRIG_PxDIR |= TI_TDC720x_DTG_TRIG_PIN;                        // Set pin direction is output
  TI_TDC720x_DTG_TRIG_PxDS |= TI_TDC720x_DTG_TRIG_PIN;
  
//  TI_TDC720x_REF_START_PxOUT &= ~TI_TDC720x_REF_START_PIN;                     // Set pin output low
  TI_TDC720x_REF_START_PxDIR &= ~TI_TDC720x_REF_START_PIN;                      // Set pin direction is output
//  TI_TDC720x_REF_START_PxDS |= TI_TDC720x_REF_START_PIN;
  
  // Output the clock on the pin when enabled by TDC720x_MEASURE_LT12NS user command   
  TA2CCR0 = pwm_period;                                                        // PWM Period
  // enable timer interrupt
  TA2CCTL0 |= CCIE;   
  // enable set/reset of P2.4 and P2.5 Timer PWM pins: P2.4 is REF_START, P2.5 is DTG_TRIGG
  // First give REF_START then DTG_TRIGG
  TA2CCTL1 = OUTMOD_3;                                                         // CCR1 set/reset
  TA2CCTL2 = OUTMOD_3;                                                         // CCR2 set/reset
  TA2CCR1 = (pwm_period+1) / 3;
  TA2CCR2 = 2 * (pwm_period+1)/3;                                              // CCR2 PWM duty cycle
  TA2CTL = TASSEL__SMCLK + MC_0 + TACLR;                                       // SMCLK, stop mode, clear TAR

}
//******************************************************************************
// Timer1 A0 interrupt service routine
//------------------------------------------------------------------------------
#pragma vector=TIMER2_A0_VECTOR
__interrupt void TIMER2_A0_ISR (void)
{
 // stop TA2 
 TA2CTL &= ~MC_3;
 TI_TDC720x_REF_START_PxSEL &= ~(TI_TDC720x_REF_START_PIN | TI_TDC720x_DTG_TRIG_PIN);                   
// TI_TDC720x_DTG_TRIG_PxSEL &= ~TI_TDC720x_DTG_TRIG_PIN; 
// __bic_SR_register_on_exit(LPM0_bits);                                       // Exit LPM0  
  
}
//******************************************************************************
// Timer1 A0 interrupt service routine
//------------------------------------------------------------------------------
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR (void)
{
  static uint8_t delay_sec=0;

  
  delay_sec++;
  if (delay_sec > timer_trigger_freq)
  {
    next_trigger_time = 1;
    delay_sec = 0;
// __bic_SR_register_on_exit(LPM0_bits);                                       // Exit LPM0  
  
  }
}
//******************************************************************************
void tdc_trigger_measure(void)
{
  
  // enable external osc before starting measurement
  //TI_TDC720x_OSCENABLE_PxOUT |= TI_TDC720x_OSCENABLE_PIN;                    // Set pin high: enable afe osc
  //TI_TDC720x_OSCENABLE2_PxOUT |= TI_TDC720x_OSCENABLE2_PIN;
      
  // use default delay if input is 0
  //timer_microsec_delay(0);  
  
  if ((tof_graph_state == 1) || (single_shot_measure_state == 1))
  {
    if (TDC720x_Measure_LT12ns == 1)
      TDC720x_mode1_lt12ns();
    else if ((tof_graph_state == 1) && (TDC720x_Graph_Select == TDC720x_BOTH))
      two_tdc_measure_results();
    else
      one_tdc_measure_results();
  }
  
  // disable osc after completing measurement
  //TI_TDC720x_OSCENABLE_PxOUT &= ~TI_TDC720x_OSCENABLE_PIN;                   // Set pin low: disable afe osc    
  //TI_TDC720x_OSCENABLE2_PxOUT &= ~TI_TDC720x_OSCENABLE2_PIN;
  
  // __bic_SR_register_on_exit(LPM0_bits);  
}

//******************************************************************************
void timer_microsec_delay(uint16_t us)
{
  uint16_t save_delay;
  save_delay = TA0CCR0;
  if (us)
    TA0CCR0 = us;
  timer_done_flag = 0;
  TA0CCTL0 |= CCIE;                                                            // enable timer0A0 int
  // since we are calling from an isr...
  __enable_interrupt();
  // SMCLK (4MHz), clear TAR, up mode, divide by 4, no overflow intrpt TAIE
  TA0CTL |= MC_1;
  // wait for timer programmed delay
  while (!timer_done_flag);
  // SMCLK (4MHz), clear TAR, stop mode, divide by 4, no overflow intrpt TAIE
  TA0CTL &= ~MC_1;
  // Restore delay
  TA0CCR0 = save_delay;  
}
//******************************************************************************
uint8_t wait_on_intb_pin(uint16_t pin)
{
  uint32_t tout = 0;
  //wait for INTB2 pin to go low
  while(TI_TDC720x_INTB_PxIN & pin)
  {
    tout++;
    // 1us delay below at 24MHz master clock
    __delay_cycles(24);
    // TIMEOUT_VALUE is 200000, wait atleast 200ms and timeout
    if (tout >= TIMEOUT_VALUE)
     return 0;
  }
  return 1;
}
//******************************************************************************
