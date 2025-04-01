#include <stdio.h>
#include <stdlib.h>
#include "tictactoe.h"
#include <string.h>




char* move_descs[] = {
	"Top Left",
	"Top Middle",
	"Top Right",
	"Middle Left",
	"Middle Center",
	"Middle Right",
	"Bottom Left",
	"Bottom Middle",
	"Bottom Right"
};

StructBoard* game_init(void) {
	StructBoard* board = (StructBoard*)malloc(sizeof(StructBoard));
	
	board->row_count = 3;
	board->col_count = 3;
	board->turn = 'X';
	
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			board->cells[i][j] = ' ';
		}
	}
	
	return board;
}


void game_enumerate_plays(StructBoard* board, PrintPlay print_play, void* user) {
	int playNum = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (board->cells[i][j] == ' ') {
				print_play(playNum, move_descs[playNum], user);
			}
			playNum = playNum + 1;
		}
	}
}



/**
 * Mapping of play_ids to board cells:
 * 1 -> cells[0][0]
 * 2 -> cells[0][1]
 * 3 -> cells[0][2]
 * 4 -> cells[1][0]
 * 5 -> cells[1][1]
 * 6 -> cells[1][2]
 * 7 -> cells[2][0]
 * 8 -> cells[2][1]
 * 9 -> cells[2][2]
 * 
 * 
*/


int game_play(StructBoard* board, int play_id) {
	
	
	
	switch (play_id+1) {
		case 1: 
			if (board->cells[0][0] == ' ') {
				board->cells[0][0] = board->turn;
				break;
			} else {
				return -1;
			}
		case 2:
			if (board->cells[0][1] == ' ') {
				board->cells[0][1] = board->turn;
				break;
			} else {
				return -1;
			}
		case 3:
			if (board->cells[0][2] == ' ') {
				board->cells[0][2] = board->turn;
				break;
			} else {
				return -1;
			}
		case 4:
			if (board->cells[1][0] == ' ') {
				board->cells[1][0] = board->turn;
				break;
			} else {
				return -1;
			}
		case 5:
			if (board->cells[1][1] == ' ') {
				board->cells[1][1] = board->turn;
				break;
			} else {
				return -1;
			}
		case 6: 
			if (board->cells[1][2] == ' ') {
				board->cells[1][2] = board->turn;
				break;
			} else {
				return -1;
			}
		case 7: 
			if (board->cells[2][0] == ' ') {
				board->cells[2][0] = board->turn;
				break;
			} else {
				return -1;
			}
		case 8:
			if (board->cells[2][1] = ' ') {
				board->cells[2][1] = board->turn;
				break;
			} else {
				return -1;
			}
		case 9:
			if (board->cells[2][2] == ' ') {
				board->cells[2][2] = board->turn;
				break;
			} else {
				return -1;
			}
		default:
			return -1;
		}
			
		int result = check_winner(board);
		if (result != 0) return result;
		
		board->turn = (board->turn == 'X') ? 'O' : 'X';
		return 0;
}

int check_winner(StructBoard* board) {
	
	// check winner in rows
	for (int i = 0; i < 3; i++) {
		if (board->cells[i][0] != ' ' &&
			board->cells[i][0] == board->cells[i][1] &&
			board->cells[i][1] == board->cells[i][2]) {
				return board->turn;
		}
	}
	
	
	// check winner in columns
	for (int i = 0; i < 3; i++) {
		if (board->cells[0][i] != ' ' &&
			board->cells[0][i] == board->cells[1][i] &&
			board->cells[1][i] == board->cells[2][i]) {
				return board->turn;
		}
	}
	
	//check winner in diagonals
	
	if (board->cells[0][0] != ' ' &&
		board->cells[0][0] == board->cells[1][1] &&
		board->cells[1][1] == board->cells[2][2]) {
			return board->turn;
	}
	
	if (board->cells[0][2] != ' ' &&
		board->cells[0][2] == board->cells[1][1] &&
		board->cells[1][1] == board->cells[2][0]) {
			return board->turn;
	}
	
	//check draw
	int fullBoardFlag = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (board->cells[i][j] == ' ') {
				fullBoardFlag = 1;
			}
		}
	}
	
	return (fullBoardFlag = 0) ? -2 : 0;
}
