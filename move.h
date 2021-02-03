#ifndef CHEESENG_MOVE_H
#define CHEESENG_MOVE_H

#include <stdbool.h>

#include "position.h"
#include "coord.h"
#include "piece.h"

#define MAX_ALGREBRAIC_NOTATION_STRING 11

typedef struct move
{
    Coord from;
    Coord to;

    char algebraicNotation[MAX_ALGREBRAIC_NOTATION_STRING];

    // easy way around en passant
    bool isCapture;
    Coord captureSquare;
} Move;



void DiagonalMove(Position* pos, Coord from, PieceColor color);
void CrossMove(Position* pos, Coord from, PieceColor color);
void KnightMove(Position* pos, Coord from, PieceColor color);
void PawnMove(Position* pos, Coord from, PieceColor color);
void KingMove(Position* pos, Coord from, PieceColor color);


#endif //CHEESENG_MOVE_H
