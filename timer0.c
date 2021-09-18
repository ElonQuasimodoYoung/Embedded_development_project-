/*
 * timer0.c
 *
 * Author: Peter Sutton
 *
 * We setup timer0 to generate an interrupt every 1ms
 * We update a global clock tick variable - whose value
 * can be retrieved using the get_clock_ticks() function.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer0.h"
#include "scoring.h"


/* Our internal clock tick count - incremented every 
 * millisecond. Will overflow every ~49 days. */
static volatile uint32_t clockTicks;
// if "P" or "p" is pressed, the value is 1 and the game will pause. Otherwise, the value is 0
// and the game is continue
uint8_t pause_state = 0;

// Seven segment display - segment values for digits 0 to 9
uint8_t seven_seg[10] = {63, 6, 91, 79, 102, 109, 125, 7, 127, 111};

uint8_t digit = 0; /* 0 = right, 1 = left */

/* Set up timer 0 to generate an interrupt every 1ms. 
 * We will divide the clock by 64 and count up to 249.
 * We will therefore get an interrupt every 64 x 250
 * clock cycles, i.e. every 1 milliseconds with a 16MHz
 * clock. 
 * The counter will be reset to 0 when it reaches it's
 * output compare value.
 */
void init_timer0(void) {
	/* Reset clock tick count. L indicates a long (32 bit) 
	 * constant. 
	 */
	clockTicks = 0L;
	
	/* Set port B (pins 3, 4 and 5) to be outputs */
	DDRB |= 0x38;

	/* Set port C, pin 3 to be an output */
	DDRC |= (1 << DDC3);
	
	/* Set port D (pins 2 to 7) to be outputs */
	DDRD |= 0xFC;
	
	/* Clear the timer */
	TCNT0 = 0;

	/* Set the output compare value to be 124 */
	OCR0A = 249;
	
	/* Set the timer to clear on compare match (CTC mode)
	 * and to divide the clock by 64. This starts the timer
	 * running.
	 */
	TCCR0A = (1<<WGM01);
	TCCR0B = (1<<CS01)|(1<<CS00);

	/* Enable an interrupt on output compare match. 
	 * Note that interrupts have to be enabled globally
	 * before the interrupts will fire.
	 */
	TIMSK0 |= (1<<OCIE0A);
	
	/* Make sure the interrupt flag is cleared by writing a 
	 * 1 to it.
	 */
	TIFR0 &= (1<<OCF0A);
}

uint32_t get_current_time(void) {
	uint32_t returnValue;

	/* Disable interrupts so we can be sure that the interrupt
	 * doesn't fire when we've copied just a couple of bytes
	 * of the value. Interrupts are re-enabled if they were
	 * enabled at the start.
	 */
	uint8_t interruptsOn = bit_is_set(SREG, SREG_I);
	cli();
	returnValue = clockTicks;
	if(interruptsOn) {
		sei();
	}
	return returnValue;
}

void display_digit(uint8_t number, uint8_t digit)
{
	// Right digit select is on port B, left is port C.
	PORTB |= (!digit << PORTB3);
	PORTC |= (digit << PORTC3);
	
	// We assume digit is in range 0 to 9
	// Note |= for PORTB since we do not want to overwrite the CC pin setting.
	PORTB |= ((seven_seg[number] & 0xC0) >> 6) << 4; // output upper 2 bits of SSD to Port B bits 4 and 5
	PORTD |= (seven_seg[number] & 0x3F) << 2; // output lower 6 bits of SSD to Port D.
}

void pause_game(void) {
	if (pause_state == 0) {
		pause_state = 1;
	} else {
		pause_state = 0;
	}
}

ISR(TIMER0_COMPA_vect) {
	/* Increment our clock tick count */
	if (pause_state == 0) {
		clockTicks++;
	} else {
		clockTicks = clockTicks;
	}
	
	
	uint8_t value;
	
	if(digit == 0) {
		/* Extract the ones place from the timer counter 0 value */
		/* HINT: Consider the modulus (%) operator. */
		value = get_score() % 10;
		} else {
		/* Extract the tens place from the timer counter 0 */
		value = (get_score() / 10) % 10;
	}
	display_digit(value, digit);
	/* Change the digit flag for next time. if 0 becomes 1, if 1 becomes 0. */
	digit = 1 - digit;
}
