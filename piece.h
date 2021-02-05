#ifndef CHEESENG_PIECE_H
#define CHEESENG_PIECE_H

#include <stdbool.h>


typedef enum PieceType{PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NO_PIECE} PieceType;
typedef enum PieceColor{WHITE, BLACK, NO_COLOR} PieceColor;

#define OTHER_COLOR(color) ((color == WHITE) ? BLACK : WHITE)


typedef struct PieceData
{
    char symbol;
    int value;
    bool moveTypes[5];
} PieceData;


typedef struct Piece
{
    PieceType type;
    PieceColor color;
} Piece;

#define NO_PIECE_LITERAL (Piece){NO_PIECE, NO_COLOR}

extern const PieceData PIECE_DATA[];

char PieceFENChar(Piece* p);
Piece PieceFromFENChar(char FENChar);

#endif //CHEESENG_PIECE_H
