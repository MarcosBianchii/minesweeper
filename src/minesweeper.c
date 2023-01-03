#include "minesweeper.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ncurses.h>

game_t *game = NULL;
char input = '.';

pos_t randPos(int size)
{
        pos_t pos;
        pos.row = rand() % size;
        pos.col = rand() % size;
        return pos;
}

bool findPos(pos_t *poses, pos_t pos, int n)
{
        for (int i = 0; i < n; i++)
                if (poses[i].row == pos.row
                &&  poses[i].col == pos.col)
                        return true;
        return false;
}

pos_t *posesInit(int size, int quan)
{
        pos_t *poses = malloc(sizeof(int) * quan);
        if (!poses) return NULL;

        for (int i = 0; i < quan; i++) {
                do poses[i] = randPos(size);
                while (findPos(poses, poses[i], i));
        }

        return poses;
}

void placeNum(cell_t **board, int x, int y)
{
        if (x > 0) {
                if (y > 0)
                        if (board[x-1][y-1].kind != BOMB)
                                board[x-1][y-1].kind++;
                if (board[x-1][y].kind != BOMB)
                        board[x-1][y].kind++;
                if (y < game->size-1)
                        if (board[x-1][y+1].kind != BOMB)
                                board[x-1][y+1].kind++;
        }
        
        if (y > 0)
                if (board[x][y-1].kind != BOMB)
                        board[x][y-1].kind++;
        if (y < game->size-1)
                if (board[x][y+1].kind != BOMB)
                        board[x][y+1].kind++;
        
        if (x < game->size-1) {
                if (y > 0)
                        if (board[x+1][y-1].kind != BOMB)
                                board[x+1][y-1].kind++;
                if (board[x+1][y].kind != BOMB)
                        board[x+1][y].kind++;
                if (y < game->size-1)
                        if (board[x+1][y+1].kind != BOMB)
                                board[x+1][y+1].kind++;
        }
}


void numInit(cell_t **board, int size)
{
        for (int i = 0; i < size; i++)
                for (int j = 0; j < size; j++)
                        if (board[i][j].kind == BOMB)
                                placeNum(board, i, j);
}

int boardInit(cell_t **board, int size, int quan)
{
        for (int i = 0; i < size; i++)
                board[i] = calloc(size, sizeof(cell_t));
        pos_t *poses = posesInit(size, quan);
        if (!poses) return -1;

        for (int i = 0; i < quan; i++)
                game->board[poses[i].row][poses[i].col].kind = BOMB;
        free(poses);

        numInit(board, size);
        return 0;
}

void assignCursor(int size)
{
        short n = size/2;
        if (game->board[n][n].kind == EMPTY) {
                game->cursor.row = n;
                game->cursor.col = n;
                return;
        }
        
        pos_t pos;
        do  pos = randPos(size);
        while (game->board[pos.row][pos.col].kind != EMPTY);
        game->cursor = pos;
}

int gameInit(int size, int quan)
{
        short max = size*size-1;

        if (size < 4)   size = 4;
        if (size > 99)  size = 99;
        if (quan < 1)   quan = size;
        if (quan > max) quan = max;

        game = malloc(sizeof(game_t));
        if (!game) return -1;

        game->board = malloc(sizeof(void *) * size);
        if (!game->board) return -1;

        game->state = PLAYING;
        game->size = size;
        game->info.bombs = quan;
        game->info.flags = 0;

        boardInit(game->board, size, quan);
        assignCursor(size);
        return 0;
}

void gameDestroy()
{
        if (!game) return;

        for (int i = 0; i < game->size; i++)
                free(game->board[i]);
        free(game->board);
        free(game);
}

void initPairs()
{
        init_pair(1, COLOR_BLUE,    -1);
        init_pair(2, COLOR_GREEN,   -1);
        init_pair(3, COLOR_RED,     -1);
        init_pair(4, COLOR_YELLOW,  -1);
        init_pair(5, COLOR_MAGENTA, -1);
        init_pair(6, COLOR_CYAN,    -1);
        init_pair(7, COLOR_BLACK,   -1);
        init_pair(8, COLOR_WHITE,   -1);
}

void printBoard()
{
        erase();
        addstr("\n");
        short bombs_left = game->info.bombs - game->info.flags;
        for (int i = 0; i < game->size-5; i++) {
                addstr(" ");
                if (i == game->size/3-1) {
                        attron(COLOR_PAIR(3));
                        if (bombs_left < 0){
                                bombs_left *= -1;
                                addstr("|-");
                        } else addstr(" |");
                        if (bombs_left < 100) addch('0');
                        if (bombs_left < 10)  addch('0');
                        printw("%i|", bombs_left);
                        attron(COLOR_PAIR(7));
                }
        }
        
        attron(COLOR_PAIR(8));
        switch (game->state) {
        case WON:
                attron(COLOR_PAIR(2));
                addstr("B)\n");
                break;
        case LOST:
                attron(COLOR_PAIR(3));
                addstr(";(\n");
                break;
        case PLAYING:
                attron(COLOR_PAIR(4));
                addstr(":)\n");
                break;
        }

        attron(COLOR_PAIR(8));
        addstr("\n");
        for (int i = 0; i < game->size; i++) {
                printw("| ");
                for (int j = 0; j < game->size; j++) {
                        if (game->board[i][j].state == FLAGGED)
                                attron(COLOR_PAIR(4));
                        else attron(COLOR_PAIR(7));
                        if (i == game->cursor.row && j == game->cursor.col)
                                attron(COLOR_PAIR(8));
                        switch (game->board[i][j].state) {
                        case CLOSED:
                                addstr("# ");
                                break;
                        case FLAGGED:
                                addstr("F ");
                                break;
                        case OPEN:
                                if (game->board[i][j].kind <= 8)
                                        attron(COLOR_PAIR(game->board[i][j].kind));
                                if (game->board[i][j].kind == 9)
                                        addstr("O ");
                                if (game->board[i][j].kind == 10) {
                                        attron(COLOR_PAIR(3));
                                        addstr("X ");
                                }

                                if (i == game->cursor.row && j == game->cursor.col)
                                        attron(COLOR_PAIR(8));
                                if (game->board[i][j].kind != BOMB
                                &&  game->board[i][j].kind != BOOM) {
                                        if (game->board[i][j].kind == EMPTY
                                        && !(i == game->cursor.row && j == game->cursor.col))
                                                addstr("  ");
                                        else printw("%i ", game->board[i][j].kind);
                                }

                                break;
                        }

                        attron(COLOR_PAIR(8));
                }

                addstr("|\n");
        }

        attroff(COLOR_PAIR(8));
        wnoutrefresh(stdscr);
        doupdate();
}

void showBoard()
{
        for (int i = 0; i < game->size; i++)
                for (int j = 0; j < game->size; j++) {
                        if (game->board[i][j].state == FLAGGED)
                                if (game->board[i][j].kind == BOMB)
                                        continue;
                        game->board[i][j].state = OPEN;
                }
}

bool isNum(short row, short col)
{
        if (game->board[row][col].kind != EMPTY
        &&  game->board[row][col].kind != BOMB)
                return true;
        return false;
}

void openSides(cell_t **board, short row, short col)
{
        if (row > 0) {
                if (col > 0)
                        if (isNum(row-1, col-1))
                                board[row-1][col-1].state = OPEN;
                if (isNum(row-1, col))
                        board[row-1][col].state = OPEN;
                if (col < game->size-1)
                        if (isNum(row-1, col+1))
                                board[row-1][col+1].state = OPEN;
        }
        
        if (col > 0)
                if (isNum(row, col-1))
                        board[row][col-1].state = OPEN;
        if (col < game->size-1)
                if (isNum(row, col+1))
                        board[row][col+1].state = OPEN;
        
        if (row < game->size-1) {
                if (col > 0)
                        if (isNum(row+1, col-1))
                                board[row+1][col-1].state = OPEN;
                if (isNum(row+1, col))
                        board[row+1][col].state = OPEN;
                if (col < game->size-1)
                        if (isNum(row+1, col+1))
                                board[row+1][col+1].state = OPEN;
        }
}

void openCells(cell_t **board, short row, short col)
{
        if (row < 0 || row > game->size-1)
                return;
        if (col < 0 || col > game->size-1)
                return;

        if (board[row][col].kind != EMPTY
        ||  board[row][col].state == OPEN)
                return;

        board[row][col].state = OPEN;
        openSides(board, row, col);

        openCells(board, row, col-1);
        openCells(board, row+1, col);
        openCells(board, row, col+1);
        openCells(board, row-1, col);
}

bool play()
{
        switch (input) {
        case 'w':
                game->cursor.row--;
                break;
        case 'a':
                game->cursor.col--;
                break;
        case 's':
                game->cursor.row++;
                break;
        case 'd':
                game->cursor.col++;
                break;
        }

        short size = game->size;

        if (game->cursor.row < 0) game->cursor.row = size-1;
        if (game->cursor.col < 0) game->cursor.col = size-1; 
        if (game->cursor.row >= size) game->cursor.row = 0;
        if (game->cursor.col >= size) game->cursor.col = 0;

        short row = game->cursor.row;
        short col = game->cursor.col;
        short state = game->board[row][col].state;

        if (input == 'q' && state != OPEN) {
                game->board[row][col].state = state == FLAGGED ? CLOSED : FLAGGED;
                game->info.flags = state == FLAGGED ? game->info.flags-1 : game->info.flags+1; 
        }
        
        if (input == 'e' && state == CLOSED) {
                if (game->board[row][col].kind == BOMB){
                        game->board[row][col].kind = BOOM;
                        game->state = LOST;
                        showBoard();
                        return false;
                }

                if (game->board[row][col].kind == EMPTY)
                        openCells(game->board, row, col);
                else game->board[row][col].state = OPEN;
        }
        
        if (input == 'z') {
                game->state = WON;
                showBoard();
                return false;
        }

        return true;
}

bool checkWin()
{
        if (game->info.bombs != game->info.flags)
                return false;

        for (int i = 0; i < game->size; i++)
                for (int j = 0; j < game->size; j++)
                        if (game->board[i][j].kind == BOMB
                        &&  game->board[i][j].state != FLAGGED)
                                return false;
        showBoard();
        game->state = WON;
        return true;
}

bool gameLoop()
{
        if (!game) return false;

        initscr();
        noecho();
        raw();
        start_color();
        use_default_colors();
        curs_set(0);
        initPairs();

        bool ret = false;
        while (game->state == PLAYING) {
                printBoard();
                input = getch();
                if (!play())
                        break;
                if (checkWin())
                        ret = true;
        }

        printBoard();
        getch();
        endwin();
        return ret;
}