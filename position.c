#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "piece.h"
#include "position.h"
#include "coord.h"


void PositionDebugPrint(Position* pos)
{

    for(int row = BOARD_SIZE-1; row >= 0; row--)
    {
        for(int file = 0; file < BOARD_SIZE; file++)
        {
            putchar(PieceFENChar(&(pos->position_grid[file][row])));

        }

        putchar('\n');
    }

    printf("%s to move\n", pos->color_playing == WHITE ? "White" : "Black");
}

Position CreatePositionFromFEN(char* FEN)
{
    Position newPos;
    int file = 0, row = BOARD_SIZE -1;

    // Position data
    for(; *FEN && !(row == 0 && file == BOARD_SIZE); FEN++)
    {
        // printf("file %d row %d FEN: %s\n", file, row, FEN);

        if(*FEN >= '1' && *FEN <= '8')
        {
            int empty = *FEN - '0';

            assert(file+empty <= BOARD_SIZE);

            for(unsigned int i = 0; i < empty; i++, file++)
                newPos.position_grid[file][row] = NOPIECE_LITERAL;
        }
        else if(*FEN == '/')
        {
            assert(file == BOARD_SIZE);

            row--;
            file = 0;
        }
        else
        {
            newPos.position_grid[file][row] = PieceFromFENChar(*FEN);
            file++;
        }

    }

    // skip space
    assert(row == 0 && file == BOARD_SIZE);

    assert(*FEN++ == ' ');

    // turn

    switch(*FEN++)
    {
        case 'w': newPos.color_playing = WHITE; break;
        case 'b': newPos.color_playing = BLACK; break;

        default: exit(-1);
    }



    return newPos;
}

Piece* getPieceAtCoord(Position* pos, Coord coord)
{
    return validCoord(coord) ? &(pos->position_grid[coord.file][coord.row]) : NULL;
}