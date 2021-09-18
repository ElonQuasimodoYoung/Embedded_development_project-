/*
 * display.c
 *
 * Author: Luke Kamols
 */ 

#include <stdio.h>
#include <avr/pgmspace.h>

#include "display.h"
#include "terminalio.h"

void initialise_display(void) {
	// first turn off the cursor
	hide_cursor();

	// next build an empty board
	set_display_attribute(FG_YELLOW);
	for (uint8_t row = 0; row < HEIGHT; row++) {
		move_terminal_cursor(TERMINAL_BOARD_X, TERMINAL_BOARD_Y+2*row);
		printf_P(PSTR("+--+--+--+--+--+--+--+--+"));
		move_terminal_cursor(TERMINAL_BOARD_X, TERMINAL_BOARD_Y+2*row+1);
		printf_P(PSTR("|  |  |  |  |  |  |  |  |"));
	}
	move_terminal_cursor(TERMINAL_BOARD_X, TERMINAL_BOARD_Y+2*HEIGHT);
	printf_P(PSTR("+--+--+--+--+--+--+--+--+"));

	// clear the colour settings so we don't print other things in yellow
	normal_display_mode();
}

void start_display(void) {
	move_terminal_cursor(TERMINAL_BOARD_X, TERMINAL_BOARD_Y);
	set_display_attribute(FG_GREEN);
	printf_P(PSTR("REVERSI"));
}

void update_square_colour(uint8_t x, uint8_t y, uint8_t object) {
	// determine which colour corresponds to this object
	DisplayParameter backgroundColour;
	if (object == PLAYER_1) {
		backgroundColour = TERMINAL_COLOUR_P1;
		} else if (object == PLAYER_2) {
		backgroundColour = TERMINAL_COLOUR_P2;
		} else if (object == CURSOR) {
		backgroundColour = TERMINAL_COLOUR_CURSOR;
		} else if (object == ILLEGAL_CURSOR) {
		backgroundColour = TERMINAL_COLOUR_ILLEGAL_CURSOR;	
		} else {
		// anything unexpected will be black
		backgroundColour = TERMINAL_COLOUR_EMPTY;
	}

	// set the background colour to be the determined colour
	// when we enter a space this will appear as a solid block
	set_display_attribute(backgroundColour);

	// update the square at the given location
	// note that two spaces form roughly a square so that is used instead of one space
	// also note that the terminal y values count from the top
	// but our referencing counts from the bottom, so the y position is inverted
	move_terminal_cursor(TERMINAL_BOARD_X + 1 + 3 * x,
	TERMINAL_BOARD_Y + 1 + 2 * (HEIGHT - y - 1));
	printf_P(PSTR("  ")); // print two spaces, since we set the background colour

	normal_display_mode(); // remove the display attribute
}