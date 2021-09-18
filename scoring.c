/*
 * Scoring.c
 *
 * Created: 2021/5/22 14:01:06
 *  Author: Donghao Yang
 */ 

#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>

#include "terminalio.h"
#include "display.h"
#include "game.h"


// scores of two players
uint8_t redScore;
uint8_t greenScore;

void init_score(void) {	
	
	// At the beginning of game, every players have two piece on board
	redScore = 2;
	greenScore = 2;
	
	// display scores of two players
	move_terminal_cursor(2, 2);
	printf("Red Score:%6d", (int)redScore);
	move_terminal_cursor(2, 3);
	printf("Green score:%4d", (int)greenScore);
}

void piece_placement_score(uint8_t player) {
	if (player == PLAYER_1) { // when player 1 places a piece successfully
		redScore += 1;
		move_terminal_cursor(2, 2);
		printf("Red Score:%6d", (int)redScore);
		move_terminal_cursor(2, 3);
		printf("Green score:%4d", (int)greenScore);
	} else if (player == PLAYER_2) { // when player 2 places a piece successfully
		greenScore += 1;
		move_terminal_cursor(2, 2);
		printf("Red Score:%6d", (int)redScore);
		move_terminal_cursor(2, 3);
		printf("Green score:%4d", (int)greenScore);
	}
	
}

void flip_piece_score(uint8_t player){
	if (player == PLAYER_1) {
		redScore++;
		greenScore--;
	} else {
		redScore--;
		greenScore++;
	}
}

uint8_t get_score(void) {
	if (get_current_player() == PLAYER_1) {
		return redScore;
	} else {
		return greenScore;
	}
}