/*
 * @file analog_peripherals.h - Coordinate the setup for the ADC sampling rate
 *
 *
 * @brief	double buffering using the DMA0 channel, additionally, the DMA0 us used as
 * 			a "shadow register" during signal processing hand-off
 *
 * Single ended 16-bit analog samples are recorded from KL25Z PortC, Pin1 (PTC1)
 *
 * @author  Ishmael Pelayo
 * @date    12-09-2022
 * @rev     1.2
 *
 */

#ifndef _ANALOG_PERIPHERALS_H_
#define _ANALOG_PERIPHERALS_H_

#include <stdint.h>
#include <stdbool.h>


/*
 * @brief  invokes a writing operation to the active buffer
 *
 * Begins storing samples into the ping-pong buffer so that the
 * current data remains recent.
 *
 * @params  none
 * @return uint16_t*, a buffer containing 512 ADC samples
 *                    NULL if ADC is not available
 */
uint16_t* get_samples();

/*
 * @brief   Returns the current state of the active buffer in the ping-pong scheme
 *
 * @params   none
 * @return  boolean, true  = data is available for reading
 *                false = data is not available for reading (cannot be processed)
 */
bool is_adc_pong_full();

/*
 * @brief   Initializes ADC0, DMA0, and TPM0 
 *
 * ADC0 is set up to trigger on TPM0 overflow at ADC_SAMPLING_FREQ. DMA0 is 
 * triggered on ADC0 conversion completion to move the ADC data to one of the
 * two ping pong buffers (A or B) depending on which one was previously written 
 *
 * @params   none
 * @return  none
 */
void analog_init();

/*
 * @brief   Initializes ADC0 for sampling via TPM0 overflow
 *
 * Sets up ADC0 for uin16_t readings from the external microphone circuit
 * pin PTC1
 *
 * @params   none
 * @return  none
 */
void init_adc0();

/*
 * @brief  Initializes DMA channel 0 for retrieving ADC values
 *
 * @param   none
 * @return  none
 */
void init_dma0();

/*
 * @brief   The DMA0 IRQ handler in charge of sub-sampling ADC0
 *
 * @param   none
 * @return  none
 */
void DMA0_IRQHandler();

/*
 * @brief   Initializes TPM0 overflow at ADC_SAMPLING_FREQ in Hz
 *
 * @param   none
 * @return  none
 */
void init_tpm0();

#endif // _ANALOG_PERIPHERALS_H_
