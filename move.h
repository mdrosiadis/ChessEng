#ifndef CHEESENG_MOVE_H
#define CHEESENG_MOVE_H

#include <stdbool.h>

#include "coord.h"
#include "position.h"


#include "darray.def.h"

#include "movetypes.h"


//typedef enum castlingmoves{SHORT_CASTLE, LONG_CASTLE, NO_CASTLE} CastlingMove;

//
//typedef struct move
//{
//    Coord from;
//    Coord to;
//
//    CastlingMove castlingType;
//    PieceType promotionType;
//
//    char algebraicNotation[MAX_ALGEBRAIC_NOTATION_STRING];
//} Move;




extern Darray(Move) (* const MOVE_TYPE_FUNCTION_LOOKUP[N_MOVE_TYPES])(const Position*, Coord, PieceColor);

Darray(Move) DiagonalMove(const Position* pos, Coord from, PieceColor color);
Darray(Move) CrossMove(const Position* pos, Coord from, PieceColor color);
Darray(Move) KnightMove(const Position* pos, Coord from, PieceColor color);
Darray(Move) PawnMove(const Position* pos, Coord from, PieceColor color);
Darray(Move) KingMove(const Position* pos, Coord from, PieceColor color);

Darray(Move) MovesFromSquare(const Position *pos, Coord square);

bool isLegalMove(const Position* pos, Move* move);
Darray(Move) createLegalMoves(Position* pos);
Darray(Move) getLegalMoves(Position* pos);

bool doesMoveExist(const Position *pos, Move *move);
void playMove(const Position *pos, Move *move, Position *newPosition);

bool CreateMoveFromUCI(const char *uci, Move *move);

void createMoveString(const Position* pos, Move* move);
void DebugPrintMove(const Move* move);

#endif //CHEESENG_MOVE_H
