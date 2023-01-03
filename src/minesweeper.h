#ifndef __MINESWEEPER_H__
#define __MINESWEEPER_H__

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ncurses.h>

#define BLACK  "\033[0;30m"
#define RED    "\033[0;31m"
#define GREEN  "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE   "\033[0;34m"
#define PURPLE "\033[0;35m"
#define CYAN   "\033[0;36m"
#define WHITE  "\033[0;37m"

#define BOMB_EMOJI    "\U0001f4a3"
#define BOOM_EMOJI    "\U0001f4a5"
#define FLAG_EMOJI    "\U0001f6a9"
#define SMILE_EMOJI   "\U0001f642"
#define GLASSES_EMOJI "\U0001f60e"
#define SAD_EMOJI     "\U0001f628"

typedef enum GameState {
        PLAYING, WON, LOST
} game_state_t;

typedef enum CellKind {
        EMPTY, ONE, TWO, THREE,
        FOUR, FIVE, SIX, SEVEN,
        EIGHT, BOMB, BOOM
} cell_kind_t;

typedef enum CellState {
        CLOSED, OPEN, FLAGGED
} cell_state_t;

typedef struct Cell {
        cell_kind_t kind;
        cell_state_t state;
} cell_t;

typedef struct Pos {
        short row;
        short col;
} pos_t;

typedef struct Info {
        int bombs;
        int flags;
} info_t;

typedef struct Game {
        cell_t **board;
        info_t info;
        pos_t cursor;
        game_state_t state;
        short size;
} game_t;

int  gameInit(int, int);
bool gameLoop();
void gameDestroy();

#endif // __MINESWEEPER_H__