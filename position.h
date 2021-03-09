
#ifndef CHEESENG_POSITION_H
#define CHEESENG_POSITION_H


#include <stdbool.h>

#include "coord.h"
#include "piece.h"
#include "movetypes.h"

#include "darray.def.h"



DarrayDeclarations(Coord)
DarrayDeclarations(Move)


#define BOARD_SIZE 8
#define MAX_PIECES 32
#define PLAYER_COUNT 2

typedef enum positionstate {NORMAL, CHECK, CHECKMATE, DRAW, INVALID} PositionState;

typedef struct posmeta
{
    Darray(Move) legalMoves;
    PositionState state;

    Coord kingPositions[2];



} PositionMetadata;

typedef struct position
{
    Piece position_grid[BOARD_SIZE][BOARD_SIZE];

    PieceColor color_playing;

    bool castling_rights[PLAYER_COUNT][2];

    Coord en_passant;

    int halfmoveClock, fullmoveNumber;

    PositionMetadata *metadata;

} Position;


void CreatePositionMetadata(Position *pos);
void FreeMetadata(PositionMetadata *meta);
void ClearPositionMetadata(Position *pos);


Piece getPieceAtCoord(const Position* pos, Coord coord);
void setPieceAtCoord(Position* pos, Coord coord, Piece piece);
Position CreatePositionFromFEN(const char* FEN);

int CoordsTargetingCoord(const Position* pos, Coord target, PieceColor color, MoveTypes castingTypes, Darray(Coord) *data);
PositionState getPositionState(const Position *pos);
bool isPositionLegal(const Position* pos);
bool isPositionPlayable(const Position *pos);
bool isInCheck(const Position *pos, PieceColor color);


void PositionDebugPrint(const Position* pos);
void CreateFENString(const Position *pos);


#endif //CHEESENG_POSITION_H
