/*
** game.h
**
** Written by Luke Kamols
**
** Function prototypes for those functions available externally
*/

#ifndef GAME_H_
#define GAME_H_

#include <inttypes.h>

// initialise the display of the board, this creates the internal board
// and also updates the display of the board
void initialise_board(void);

// returns which piece is located at position (x,y)
// the value returned will be SQUARE_EMPTY, SQUARE_P1 or SQUARE_P2
// anything outside the bounds of the boards will be SQUARE_EMPTY
uint8_t get_piece_at(uint8_t x, uint8_t y);

// update the cursor display, by changing whether it is visible or not
// call this function at regular intervals to have the cursor flash
void flash_cursor(void);

// moves the position of the cursor by (dx, dy) such that if the cursor
// started at (cursor_x, cursor_y) then after this function is called, 
// it should end at ( (cursor_x + dx) % WIDTH, (cursor_y + dy) % HEIGHT)
// the cursor should be displayed after it is moved as well
void move_display_cursor(uint8_t dx, uint8_t dy);

// get the distance between two valid piece
uint8_t distance(uint8_t x, uint8_t y, uint8_t xx, uint8_t yy);

// check if the piece placement is valid
uint8_t check_valid_place(uint8_t x, uint8_t y);

// flip piece in terms of legal move
void flip_piece(uint8_t x, uint8_t y);

// A piece can be placed at the current location of the cursor when button B0
// or space bar is pressed
void piece_placement(void);

// check if existing available move
uint8_t check_available_move(uint8_t player);

// game over if no available move
uint8_t no_available_move_game_over(void);

// returns 1 if the game is over, 0 otherwise
uint8_t is_game_over(void);

// return the current player
uint8_t get_current_player(void);

#endif

