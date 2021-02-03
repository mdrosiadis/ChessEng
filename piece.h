#ifndef CHEESENG_PIECE_H
#define CHEESENG_PIECE_H

#include <stdbool.h>


typedef enum PieceType{PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NO_PIECE} PieceType;
typedef enum Color{WHITE, BLACK, NO_COLOR} PieceColor;

typedef struct PieceMoveTypes
{
    bool diagonal: 1;
    bool cross   : 1;
    bool knight  : 1;
    bool pawn    : 1;
    bool king    : 1;
} PieceMoveTypes;

typedef struct PieceData
{
    char symbol;
    int value;
    PieceMoveTypes move_types;
} PieceData;



typedef struct Piece
{
    PieceType type;
    PieceColor color;
} Piece;

#define NOPIECE_LITERAL (Piece){NO_PIECE, NO_COLOR}

char PieceFENChar(Piece* p);
Piece PieceFromFENChar(char FENChar);

#endif //CHEESENG_PIECE_H
