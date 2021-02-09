#ifndef CHEESENG_MOVE_H
#define CHEESENG_MOVE_H

#include <stdbool.h>

#include "coord.h"
#include "position.h"

#include "linkedlist.def.h"

#define MAX_ALGEBRAIC_NOTATION_STRING 11

typedef enum castlingmoves{SHORT_CASTLE, LONG_CASTLE, NO_CASTLE} CastlingMove;


typedef struct move
{
    Coord from;
    Coord to;

    CastlingMove isCastling;

    char algebraicNotation[MAX_ALGEBRAIC_NOTATION_STRING];

    // easy way around en passant
    bool isCapture;
    Coord captureSquare;

    PieceType promotionType;
} Move;



LListDeclarations(Move)

extern LList(Move) (* const MOVE_TYPE_FUNCTION_LOOKUP[N_MOVE_TYPES])(const Position*, Coord, PieceColor);

LList(Move) DiagonalMove(const Position* pos, Coord from, PieceColor color);
LList(Move) CrossMove(const Position* pos, Coord from, PieceColor color);
LList(Move) KnightMove(const Position* pos, Coord from, PieceColor color);
LList(Move) PawnMove(const Position* pos, Coord from, PieceColor color);
LList(Move) KingMove(const Position* pos, Coord from, PieceColor color);

bool isLegalMove(const Position* pos, Move* move);
LList(Move) getLegalMoves(const Position* pos);

bool doesMoveExist(const Position *pos, Move *move);
void playMove(const Position *pos, Move *move, Position *newPosition);

bool CreateMoveFromUCI(const char *uci, Move *move);

void createMoveString(const Position* pos, Move* move);
void DebugPrintMove(const Move* move);

#endif //CHEESENG_MOVE_H
