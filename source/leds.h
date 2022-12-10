/* gpio.h - a file containing functions and definitions when using the PORTB and PORTD GPIO
 *
 * Author: Ishmael Pelayo, ispe3057@colorado.edu
 *
 */

/* These are definitions to alter registers for LED color output */
#define red_led_on()    {GPIOB->PDOR &= ~((uint32_t)(1 << 18));}
#define red_led_off()   {GPIOB->PDOR |=  ((uint32_t)(1 << 18));}

#define green_led_on()  {GPIOB->PDOR &= ~((uint32_t)(1 << 19));}
#define green_led_off() {GPIOB->PDOR |=  ((uint32_t)(1 << 19));}

#define blue_led_on()   {GPIOD->PDOR &= ~((uint32_t)(1 << 1));}
#define blue_led_off()  {GPIOD->PDOR |=  ((uint32_t)(1 << 1));}

/*
 * This functions initializes the GPIO registers
 */
void init_rgb_led();

