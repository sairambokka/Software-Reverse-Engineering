/**
 * @file game.h
 * @brief Header file for Tic Tac Toe functionality
 */

#ifndef GAME_H
#define GAME_H

/**
 * A structure that stores the state of the game
 */
typedef struct StructBoard {
    /** The player who turn it is */
    int turn;
    /** The number of columns in the cell array */
    int col_count;
    /** The number of rows in the cell array */
    int row_count;
    /** @brief The cells in row-major order.
     * Each byte is an ASCII character, either 'X', 'O', ' '
     */
    char cells[3][3];
} StructBoard;

// Define the move descriptions array
extern char* move_descs[];
/**
 * @brief Allocate and initialize a game board
 * 
 * It is a caller's responsibility to free this when finished
 * @return a pointer to the new game board
 */
StructBoard* game_init(void);

/**
 * @brief A callback to receive information about an available play
 * 
 * The caller to #game_enumerate_plays passes a function pointer to receive
 * each enumerated play. If provided, the user data is passed back into the 
 * user parameter
 * 
 * @param play_id the 0-up index of the play (corresponds to column number)
 * @param desc a user-readable description of the play from move_descs array
 * @param user a pointer to caller-provided context
 */
typedef void (*PrintPlay) (int play_id, char* desc, void* user);

/**
 * @brief Enumerate all valid moves available on the current board
 * 
 * Iterates through each column of the board and checks if a piece can be placed
 * (i.e., if the top position is empty). For each valid column, calls the provided
 * callback function with:
 * - The column number as play_id
 * - A description of the move from move_descs
 * - The user-provided context pointer
 * 
 * @param board a pointer to the game board
 * @param print_play callback function to receive information about each valid move
 * @param user_data pointer passed to the callback function for context
 * 
 * @note Moves are only valid in columns that are not full (have a space ' ' at the top)
 * @note play_id corresponds directly to the column number (0 to col_count-1)
 * @note This function expects an external move_descs array containing move descriptions
 */
void game_enumerate_plays(StructBoard* board, PrintPlay print_play, void* user_data);

/**
 * @brief Attempt to place a piece in the specified column and update game state
 * 
 * Places the current player's piece ('X' or 'O') in the lowest empty position
 * in the specified column. After a successful play, checks for a winner and
 * updates the turn to the next player if the game continues.
 * 
 * @param board pointer to the game board
 * @param play_id the column number where the piece should be placed (0 to col_count-1)
 * 
 * @return int Returns one of the following:
 *      - ASCII value of winner ('X' or 'O') if the move results in a win
 *      - -1: Invalid move (column full or out of bounds)
 *      - 0: Move successful, game continues
 *      - -2: Move results in a draw
 *      - '?': Error in turn management
 * 
 * @note After a successful move that doesn't end the game, switches turn from 'X' to 'O' or vice versa
 * @note Checks from bottom up for the first empty space in the chosen column
 * @note Invalid moves include: negative column numbers, column >= col_count, or full column
 */
int game_play(StructBoard* board, int play_id);

/**
 * @brief Check if there is a winner or draw on the current board
 * 
 * Examines the board for winning conditions by checking:
 * - Horizontal sequences of 4 matching pieces (left to right in each row)
 * - Vertical sequences of 4 matching pieces (bottom to top in each column)
 * - Diagonal sequences of 4 matching pieces:
 *   * Rising diagonals (bottom-left to top-right)
 *   * Falling diagonals (top-left to bottom-right)
 * - Draw condition when the board is full
 * 
 * @param board pointer to the game board
 * @return int Returns one of the following:
 *      - ASCII value of winning player's symbol ('X' or 'O'): indicates winner
 *      - 0: game is still in progress (empty spaces exist)
 *      - -2: game is a draw (board full with no winner)
 * 
 * @note Checks are done in the following order: horizontal, vertical, diagonals, draw
 * @note A winning sequence must contain exactly 4 matching pieces
 * @note Empty spaces are represented by ' ' character
 * @note Function terminates and returns as soon as any winning condition is found
 */
int check_winner(StructBoard* board);

#endif /* GAME_H */