/*
 * analog_peripherals.c
 * This file contains the initialization of the ADC0, TPM0, and DMA0 to interface
 * with the external microphone circuit
 * via single ended 16-bit configuration on the KL25Z PortC, Pin1 (PTC1)
 *
 * @author  Ishmael Pelayo
 * @date    2022-12-01
 * @rev		1.2
 *
 * references: https://stm32f4-discovery.net/2015/06/how-to-properly-enabledisable-interrupts-in-arm-cortex-m/
 */

#include <analog_peripherals.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "MKL25Z4.h"

#define START_CRITICAL_SECTION \
          uint32_t masking_state = __get_PRIMASK(); \
          __disable_irq()

#define END_CRITICAL_SECTION \
          __set_PRIMASK(masking_state)

#define ADC_SAMPLING_FREQ  (8192U) // Frequency in Hz
#define ADC_MAX_SAMPLES    (512)  


// use a ping-pong buffer approach
static uint16_t adc_ping[ADC_MAX_SAMPLES]; // A
static uint16_t adc_pong[ADC_MAX_SAMPLES]; // B
// the status flags
static volatile bool adc_ping_active;
static volatile bool adc_pong_full;

// initializes the analog module to a known state using internal/public functions
void analog_init() {

  // init dma0, tpm0, adc0
  init_dma0();
  init_tpm0();
  init_adc0();

  // DMA sets the PING side of the double buffer to be filled first
  adc_ping_active = true;
  adc_pong_full = false;
  
  // turn on the TPM0 timer this starts DMA0
  TPM0->SC |= TPM_SC_CMOD(1);
}


// this functions checks to swap writing/reading order
bool is_adc_pong_full() {
  return adc_pong_full;
}


// begins keeping track of the buffer's sampled as they are retrieved by DMA
uint16_t* get_samples() {

  // if samples are not available, we are still recording
  // so return NULL
  if (!adc_pong_full) {
    return NULL;
  }
  
  // the return buffer:
  uint16_t* process_buffer_return;

  // disable interrupts when swapping buffers
  START_CRITICAL_SECTION;

  // change the order of the ping-pong buffers
  if (adc_ping_active) {
    process_buffer_return = adc_ping;
    // setup DMA destination
    DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) (&(adc_pong[0])));
    adc_ping_active = false;

  // change the order of the ping-pong buffers
  } else {
    process_buffer_return = adc_pong;
    // setup DMA destination
    DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) (&(adc_ping[0])));
    adc_ping_active = true;
  }
  
  // when pong is not filled, we know that the ping buffer is used in FFT
  adc_pong_full = false;

  // re-start DMA0
  DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_BCR(2*ADC_MAX_SAMPLES);
  DMA0->DMA[0].DCR |= DMA_DCR_ERQ_MASK;

  // restore context and interrupt priority
  END_CRITICAL_SECTION;

  // return buffer for processing
  return process_buffer_return;
}

// this is the standard DMA handler that clears the DMA transfer flag
void DMA0_IRQHandler() {
  // clear done flag
  DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
  // samples are now available
  adc_pong_full = true;
}

// ADC0 initialized similar to Lab7, except we are using a different pin
void init_adc0() {
  // enable clock gating
  SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
  // enable alternate trigger pg. 201
  // select TPM0 as trigger
  SIM->SOPT7  = SIM_SOPT7_ADC0ALTTRGEN(1) | SIM_SOPT7_ADC0TRGSEL(9);

  // pg. 466 datasheet
  // use bus clock (24 MHz) and divide by 4
  // sets to 16-bit single ended conversion
  // note: input clock must be between 2 and 12 MHz for 16-bit mode
  ADC0->CFG1  = ADC_CFG1_ADICLK(0) | ADC_CFG1_ADIV(2)  |
                ADC_CFG1_MODE(3)   | ADC_CFG1_ADLPC(0) |
                ADC_CFG1_ADLSMP(0);

  ADC0->CFG2 = 0;

  // select reference voltages
  ADC0->SC2 = ADC_SC2_REFSEL(0) |  ADC_SC2_ADTRG(0) |
              ADC_SC2_ACFE(0)   |  ADC_SC2_DMAEN(0);

  // set adc0 input to SE15
  // PORTC1
  // AIEN and DIFF set to standard configuration
  ADC0->SC1[0] = ADC_SC1_ADCH(15) | ADC_SC1_AIEN(0) | ADC_SC1_DIFF(0);

  // run calibration datasheet sec 28.4.6:
  // turn on averaging (32) temporarily for calibration sequence
  ADC0->SC3 |= ADC_SC3_CAL_MASK | ADC_SC3_AVGS(3);
  // start calibration
  while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK)) {;} // wait for calibration complete

  // plus-side calibration
  uint16_t cal = 0;
  cal = ADC0->CLP0+ADC0->CLP1+ADC0->CLP2+ADC0->CLP3+ADC0->CLP4+ADC0->CLPS;
  cal = cal/2;
  cal |= (1<<15); // set MSB
  ADC0->PG = cal; // write calibration

  // minus-side calibration
  cal = 0;
  cal = ADC0->CLM0+ADC0->CLM1+ADC0->CLM2+ADC0->CLM3+ADC0->CLM4+ADC0->CLMS;
  cal = cal/2;
  cal |= (1<<15); // set MSB
  ADC0->MG = cal; // write calibration

  // disable averaging
  ADC0->SC3 &= ~(ADC_SC3_AVGS_MASK);
  // re-enable hardware triggering
  // enable dma request
  ADC0->SC2 |= ADC_SC2_ADTRG(1) | ADC_SC2_DMAEN(1);
}

#define DMA_ADC0_COCO_TRIG  (40)

// DMA0 initialized similar to Lab7, except we are using ADC0 rather than DAC0
void init_dma0() {
  // enable clock gating to DMA
  SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
  SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

  // disable during config.
  DMAMUX0->CHCFG[0] = 0;

  // see pg. 357 of datasheet
  // EINT  - Enable interrupts on transfer completion
  // ERQ   - Enable peripheral request (from ADC0)
  // DINC  - Enable destination increment after transfer
  // SSIZE - sets source size to 16 bits (from ADC0)
  // DSIZE - sets destination size to 16 bits
  // D_REQ - DCR ERQ bit is cleared when BCR is depleted
  // CS    - force single read/write per request (cycle steal)
  DMA0->DMA[0].DCR = ( DMA_DCR_EINT_MASK  |
                       DMA_DCR_ERQ_MASK   |
                       DMA_DCR_DINC_MASK  |
                       DMA_DCR_SSIZE(2)   |
                       DMA_DCR_DSIZE(2)   |
                       DMA_DCR_D_REQ_MASK |
                       DMA_DCR_CS_MASK    );

  // setup source from adc0, dest to adc_samples
  DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t)&(ADC0->R[0]));
  DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t)&(adc_ping[0]));
  // load BCR with ADC_MAX_SAMPLES*2 bytes (16-bits) per transfer 
  DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_BCR(2*ADC_MAX_SAMPLES);
  
  // configure the interrupt upon transfer complete, priority 
  NVIC_SetPriority(DMA0_IRQn, 2);
  NVIC_ClearPendingIRQ(DMA0_IRQn);
  NVIC_EnableIRQ(DMA0_IRQn);

  // turn on the DMA, triggered by ADC0 conversion complete datasheet 3.4.8.1
  DMAMUX0->CHCFG[0] = (DMAMUX_CHCFG_SOURCE(DMA_ADC0_COCO_TRIG) |
                       DMAMUX_CHCFG_ENBL_MASK);
}


#define TPM0_CLK_INPUT_FREQ (48000000UL) // 48 MHz
// TPM0 module sets the sampling frequency for the ADC0 at 48000Khz (Studio Quality)
void init_tpm0() {

  // configure clock gating for tpm0 on scgc6
  SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK; 
  // Configure TPM clock source - KL25Z datasheet sec. 12.2.3
  SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL(1));
  // set TPM count direction to up with prescaler
  // KL25Z datasheet sec. 12.2.3
  // TPM must be disabled to select prescale/counter bits:
  TPM0->SC &= ~TPM_SC_CMOD_MASK;
  TPM0->SC |= TPM_SC_PS(0) | TPM_SC_CPWMS(0);
  // Continue the TPM operation while in debug mode
  // KL25Z datasheet sec. 31.3.7
  TPM0->CONF |= TPM_CONF_DBGMODE(0b11);
  // set the overflow - KL25Z datasheet sec. 31.3.3
  TPM0->MOD = TPM0_CLK_INPUT_FREQ/ADC_SAMPLING_FREQ - 1;
  // clear counter 
  TPM0->CNT = 0;
  // TPM0 will be started when reading samples
}
