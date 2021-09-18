/*
 * Scoring.h
 *
 * Created: 2021/5/22 14:04:48
 *  Author: 15972
 */ 


#ifndef SCORING_H_
#define SCORING_H_

// initialise scores of two players
void init_score(void);

// change the score if a player place a piece successfully
void piece_placement_score(uint8_t player);

// change the score if a player flips another player's piece
void flip_piece_score(uint8_t player);

// return current score of players 
uint8_t get_score(void);


#endif /* SCORING_H_ */