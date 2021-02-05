#ifndef CHEESENG_POSITION_H
#define CHEESENG_POSITION_H

#include <stdbool.h>

#include "piece.h"
#include "coord.h"
#include "linkedlist.def.h"

LListDeclarations(Coord)


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
//bool makeMove(Position* pos, Move* move);
LList(Coord) CoordsTargetingCoord(Position* pos, Coord target, PieceColor color);
bool isPositionLegal(Position* pos);
void getLegalMoves(Position* pos);

void PositionDebugPrint(Position* pos);


#endif //CHEESENG_POSITION_H
