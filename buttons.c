/*
 * buttons.c
 *
 * Authors: Peter Sutton, Luke Kamols
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "buttons.h"
#include "timer0.h"

// Global variable to keep track of the last button state so that we 
// can detect changes when an interrupt fires. The lower 3 bits (0 to 2)
// will correspond to the last state of port C pins 0 to 2.
static volatile uint8_t last_button_state;

// Our button queue. button_queue[0] is always the head of the queue. If we
// take something off the queue we just move everything else along. We don't
// use a circular buffer since it is usually expected that the queue is very
// short. In most uses it will never have more than 1 element at a time.
// This button queue can be changed by the interrupt handler below so we should
// turn off interrupts if we're changing the queue outside the handler.
#define BUTTON_QUEUE_SIZE 4
static volatile uint8_t button_queue[BUTTON_QUEUE_SIZE];
static volatile int8_t queue_length;

// These buttons are not hardware debounced, instead they must be software
// debounced. The approach to this will be a simple one, rejecting any button
// pushes within a short period of time after the most recent one
static volatile uint32_t last_button_time[3];
#define DEBOUNCE_TIME 30

// Setup interrupt if any of pins C0 to C2 change. We do this
// using a pin change interrupt. These pins correspond to pin
// change interrupts PCINT8 to PCINT10 which are covered by
// Pin change interrupt 1.
void init_button_interrupts(void) {
	// Enable the interrupt (see datasheet page 82)
	PCICR |= (1<<PCIE1);
	
	// Make sure the interrupt flag is cleared (by writing a 
	// 1 to it) (see datasheet page 82)
	PCIFR |= (1<<PCIF1);
	
	// Choose which pins we're interested in by setting
	// the relevant bits in the mask register (see datasheet page 83)
	PCMSK1 |= (1<<PCINT8)|(1<<PCINT9)|(1<<PCINT10);	
	
	// Empty the button push queue
	queue_length = 0;
	
	// Set the last button pressed time for all pins to be zero
	// This is not the current time as that would enforce an ordering
	// on the hardware initialisation (i.e that the timer would have
	// to be set up first)
	for (uint8_t pin = 0; pin<=2; pin++) {
		last_button_time[pin] = 0;
	}
}

int8_t button_pushed(void) {
	int8_t return_value = NO_BUTTON_PUSHED;	// Assume no button pushed
	if(queue_length > 0) {
		// Remove the first element off the queue and move all the other
		// entries closer to the front of the queue. We turn off interrupts (if on)
		// before we make any changes to the queue. If interrupts were on
		// we turn them back on when done.
		return_value = button_queue[0];
		
		// Save whether interrupts were enabled and turn them off
		int8_t interrupts_were_enabled = bit_is_set(SREG, SREG_I);
		cli();
		
		for(uint8_t i = 1; i < queue_length; i++) {
			button_queue[i-1] = button_queue[i];
		}
		queue_length--;
		
		if(interrupts_were_enabled) {
			// Turn them back on again
			sei();
		}
	}
	return return_value;
}

// Interrupt handler for a change on buttons
ISR(PCINT1_vect) {
	// Get the current state of the buttons. We'll compare this with
	// the last state to see what has changed.
	uint8_t button_state = PINC & 0x07;
	
	uint32_t press_time = get_current_time();
	// Get the time this button press occurred
	
	// Iterate over all the buttons and see which ones have changed.
	// Any buttons which have changed have their debounce reference time updated
	// Any button pushes are added to the queue of button pushes (if
	// there is space). A button press is a transition from 0 in the 
	// last_button_state bit to a 1 in the button_state.
	for(uint8_t pin=0; pin<=2; pin++) {
		if (button_state & (1<<pin) && !(last_button_state & (1<<pin))) {
			// This is a transition from 0 to 1 on this pin
			if (queue_length < BUTTON_QUEUE_SIZE && 
					press_time >= last_button_time[pin] + DEBOUNCE_TIME) {
				// Add the button push to the queue (and update the
				// length of the queue
				button_queue[queue_length++] = pin;	
			}
			// Any button press, even if it is not added to the queue should
			// be registered for debouncing
			last_button_time[pin] = get_current_time();
		} else if (!(button_state & (1<<pin)) && last_button_state & (1<<pin)) {
			// This is a transition from 1 to 0 on this pin.
			// Update the debounce timer anyway because it can bounce on release
			last_button_time[pin] = get_current_time();
		}
	}
	
	// Remember this button state
	last_button_state = button_state;
}