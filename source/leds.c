#include <leds.h>
#include "MKL25Z4.h"

/*
 * This function initializes the GPIO PORTS B and PORTD for the RGB LED
 *
 * Parameters:
 * None
 *
 * Returns:
 * None
 */
void init_rgb_led()
{
	// Turn on clock to the PORTB module
	SIM->SCGC5|= SIM_SCGC5_PORTB_MASK;

	// Turn on clock to the PORTD module
	SIM->SCGC5|= SIM_SCGC5_PORTD_MASK;

	// Set the PORTB PIN 18 (RED LED) multiplexer to GPIO mode
	PORTB->PCR[18] = (uint32_t) (0x00000100);

	// Set the PORTB PIN 19 (GREEN LED) multiplexer to GPIO mode
	PORTB->PCR[19] = (uint32_t) (0x00000100);

	// Set the PORTD PIN 01 (BLUE LED) multiplexer to GPIO mode
	PORTD->PCR[1] = (uint32_t) (0x00000100);

	// Set the initial output of the pins to high
	GPIOB->PDOR |= (uint32_t) (1 << 18);
	GPIOB->PDOR |= (uint32_t) (1 << 19);
	GPIOD->PDOR |= (uint32_t) (1 << 1);

	// Set the pins' direction to output
	GPIOB->PDDR |= 1 << 18;
	GPIOB->PDDR |= 1 << 19;
	GPIOD->PDDR |= 1 << 1;
}

