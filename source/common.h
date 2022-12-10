//
//  Copyright (c) 2012-2013 Andrew Payne <andy@payne.org>
//


// From touch_sensor.c
int touch_data(int channel);
void touch_init(uint32_t channel_mask);

// Interrupt enabling and disabling to not conflict with other modules
static inline void enable_irq(int n) {
    NVIC->ICPR[0] |= 1 << (n - 16);
    NVIC->ISER[0] |= 1 << (n - 16);
}

