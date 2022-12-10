/* dsp_fft.c - Contains all the signal processing functions
 *
 * Public functions to handle a 512 sample FFT power spectrum via the CMSIS DSP
 *
 * @author  Ishmael Pelayo
 * @date    2020-12-07
 * @rev     1.2
 *
 * references: https://www.keil.com/pack/doc/CMSIS/DSP/html/group__RealFFT.html
 * https://www.youtube.com/watch?v=WnFyB9IuyI0&ab_channel=NXP-DesignwithUs
 * http://www.ocfreaks.com/tutorial-using-mcuxpresso-create-cortex-m-project-cmsis
 * https://www.tek.com/en/blog/window-functions-spectrum-analyzers
 * https://community.nxp.com/t5/MCUXpresso-General-Knowledge/Using-CMSIS-DSP-with-MCUXpresso-SDK-and-IDE/ta-p/1129232
 */
#include <dsp_fft.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "arm_const_structs.h"
#include "arm_math.h"

#define MAXSAMPLES    	 (512)
#define HARMONICS     	 (32)

// Describe the major formants in the PDA
#define FORMANT_G4    	 (1)
#define FORMANT_G5	  	 (3)
#define FORMANT_B5		 (5)
#define FORMANT_D_SHARP6 (4)
#define FORMANT_E_FLAT7  (9)
#define FORMANT_G7		 (11)
#define FORMANT_B7		 (14)

// define variables that are required for arm_fft
static q15_t FFT_mag[MAXSAMPLES];
static const int16_t hanning[MAXSAMPLES];


uint16_t dsp_fft_max_pitch(int16_t* fft_mag)
{

  // error case
  if (fft_mag==NULL )  {
    return -1;
  }

  int i=0;
  int16_t bucket_peak = 0;

  /* search through the first 32 bins of the FFT
   * (8192Hz/512) * 2 = 32 -- Sampling Rate/ N-point FFT -- double sided frequency spectrum
   * we are guaranteed to find some harmonic leakage in the first 32 bins as long as our
   * signal is above  2 * DC value
   */
  for (i = 1; i <HARMONICS; ++i)
  {
      if (fft_mag[bucket_peak] < fft_mag[i])
      bucket_peak=i;
  }

  // special case for detecting B5 because too many fundamental frequency artifacts at 1000Hz
  if((bucket_peak == 3) && (fft_mag[bucket_peak + 1] > HARMONICS * 2)){
	// choose the next ODD frequency
	bucket_peak = 5;
  }
return bucket_peak;
}

void dsp_fft_pitch_detect(int index) {

	switch(index) {

	   case FORMANT_G4:
	      printf("400Hz pitch detected! \n");
	      break;

	   case FORMANT_G5:
		  printf("800Hz pitch detected! \n");
		  break;

	   case FORMANT_B5:
		  printf("1000Hz pitch detected! \n");
		  break;

	   case FORMANT_D_SHARP6:
		  printf("1250Hz pitch detected! \n");
		  break;

	   case FORMANT_E_FLAT7:
		  printf("2500Hz pitch detected! \n");
		  break;

	   case FORMANT_G7:
		  printf("3150Hz pitch detected! \n");
		  break;

	   case FORMANT_B7:
		  printf("4000Hz pitch detected! \n");
		  break;

	   default :
		   printf("No input signal detected/pitch out of range \n");
	}
}

// see .h for more details
int16_t* dsp_fft_mag(uint16_t* samples, int nsamples) {

  // handle error:
  if (samples==NULL || nsamples != MAXSAMPLES) return NULL;

  arm_rfft_instance_q15 fft_q15_ctx = {0};
  q15_t FFT_input[nsamples];
  q15_t FFT_output[2*nsamples];
    
  // normalize samples to q15_t type from uint16_t type
  for (int i=0; i<nsamples; i++) {
    // shift down
    FFT_input[i] = (int16_t)(samples[i]-(1<<15));
    // apply Hanning Window to filter out edge discontinuity
    FFT_input[i] = ((q31_t)FFT_input[i]*hanning[i])>>15;
  }
  
  // initialize the real fft
  arm_rfft_init_q15(&fft_q15_ctx, nsamples, 0, 1);
 
  // see arm_rfft_q15 at below link for more info:
  // https://www.keil.com/pack/doc/CMSIS/DSP/html/group__RealFFT.html
  arm_rfft_q15(&fft_q15_ctx, (q15_t*)FFT_input, (q15_t*)FFT_output);

  // compute the power of the signal
  arm_cmplx_mag_squared_q15((q15_t*) FFT_output, (q15_t*) FFT_mag, nsamples);

  return (int16_t*) FFT_mag;
}


// the Hanning smoothing window
static const int16_t hanning[MAXSAMPLES] = {
    0,     1,     4,    11,    19,    30,    44,    60,
   79,   100,   123,   149,   178,   208,   242,   277,
  316,   356,   399,   445,   492,   543,   595,   650,
  708,   768,   830,   894,   961,  1030,  1102,  1175,
 1251,  1330,  1411,  1493,  1579,  1666,  1756,  1847,
 1942,  2038,  2136,  2237,  2339,  2444,  2551,  2660,
 2771,  2884,  3000,  3117,  3236,  3357,  3480,  3605,
 3732,  3861,  3992,  4125,  4260,  4396,  4534,  4674,
 4816,  4960,  5105,  5252,  5401,  5551,  5703,  5856,
 6012,  6168,  6327,  6486,  6648,  6810,  6975,  7140,
 7307,  7476,  7645,  7816,  7989,  8162,  8337,  8513,
 8691,  8869,  9049,  9229,  9411,  9594,  9778,  9963,
10149, 10335, 10523, 10712, 10901, 11091, 11282, 11474,
11667, 11860, 12054, 12249, 12444, 12640, 12836, 13033,
13230, 13428, 13627, 13825, 14025, 14224, 14424, 14624,
14825, 15025, 15226, 15427, 15628, 15830, 16031, 16232,
16434, 16635, 16837, 17038, 17239, 17440, 17641, 17842,
18043, 18243, 18443, 18643, 18842, 19041, 19240, 19438,
19635, 19833, 20029, 20225, 20421, 20616, 20810, 21004,
21197, 21389, 21580, 21771, 21961, 22150, 22338, 22525,
22712, 22897, 23081, 23265, 23447, 23628, 23808, 23987,
24165, 24342, 24517, 24692, 24865, 25036, 25207, 25376,
25543, 25710, 25875, 26038, 26200, 26361, 26520, 26677,
26833, 26988, 27140, 27292, 27441, 27589, 27735, 27879,
28022, 28163, 28302, 28439, 28575, 28709, 28840, 28970,
29098, 29224, 29349, 29471, 29591, 29709, 29825, 29939,
30052, 30162, 30270, 30375, 30479, 30581, 30680, 30778,
30873, 30966, 31056, 31145, 31231, 31315, 31397, 31477,
31554, 31629, 31701, 31772, 31840, 31905, 31969, 32030,
32088, 32144, 32198, 32250, 32299, 32345, 32390, 32431,
32471, 32508, 32542, 32574, 32604, 32631, 32656, 32678,
32698, 32715, 32730, 32742, 32752, 32760, 32765, 32767,
32767, 32765, 32760, 32752, 32742, 32730, 32715, 32698,
32678, 32656, 32631, 32604, 32574, 32542, 32508, 32471,
32431, 32390, 32345, 32299, 32250, 32198, 32144, 32088,
32030, 31969, 31905, 31840, 31772, 31701, 31629, 31554,
31477, 31397, 31315, 31231, 31145, 31056, 30966, 30873,
30778, 30680, 30581, 30479, 30375, 30270, 30162, 30052,
29939, 29825, 29709, 29591, 29471, 29349, 29224, 29098,
28970, 28840, 28709, 28575, 28439, 28302, 28163, 28022,
27879, 27735, 27589, 27441, 27292, 27140, 26988, 26833,
26677, 26520, 26361, 26200, 26038, 25875, 25710, 25543,
25376, 25207, 25036, 24865, 24692, 24517, 24342, 24165,
23987, 23808, 23628, 23447, 23265, 23081, 22897, 22712,
22525, 22338, 22150, 21961, 21771, 21580, 21389, 21197,
21004, 20810, 20616, 20421, 20225, 20029, 19833, 19635,
19438, 19240, 19041, 18842, 18643, 18443, 18243, 18043,
17842, 17641, 17440, 17239, 17038, 16837, 16635, 16434,
16232, 16031, 15830, 15628, 15427, 15226, 15025, 14825,
14624, 14424, 14224, 14025, 13825, 13627, 13428, 13230,
13033, 12836, 12640, 12444, 12249, 12054, 11860, 11667,
11474, 11282, 11091, 10901, 10712, 10523, 10335, 10149,
 9963,  9778,  9594,  9411,  9229,  9049,  8869,  8691,
 8513,  8337,  8162,  7989,  7816,  7645,  7476,  7307,
 7140,  6975,  6810,  6648,  6486,  6327,  6168,  6012,
 5856,  5703,  5551,  5401,  5252,  5105,  4960,  4816,
 4674,  4534,  4396,  4260,  4125,  3992,  3861,  3732,
 3605,  3480,  3357,  3236,  3117,  3000,  2884,  2771,
 2660,  2551,  2444,  2339,  2237,  2136,  2038,  1942,
 1847,  1756,  1666,  1579,  1493,  1411,  1330,  1251,
 1175,  1102,  1030,   961,   894,   830,   768,   708,
  650,   595,   543,   492,   445,   399,   356,   316,
  277,   242,   208,   178,   149,   123,   100,    79,
   60,    44,    30,    19,    11,     4,     1,     0};
