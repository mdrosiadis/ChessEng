#ifndef CHEESENG_MOVE_H
#define CHEESENG_MOVE_H

#include <stdbool.h>

#include "coord.h"
#include "position.h"

#include "linkedlist.def.h"

#define MAX_ALGEBRAIC_NOTATION_STRING 11


typedef struct move
{
    Coord from;
    Coord to;

    char algebraicNotation[MAX_ALGEBRAIC_NOTATION_STRING];

    // easy way around en passant
    bool isCapture;
    Coord captureSquare;

    PieceType promotionType;
} Move;



LListDeclarations(Move)

extern LList(Move) (* const MOVE_TYPE_FUNCTION_LOOKUP[N_MOVE_TYPES])(Position*, Coord, PieceColor);

LList(Move) DiagonalMove(Position* pos, Coord from, PieceColor color);
LList(Move) CrossMove(Position* pos, Coord from, PieceColor color);
LList(Move) KnightMove(Position* pos, Coord from, PieceColor color);
LList(Move) PawnMove(Position* pos, Coord from, PieceColor color);
LList(Move) KingMove(Position* pos, Coord from, PieceColor color);


LList(Move) getLegalMoves(Position* pos);
void DebugPrintMove(Move* move);

#endif //CHEESENG_MOVE_H
