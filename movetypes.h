#ifndef CHEESENG_MOVETYPES_H
#define CHEESENG_MOVETYPES_H

#include "coord.h"
#include "piece.h"

#define N_MOVE_TYPES 5
#define MAX_ALGEBRAIC_NOTATION_STRING 11

typedef struct movetypes
{
    bool types[N_MOVE_TYPES];
} MoveTypes;

typedef enum castlingmoves{SHORT_CASTLE, LONG_CASTLE, NO_CASTLE} CastlingMove;

typedef struct move
{
    Coord from;
    Coord to;

    CastlingMove castlingType;
    PieceType promotionType;

    char algebraicNotation[MAX_ALGEBRAIC_NOTATION_STRING];
} Move;

typedef enum move_type{MOVE_DIAGONAL, MOVE_CROSS, MOVE_KNIGHT, MOVE_PAWN, MOVE_KING} MoveType;

#endif //CHEESENG_MOVETYPES_H
