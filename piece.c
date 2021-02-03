#include "piece.h"

#include <ctype.h>

const char PIECE_SYMBOLS[] = {'P', 'N', 'B', 'R', 'Q', 'K', '.'};

const PieceData PIECE_DATA[] =
{
    {'P', 1, {0, 0, 0, 1, 0}},
    {'N', 3, {0, 0, 1, 0, 0}},
    {'B', 1, {1, 0, 0, 0, 0}},
    {'R', 5, {0, 1, 0, 0, 0}},
    {'Q', 9, {1, 1, 0, 0, 0}},
    {'K', 0, {0, 0, 0, 0, 1}},
    {'.', 0, {0, 0, 0, 0, 0}}
};


char PieceFENChar(Piece* p)
{
    return p->color == WHITE ? PIECE_SYMBOLS[p->type] : tolower(PIECE_SYMBOLS[p->type]);
}

Piece PieceFromFENChar(char FENChar)
{

    char p = toupper(FENChar);
    for(int i = 0; i < sizeof PIECE_SYMBOLS / sizeof (char); i++)
    {
        if(p == PIECE_SYMBOLS[i])
            return (Piece){i,  isupper(FENChar) ? WHITE : BLACK};
    }

    return  NOPIECE_LITERAL;
}

