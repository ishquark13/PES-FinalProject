/*
 * @file	main.c
 *
 * @brief	application entry point
 * @author​	​Ishmael Pelayo
​ ​*​ ​@date​ ​	December 08 2022
​ ​*​ ​@version​ ​1.0
 *
 */
#include <dsp_fft.h>
#include "analog_peripherals.h"
#include "leds.h"
#include "touch_sensor.h"
#include <stdio.h>
#include <test_dsp_fft.h>
#include <tpm_sync.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"

void system_init() {
  // initialize hardware
  BOARD_InitBootPins();
  BOARD_InitBootClocks();
  BOARD_InitBootPeripherals();

#ifdef DEBUG
  // initialize debug console
  BOARD_InitDebugConsole();
#endif
  
  // initialize ADC controls that communicate with external microphone
  analog_init();

  // initialize the TPM clocks
  init_tpm_sync();
}


int main(void) {

  // initialize the FRDM-kl25z board to a default state
  system_init();

  // initialize UI
  init_rgb_led();
  touch_sensor_init((1 << TSI_SENSOR_CHANNEL));

  // run tests
  printf("Number of passing Unit Tests %d/5 \r\n", test_dsp());

  // local and global variables to keep track of application status
  uint16_t current_bin;
  uint16_t *samples;
  int16_t *fft_mags;
  bool g_recording = false;
  bool g_output    = false;


  // main program loop
  while(1){

		  // wait for ADC sampling to complete and be available for reading
		  while( !is_adc_pong_full() ) {;}
		  // get ADC samples from microphone (also begins new sampling sequence) swap ping-pong
		  samples = get_samples();

		  // 1D transform of current signal's power spectrum
		  fft_mags = dsp_fft_mag(samples, 512);

		  // DEBUG: print out magnitudes and correlate with Freq
//		  for (int i=0; i<32; i++) {
//					printf("%d, FFT MAGNITUDES: %d\r\n", i, fft_mags[i]);
//				  }

		  // compute the current bin number of the FFT that contains most energy (PARSEVAL THM)
		  current_bin = dsp_fft_max_pitch(fft_mags);

		  if(touch_data(10) > TSI_THRESHOLD && g_recording == false) {
			  //begin recording
			  green_led_off();
			  red_led_on();
			  g_recording = true;
			  g_output = true;
		  }
		  if(g_recording) {
			  // end recording
			  red_led_off();
			  green_led_on();
			  g_recording = false;
			  if(g_output) {
				  // compare extracted harmonics to precomputed signal values
				  dsp_fft_pitch_detect(current_bin);
				  g_output = false;
			  }

		  }


  }

  return 1;

}
