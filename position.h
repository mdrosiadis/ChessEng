
#ifndef CHEESENG_POSITION_H
#define CHEESENG_POSITION_H


#include <stdbool.h>

#include "coord.h"
#include "piece.h"

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

    int halfmoveClock, fullmoveNumber;

} Position;

typedef enum positionstate {NORMAL, CHECK, CHECKMATE, DRAW, INVALID} PositionState;



Piece getPieceAtCoord(const Position* pos, Coord coord);
void setPieceAtCoord(Position* pos, Coord coord, Piece piece);
Position CreatePositionFromFEN(const char* FEN);

int CoordsTargetingCoord(const Position* pos, Coord target, PieceColor color, MoveTypes castingTypes, LList(Coord) *data);
PositionState getPositionState(const Position *pos);
bool isPositionLegal(const Position* pos);
bool isPositionPlayable(const Position *pos);
bool isInCheck(const Position *pos, PieceColor color);


void PositionDebugPrint(const Position* pos);


#endif //CHEESENG_POSITION_H
