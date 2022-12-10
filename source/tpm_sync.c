/*
 * @file tpm_sync.c - This file synchronizes the TPM clocks
 *
 * Originally, the idea was to keep both ping-pong buffers on different TPMs
 * to prevent data contention from the ADC -> DMA0 transfer from clashing with
 * the FFT calculations, so the TPM clocks needed to be synced.
 * 
 * @author Ishmael Pelayo, ispe3057@colorado.edu
​ ​*​ ​@date​ ​	December 02 2022
​ ​*​ ​@version​ ​1.0
 * -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <tpm_sync.h>
#include "MKL25Z4.h"



#define TPM1_CLK_INPUT_FREQ (48000000UL) // 48 MHz

// refer to tpm_sync.h for explanation
void init_tpm_sync() {

  // configure clock gating for tpm0 on scgc6
  SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK; 

  // Configure TPM clock source - KL25Z datasheet sec. 12.2.3
  SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);

  // KL25Z datasheet sec. 12.2.3
  // TPM must be disabled to select prescale/counter bits:
  //   PS   - sets precaler to 16 - yields 3MHz clock
  //   DMA  - enables DMA transfers 
  TPM1->SC &= ~TPM_SC_CMOD_MASK;
  TPM1->SC |= (TPM_SC_PS(0b100) | TPM_SC_DMA_MASK );

  // Continue the TPM operation while in debug mode
  // KL25Z datasheet sec. 31.3.7
  TPM1->CONF |= TPM_CONF_DBGMODE(3);

  // set the overflow val - KL25Z datasheet sec. 31.3.3
  TPM1->MOD = 4;

  // start timer
  TPM1->SC |= TPM_SC_CMOD(1);
}

