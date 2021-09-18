/*
 * project.c
 *
 * Main file
 *
 * Authors: Peter Sutton, Luke Kamols
 * Modified by <Donghao Yang>
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "game.h"
#include "display.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "timer0.h"
#include "scoring.h"

#define F_CPU 16000000L
#include <util/delay.h>

// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void start_screen(void);
void new_game(void);
void play_game(void);
void handle_game_over(void);

// check if the game is paused
uint8_t pause = 0;


/////////////////////////////// main //////////////////////////////////
int main(void) {
	// Setup hardware and call backs. This will turn on 
	// interrupts.
	initialise_hardware();
	
	// Show the splash screen message. Returns when display
	// is complete
	start_screen();
	
	// Loop forever,
	while(1) {
		new_game();
		play_game();
		handle_game_over();
	}
}

void initialise_hardware(void) {
	init_button_interrupts();
	// Setup serial port for 19200 baud communication with no echo
	// of incoming characters
	init_serial_stdio(19200,0);
	
	init_timer0();
	
	// Turn on global interrupts
	sei();
}

void start_screen(void) {
	// Clear terminal screen and output a message
	clear_terminal();
	move_terminal_cursor(10,10);
	printf_P(PSTR("Reversi"));
	move_terminal_cursor(10,12);
	printf_P(PSTR("CSSE2010/7201 project by <Donghao Yang 45930032>"));
	
	// Output the static start screen and wait for a push button 
	// to be pushed or a serial input of 's'
	start_display();
	
	// Wait until a button is pressed, or 's' is pressed on the terminal
	while(1) {
		// First check for if a 's' is pressed
		// There are two steps to this
		// 1) collect any serial input (if available)
		// 2) check if the input is equal to the character 's'
		char serial_input = -1;
		if (serial_input_available()) {
			serial_input = fgetc(stdin);
		}
		// If the serial input is 's', then exit the start screen
		if (serial_input == 's' || serial_input == 'S') {
			break;
		}
		// Next check for any button presses
		int8_t btn = button_pushed();
		if (btn != NO_BUTTON_PUSHED) {
			break;
		}
	}
}

void new_game(void) {
	// Clear the serial terminal
	clear_terminal();
	
	// Initialise the game and display
	initialise_board();
	
	// Clear a button push or serial input if any are waiting
	// (The cast to void means the return value is ignored.)
	(void)button_pushed();
	clear_serial_input_buffer();
	
	// Initialise scores
	init_score();
}

void play_game(void) {
	
	uint32_t last_flash_time, current_time;
	uint8_t btn; //the button pushed
	
	last_flash_time = get_current_time();
	
	// We play the game until it's over
	while(!is_game_over() && !no_available_move_game_over()) {
		// First check for buttons pressed
		
		// We need to check if any button has been pushed, this will be
		// NO_BUTTON_PUSHED if no button has been pushed
		btn = button_pushed();
		
		if (btn == BUTTON2_PUSHED && pause == 0) {
			// If button 2 is pushed, move left,
			// i.e decrease x by 1 and leave y the same
			move_display_cursor(-1, 0);
		} else if (btn == BUTTON1_PUSHED && pause == 0) {
			// If button 1 is pushed, move upwards,
			// i.e leave x the same and increase y by 1
			move_display_cursor(0, 1);
		}
		
		// check for if "A, a, S, s, D, d, W, w" pressed
		// There are two steps to this
		// 1) collect any serial input (if available)
		// 2) check if the input is equal to the character
		// 'A', 'a', 'S', 's', 'D', 'd', 'W', 'w'
		char serial_input_game_play = -1;
		if (serial_input_available()) {
			serial_input_game_play = fgetc(stdin);
		}
		// Check serial input and move the cursor
		if (serial_input_game_play == 's' || serial_input_game_play == 'S') {
			if (pause == 0) {
				move_display_cursor(0, -1);
			}
		} else if (serial_input_game_play == 'w' || serial_input_game_play == 'W') {
			if (pause == 0) {
				move_display_cursor(0, 1);
			}
		} else if (serial_input_game_play == 'a' || serial_input_game_play == 'A') {
			if (pause == 0) {
				move_display_cursor(-1, 0);
			}
		} else if (serial_input_game_play == 'd' || serial_input_game_play == 'D') {
			if (pause == 0) {
				move_display_cursor(1, 0);
			}
		} else if (serial_input_game_play == 'p' || serial_input_game_play == 'P') {
			pause_game();
			if (pause == 0) {
				pause = 1;
			} else {
				pause = 0;
			}
		}
		
		// a piece can be placed at the current location of the cursor when button B0 or
		//space bar are pressed
		// check if button 0 is pressed
		if (btn == BUTTON0_PUSHED && pause == 0) {
			piece_placement();
		}
		// check if space bar is pressed
		if (serial_input_game_play == ' ' && pause == 0) {
			piece_placement();
		}
		
		current_time = get_current_time();
		if(current_time >= last_flash_time + 500) {
			// 500ms (0.5 second) has passed since the last time we
			// flashed the cursor, so flash the cursor
			flash_cursor();
			
			// Update the most recent time the cursor was flashed
			last_flash_time = current_time;
		}
	}
	// We get here if the game is over.
}

void handle_game_over() {
	move_terminal_cursor(10,14);
	printf_P(PSTR("GAME OVER"));
	move_terminal_cursor(10,15);
	printf_P(PSTR("Press a button to start again"));
	
	while(button_pushed() == NO_BUTTON_PUSHED) {
		; // wait
	}
}
