/* touch_sensor.h - a file containing functions and definitions that enable
 * the capacitive touch sensor module to output scanned values from the electrodes.
 *
 * Author: Ishmael Pelayo, ispe3057@colorado.edu
 * Credits: Ben Roloff Rev 1.0	5/11/2016 from Digikey Forum Post:
 * using-the-capacitive-touch-sensor-on-the-frdm-kl46z/13246
 *
 * Credits to 2012-2013 Andrew Payne <andy@payne.org>
 * for putting together a demo code snippet of generating interrupt driven TSI values
 */

#ifndef __TOUCH_SENSOR_H_
#define __TOUCH_SENSOR_H_

#include "MKL25Z4.h"
#define TSI_SENSOR_CHANNEL (10)

// Touch Sensor function prototypes
void touch_sensor_init(uint32_t channel_mask);

int touch_data(int channel);
void touch_init(uint32_t channel_mask);

// Interrupt enabling and disabling
static inline void enable_irq(int n) {
    NVIC->ICPR[0] |= 1 << (n - 16);
    NVIC->ISER[0] |= 1 << (n - 16);
}

// Macros
#define SCAN_OFFSET 586 // Offset for scan range
#define SCAN_DATA TSI0->DATA & 0xFFFF // Accessing the bits held in TSI0_DATA_TSICNT
#define TSI_THRESHOLD 15

#endif
