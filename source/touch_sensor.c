// Credits to Copyright (c) 2012-2013 Andrew Payne <andy@payne.org>
// for most of the helper functions and the a large majority or the TSI multi-channel API
// touch_sensor.c adopted from Assignment3, I changed some critical registers that were not
// ideal to have in Low-Power-Mode
// Additionally, the NVIC needs to be configured differently since SysTick is in use as well.
//

#include <stdio.h>
#include "MKL25Z4.h"
#include "common.h"

#define INT_TSI0 42
#define NCHANNELS 16
#define TSI_THRESHOLD 10
static volatile uint16_t raw_counts[NCHANNELS];
static volatile uint16_t base_counts[NCHANNELS];
static uint32_t enable_mask;                    // Bitmask of enabled channels

// Get current touch input value (normalized to baseline) for specified
// input channel
int touch_data(int channel)
{
    return raw_counts[channel] - base_counts[channel];
}

// Initiate a touch scan on the given channel
inline static void scan_start(int channel)
{
    TSI0->DATA = TSI_DATA_TSICH(channel) | TSI_DATA_SWTS_MASK;
}

// Return scan data for most recent scan
inline static uint16_t scan_data(void)
{
    TSI0->GENCS |= TSI_GENCS_EOSF_MASK;
    return TSI0->DATA & TSI_DATA_TSICNT_MASK;
}

// Initialize touch input
void touch_sensor_init(uint32_t channel_mask)
{
    // Turn on clock gating for TSI and configure touch input
    SIM->SCGC5 |= SIM_SCGC5_TSI_MASK;
    TSI0->GENCS |= (TSI_GENCS_ESOR_MASK          // Enable end of scan interrupt
                   | TSI_GENCS_MODE(0)          // Capactive sensing
                   | TSI_GENCS_REFCHRG(4)       // Reference charge 4 uA
                   | TSI_GENCS_DVOLT(0)         // Voltage rails
                   | TSI_GENCS_EXTCHRG(7)       // External osc charge
                   | TSI_GENCS_PS(4)            // Prescalar divide by 4
                   | TSI_GENCS_NSCN(11)         // Scans per electrode
                   | TSI_GENCS_TSIIEN_MASK      // Input interrupt enable
                   | TSI_GENCS_STPE_MASK        // Enable in STOP mode
                   );

    TSI0->GENCS |= TSI_GENCS_TSIEN_MASK;

    // Enable touch I/O pins for FRDM board and configure
    // the scan sequence
    PORTB->PCR[16] = PORT_PCR_MUX(0);      // PTB16 as touch channel 9
    PORTB->PCR[17] = PORT_PCR_MUX(0);      // PTB17 as touch channel 10

    // Read initial (baseline) values for each enabled channel
    int i, first_channel = 0;
    enable_mask = channel_mask;
    for(i=15; i>=0; i--) {
        if((1 << i) & enable_mask) {
            scan_start(i);
            while(!(TSI0->GENCS & TSI_GENCS_EOSF_MASK))      // Wait until done
                ;

            base_counts[i] = scan_data();
            first_channel = i;
        }
    }

    // Enable TSI interrupts and start the first scan
    enable_irq(INT_TSI0);
    scan_start(first_channel);
}

// Touch input interrupt handler
void TSI0_IRQHandler() __attribute__((interrupt("IRQ")));
void TSI0_IRQHandler(void)
{
    // Save data for channel
    uint32_t channel = (TSI0->DATA & TSI_DATA_TSICH_MASK) >> TSI_DATA_TSICH_SHIFT;
    raw_counts[channel] = scan_data();

    // Start a new scan on next enabled channel
    for(;;) {
        channel = (channel + 1) % NCHANNELS;
        if ((1 << channel) & enable_mask) {
            scan_start(channel);
            return;
        }
    }
}
