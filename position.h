#ifndef CHEESENG_POSITION_H
#define CHEESENG_POSITION_H

#include <stdbool.h>

#include "piece.h"
#include "coord.h"

#define BOARD_SIZE 8
#define MAX_PIECES 32
#define PLAYER_COUNT 2



typedef struct position
{
    Piece position_grid[BOARD_SIZE][BOARD_SIZE];

    PieceColor color_playing;

    bool castling_rights[PLAYER_COUNT][2];

    Coord en_passant;

} Position;

Piece* getPieceAtCoord(Position* pos, Coord coord);

Position CreatePositionFromFEN(char* FEN);

void PositionDebugPrint(Position* pos);


#endif //CHEESENG_POSITION_H
