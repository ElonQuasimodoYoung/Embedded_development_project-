/*
 * display.h
 *
 * Author: Luke Kamols
 */ 

#ifndef DISPLAY_H_
#define DISPLAY_H_

// display dimensions, these match the size of the board
#define WIDTH  8
#define HEIGHT 8

// positioning of the top left corner of the board on the terminal
#define TERMINAL_BOARD_X 45
#define TERMINAL_BOARD_Y 5

// object definitions
#define EMPTY_SQUARE    0
#define PLAYER_1		1
#define PLAYER_2		2
#define CURSOR			3
#define ILLEGAL_CURSOR  4

// terminal colour definitions
#define TERMINAL_COLOUR_EMPTY	         BG_BLACK
#define TERMINAL_COLOUR_P1		         BG_RED
#define TERMINAL_COLOUR_P2		         BG_GREEN
#define TERMINAL_COLOUR_CURSOR	         BG_YELLOW
#define TERMINAL_COLOUR_ILLEGAL_CURSOR   BG_BLUE

// initialise the display for the board, this creates the display
// for an empty board
void initialise_display(void);

// shows a starting display
void start_display(void);

// updates the colour at square (x, y) to be the colour
// of the object 'object'
// 'object' is expected to be EMPTY_SQUARE, PLAYER_1, PLAYER_2 or 
// CURSOR
void update_square_colour(uint8_t x, uint8_t y, uint8_t object);

#endif 