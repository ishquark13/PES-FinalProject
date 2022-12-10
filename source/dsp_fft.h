/*
 * dsp_fft.h - This file contains the custom library for FFT using CMSIS
 *
 * invoke FFT functions using CM0_ARM_MATH instances
 * for this project, the best benchmark of arm_rfft was the 512 length
 *
 *
 * @author  Ishmael Pelayo
 * @date    2022-12-05
 * @rev     1.1
 *
 * Reference:  https://www.keil.com/pack/doc/CMSIS/DSP/html/group__RealFFT.html
 *
 */
#include <stdint.h>

#ifndef _DSP_FFT_H_
#define _DSP_FFT_H_


/* @brief   Returns the NORM of a 512 sample real FFT in array form
 * 
 * The power spectrum is projected into an array containing the power of the signal
 * after the FFT has been performed
 *
 * @param   data,  the sampled data casted as uint16_t (ADC0)
 *          nsamples, 512 sample FFT
 *
 * @return  int16_t, of the real and imaginary parts of the FFT
 */
int16_t* dsp_fft_mag(uint16_t* data, int nsamples);

/* @brief  Locates which frequency bin the speech formant is centered around.
 *
 * This function traverses through the fft magnitude array and compares the relative strength
 * of the 1D power spectrum using the definition of a "speech formant" and returns the index
 * where the maximum value is found.
 *
 * @param  fft_mag, magnitude array of the FFT result from the ping-pong buffer
 *
 *
 * @return  index where the formant contains the highest value compared to average value
 */
uint16_t dsp_fft_max_pitch(int16_t* fft_mag);

/* @brief  Finds the pitch by comparing fundamental frequency and subsequent harmonics
 *
 * This function identifies which of the predetermined speech formants is found by
 * comparing known harmonic values of notes.
 *
 * @param  index, frequency bin of the FFT output
 *
 *
 * @return none
 */
void dsp_fft_pitch_detect(int index);
#endif // _DSP_FFT_H_
