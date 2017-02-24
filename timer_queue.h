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

#define MAX_STR_LENGTH 32
#define NUM_SAMPLES         1             // number of samples per block
#define SAMPLE_SIZE         TDC720x_ALL_DATA_SIZE // totally 39 bytes 
#define NUM_BLOCKS          1             //  

#define BUF_LENGTH (NUM_SAMPLES * SAMPLE_SIZE * NUM_BLOCKS)

#define INTERVAL_100ms         12500                                           // SMCLK = 4MHz & /32
#define INTERVAL_10ms          1250
#define INTERVAL_5ms           625

#define TDC720x_MUX_METHOD 0
#define TDC720x_TWO_DEVICE_METHOD 1

void timer0_A0_init(void); 
void timer1_A0_init(void);
void timer2_A0_init(void);
void timer_microsec_delay(uint16_t);
void tdc_trigger_measure(void);
uint8_t wait_on_intb_pin(uint16_t);
void two_tdc_read_results(void);
void one_tdc_read_results(void);


