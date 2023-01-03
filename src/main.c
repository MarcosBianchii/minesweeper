#include "minesweeper.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
        if (argc < 3) {
                printf(GREEN"> "WHITE"./minesweeper "GREEN"<board-size> <bombs>\n"WHITE);
                return 0;
        }

        srand(time(NULL));

        gameInit(atoi(argv[1]), atoi(argv[2]));
        gameLoop();
        gameDestroy();
        
        return 0;
}