/*
 * game.c
 *
 * Contains functions relating to the play of the game Reversi
 *
 * Author: Luke Kamols
 */ 

#include <avr/io.h>
#include <stdlib.h>

#include "game.h"
#include "display.h"
#include "scoring.h"

#define CURSOR_X_START 5
#define CURSOR_Y_START 3

#define START_PIECES 2
static const uint8_t p1_start_pieces[START_PIECES][2] = { {3, 3}, {4, 4} };
static const uint8_t p2_start_pieces[START_PIECES][2] = { {3, 4}, {4, 3} };

uint8_t board[WIDTH][HEIGHT];
uint8_t cursor_x;
uint8_t cursor_y;
uint8_t cursor_visible;
uint8_t current_player;
uint8_t valid_direction[WIDTH][HEIGHT][8];
uint8_t no_move_game_over = 0;

void initialise_board(void) {
	
	// initialise the display we are using
	initialise_display();
	
	// initialise the board to be all empty
	for (uint8_t x = 0; x < WIDTH; x++) {
		for (uint8_t y = 0; y < HEIGHT; y++) {
			board[x][y] = EMPTY_SQUARE;
		}
	}
	
	// now load in the starting pieces for player 1
	for (uint8_t i = 0; i < START_PIECES; i++) {
		uint8_t x = p1_start_pieces[i][0];
		uint8_t y = p1_start_pieces[i][1];
		board[x][y] = PLAYER_1; // place in array
		update_square_colour(x, y, PLAYER_1); // show on board
	}
	
	// and for player 2
	for (uint8_t i = 0; i < START_PIECES; i++) {
		uint8_t x = p2_start_pieces[i][0];
		uint8_t y = p2_start_pieces[i][1];
		board[x][y] = PLAYER_2;
		update_square_colour(x, y, PLAYER_2);		
	}
	
	// set the starting player
	current_player = PLAYER_1;
	
	// also set where the cursor starts
	cursor_x = CURSOR_X_START;
	cursor_y = CURSOR_Y_START;
	cursor_visible = 0;
	// the game start from player 1 with red piece
	DDRB |= (1 << DDB0) | (1 << DDB1);
	PORTB |= 1 << PORTB0;
	PORTB &= ~(1 << PORTB1);
}

uint8_t get_piece_at(uint8_t x, uint8_t y) {
	// check the bounds, anything outside the bounds
	// will be considered empty
	if (x < 0 || x >= WIDTH || y < 0 || y >= WIDTH) {
		return EMPTY_SQUARE;
	} else {
		//if in the bounds, just index into the array
		return board[x][y];
	}
}

void flash_cursor(void) {
	
	if (cursor_visible) {
		// we need to flash the cursor off, it should be replaced by
		// the colour of the piece which is at that location
		uint8_t piece_at_cursor = get_piece_at(cursor_x, cursor_y);
		update_square_colour(cursor_x, cursor_y, piece_at_cursor);
		
	} else {
		// we need to flash the cursor on
		if (check_valid_place(cursor_x, cursor_y)) {
			update_square_colour(cursor_x, cursor_y, CURSOR);
		} else {
			update_square_colour(cursor_x, cursor_y, ILLEGAL_CURSOR);
		}
	}
	cursor_visible = 1 - cursor_visible; //alternate between 0 and 1
}

//check the header file game.h for a description of what this function should do
// (it may contain some hints as to how to move the pieces)
void move_display_cursor(uint8_t dx, uint8_t dy) {
	//YOUR CODE HERE
	// We need to flash the cursor off, it should be replaced by 
	// the colour of the piece which is at that location
	cursor_visible = 1;
	flash_cursor();
	// Update the position of the cursor
	cursor_x = (cursor_x + dx) % WIDTH;
	cursor_y = (cursor_y + dy) % HEIGHT;
	// Display the cursor at the new location
	cursor_visible = 0;
	flash_cursor();
	/*suggestions for implementation:
	 * 1: remove the display of the cursor at the current location
	 *		(and replace it with whatever piece is at that location)
	 * 2: update the positional knowledge of the cursor, this will include
	 *		variables cursor_x, cursor_y and cursor_visible
	 * 3: display the cursor at the new location
	 */
}

uint8_t distance(uint8_t x, uint8_t y, uint8_t xx, uint8_t yy) {
	uint8_t dx = abs(x - xx);
	uint8_t dy = abs(y - yy);
	if (dx > 0) {
		return dx;
	} else {
		return dy;
	}
}

uint8_t check_valid_place(uint8_t x, uint8_t y) {
	// if the direction is legal direction, 0 is illegal, 1 is legal
	uint8_t is_valid = 0;
	uint8_t next_player;
	if (current_player == PLAYER_1) {
		next_player = PLAYER_2;
	} else {
		next_player = PLAYER_1;
	}
	
	// check the up direction
	uint8_t x_up = x;
	uint8_t y_up = y + 1;
	while (y_up <= 7 && get_piece_at(x_up, y_up) == next_player) {
		y_up += 1;
	}
	if (y_up <= 7 && distance(x, y, x_up, y_up) > 1 && board[x_up][y_up] == current_player) {
		valid_direction[x][y][0] = 1;
		is_valid = 1;
	}
	
	// check the up-right direction
	uint8_t x_up_right = x + 1;
	uint8_t y_up_right = y + 1;
	while (x_up_right <= 7 && y_up_right <= 7 && get_piece_at(x_up_right, y_up_right) == next_player) {
		x_up_right += 1;
		y_up_right += 1;
	}
	if (x_up_right <= 7 && y_up_right <= 7 && distance(x, y, x_up_right, y_up_right) > 1 && board[x_up_right][y_up_right] == current_player) {
		valid_direction[x][y][1] = 1;
		is_valid = 1;
	}
	
	// check the right direction
	uint8_t x_right = x + 1;
	uint8_t y_right = y;
	while (x_right <= 7 && get_piece_at(x_right, y_right) == next_player) {
		x_right += 1;
	}
	if (x_right <= 7 && distance(x, y, x_right, y_right) > 1 && board[x_right][y_right] == current_player) {
		valid_direction[x][y][2] = 1;
		is_valid = 1;
	}
	
	// check the down-right direction 
	uint8_t x_down_right = x + 1;
	uint8_t y_down_right = y - 1;
	while(x_down_right <= 7 && y_down_right >= 0 && get_piece_at(x_down_right, y_down_right) == next_player) {
		x_down_right += 1;
		y_down_right -= 1;
	}
	if (x_down_right <= 7 && y_down_right >= 0 && distance(x, y, x_down_right, y_down_right) > 1 && board[x_down_right][y_down_right] == current_player) {
		valid_direction[x][y][3] = 1;
		is_valid = 1;
	}
	
	// check the down direction 
	uint8_t x_down = x;
	uint8_t y_down = y - 1;
	while (y_down >= 0 && get_piece_at(x_down, y_down) == next_player) {
		y_down -= 1;
	}
	if (y_down >= 0 && distance(x, y, x_down, y_down) > 1 && board[x_down][y_down] == current_player) {
		valid_direction[x][y][4] = 1;
		is_valid = 1;
	}
	
	// check the down-left direction
	uint8_t x_down_left = x - 1;
	uint8_t y_down_left = y - 1;
	while (x_down_left >= 0 && y_down_left >= 0 && get_piece_at(x_down_left, y_down_left) == next_player) {
		x_down_left -= 1;
		y_down_left -= 1;
	}
	if (x_down_left >= 0 && y_down_left >= 0 && distance(x, y, x_down_left, y_down_left) > 1 && board[x_down_left][y_down_left] == current_player) {
		valid_direction[x][y][5] = 1;
		is_valid = 1;
	}
	
	// check the left direction
	uint8_t x_left = x - 1;
	uint8_t y_left = y;
	while (x_left >= 0 && get_piece_at(x_left, y_left) == next_player) {
		x_left -= 1;
	}
	if (x_left >= 0 && distance(x, y, x_left, y_left) > 1 && board[x_left][y_left] == current_player) {
		valid_direction[x][y][6] = 1;
		is_valid = 1;
	}
	
	// check the up-left direction
	uint8_t x_up_left = x - 1;
	uint8_t y_up_left = y + 1;
	while (x_up_left >= 0 && y_up_left <= 7 && get_piece_at(x_up_left, y_up_left) == next_player) {
		x_up_left -= 1;
		y_up_left += 1;
	}
	if (x_up_left >= 0 && y_up_left <= 7 && distance(x, y, x_up_left, y_up_left) > 1 && board[x_up_left][y_up_left] == current_player) {
		valid_direction[x][y][7] = 1;
		is_valid = 1;
	}
	return is_valid;
}

void flip_piece(uint8_t x, uint8_t y) {
	uint8_t next_player_for_flip;
	if (current_player == PLAYER_1) {
		next_player_for_flip = PLAYER_2;
	} else {
		next_player_for_flip = PLAYER_1;
	}
	
	uint8_t x_for_flip;
	uint8_t y_for_flip;
	
	// flip up direction
	if (valid_direction[x][y][0]) {
		x_for_flip = x;
		y_for_flip = y + 1;
		while (board[x_for_flip][y_for_flip] == next_player_for_flip) {
			board[x_for_flip][y_for_flip] = current_player;
			update_square_colour(x_for_flip, y_for_flip, board[x_for_flip][y_for_flip]);
			flip_piece_score(board[x][y]);
			y_for_flip++;
		}
	}
	
	// flip the up-right
	if (valid_direction[x][y][1]) {
		x_for_flip = x + 1;
		y_for_flip = y + 1;
		while (board[x_for_flip][y_for_flip] == next_player_for_flip) {
			board[x_for_flip][y_for_flip] = current_player;
			update_square_colour(x_for_flip, y_for_flip, board[x_for_flip][y_for_flip]);
			flip_piece_score(board[x][y]);
			x_for_flip++;
			y_for_flip++;
		}
	}
	
	// flip the right direction
	if (valid_direction[x][y][2]) {
		x_for_flip = x + 1;
		y_for_flip = y;
		while (board[x_for_flip][y_for_flip] == next_player_for_flip) {
			board[x_for_flip][y_for_flip] = current_player;
			update_square_colour(x_for_flip, y_for_flip, board[x_for_flip][y_for_flip]);
			flip_piece_score(board[x][y]);
			x_for_flip++;
		}
	}
	
	// flip the down-right direction
	if (valid_direction[x][y][3]) {
		x_for_flip = x + 1;
		y_for_flip = y - 1;
		while (board[x_for_flip][y_for_flip] == next_player_for_flip) {
			board[x_for_flip][y_for_flip] = current_player;
			update_square_colour(x_for_flip, y_for_flip, board[x_for_flip][y_for_flip]);
			flip_piece_score(board[x][y]);
			x_for_flip++;
			y_for_flip--;
		}
	}
	
	// flip the down direction
	if (valid_direction[x][y][4]) {
		x_for_flip = x;
		y_for_flip = y - 1;
		while (board[x_for_flip][y_for_flip] == next_player_for_flip) {
			board[x_for_flip][y_for_flip] = current_player;
			update_square_colour(x_for_flip, y_for_flip, board[x_for_flip][y_for_flip]);
			flip_piece_score(board[x][y]);
			y_for_flip--;
		}
	}
	
	// flip the down-left direction
	if (valid_direction[x][y][5]) {
		x_for_flip = x - 1;
		y_for_flip = y - 1;
		while (board[x_for_flip][y_for_flip] == next_player_for_flip) {
			board[x_for_flip][y_for_flip] = current_player;
			update_square_colour(x_for_flip, y_for_flip, board[x_for_flip][y_for_flip]);
			flip_piece_score(board[x][y]);
			x_for_flip--;
			y_for_flip--;
		}
	}
	
	// flip the left direction
	if (valid_direction[x][y][6]) {
		x_for_flip = x - 1;
		y_for_flip = y;
		while (board[x_for_flip][y_for_flip] == next_player_for_flip) {
			board[x_for_flip][y_for_flip] = current_player;
			update_square_colour(x_for_flip, y_for_flip, board[x_for_flip][y_for_flip]);
			flip_piece_score(board[x][y]);
			x_for_flip--;
		}
	}
	
	// flip up-left direction
	if (valid_direction[x][y][7]) {
		x_for_flip = x - 1;
		y_for_flip = y + 1;
		while(board[x_for_flip][y_for_flip] == next_player_for_flip) {
			board[x_for_flip][y_for_flip] = current_player;
			update_square_colour(x_for_flip, y_for_flip, board[x_for_flip][y_for_flip]);
			flip_piece_score(board[x][y]);
			x_for_flip--;
			y_for_flip++;
		}
	}
}

void piece_placement(void) {
	// check if the current position of cursor is empty
	if (get_piece_at(cursor_x, cursor_y) == EMPTY_SQUARE && check_valid_place(cursor_x, cursor_y)){
		
		board[cursor_x][cursor_y] = current_player;
		flip_piece(cursor_x, cursor_y);
		piece_placement_score(board[cursor_x][cursor_y]);
		// place the correct piece of each player
		update_square_colour(cursor_x, cursor_y, board[cursor_x][cursor_y]);
		
		
		// display the turn of players by LED
		if (current_player == PLAYER_1) {
			PORTB |= 1 << PORTB1;
			PORTB &= ~(1 << PORTB0);
			} else if (current_player == PLAYER_2) {
			PORTB |= 1 << PORTB0;
			PORTB &= ~(1 << PORTB1);
		}
		
		// switch play turn of players
		if (current_player == PLAYER_1) {
			current_player = PLAYER_2;
		} else {
			current_player = PLAYER_1;
		}
		
		// check if there is available move for players, if not, game over
		if (!check_available_move(current_player)) {
			if (current_player == PLAYER_1) {
				current_player = PLAYER_2;
				PORTB |= 1 << PORTB1;
				PORTB &= ~(1 << PORTB0);
				if (! check_available_move(current_player)) {
					no_move_game_over = 1;
				} 
			} else {
				current_player = PLAYER_1;
				PORTB |= 1 << PORTB0;
				PORTB &= ~(1 << PORTB1);
				if (! check_available_move(current_player)) {
					no_move_game_over = 1;
				}
			}
		} 
	}
}

uint8_t check_available_move(uint8_t player) {
	// check every place of board
	for (int i = 0; i <= 7; i++) {
		for (int j = 0; j <= 7; j++) {
			if (get_piece_at(i, j) == EMPTY_SQUARE && check_valid_place(i, j)) {
				return 1;
			}
		}
	}
	return 0;
}

uint8_t no_available_move_game_over(void) {
	return no_move_game_over;
}

uint8_t is_game_over(void) {
	// The game ends when every single square is filled
	// Check for any squares that are empty
	for (uint8_t x = 0; x < WIDTH; x++) {
		for (uint8_t y = 0; y < HEIGHT; y++) {
			if (board[x][y] == EMPTY_SQUARE) {
				// there was an empty square, game is not over
				return 0;
			}
		}
	}
	// if we don't clear this variable, the game can't restart normally.
	no_move_game_over = 0;
	// every single position has been checked and no empty squares were found
	// the game is over
	return 1;
}

uint8_t get_current_player(void) {
	if (current_player == PLAYER_1) {
		return PLAYER_1;
	} else {
		return PLAYER_2;
	}
}