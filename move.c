#include "move.h"
#include "coord.h"

static Coord DIAGONAL_DIRECTIONS[] = {{-1, -1}, {1, 1}, {-1, 1}, {1, -1}};
static Coord CROSS_DIRECTIONS[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
static Coord KNIGHT_MOVES[] = {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}};

#define COORD_ADD_SELF(a, b) a.file+=b.file, a.row+=b.row

#define LINE_MOVES(DIRSET) \
    for(int dir = 0; dir < 4; dir++) \
    { \
        Coord current = from;\
        COORD_ADD_SELF(current, DIRSET[dir]); \
        for(; validCoord(current); COORD_ADD_SELF(current, DIRSET[dir]))\
        { \
            Piece* pieceAtCurrent = getPieceAtCoord(pos, current); \
\
            /* Piece hit! If enemy, it can be a capture unless its the enemy king */ \
            if(pieceAtCurrent->type != NO_PIECE) \
            { \
\
                if(pieceAtCurrent->color != color && pieceAtCurrent->type != KING) \
                { \
                    DebugPrintCoord(current);\
                } \
\
\
                break; \
            } \
\
            /* Otherwise, we can make a regular move on square current */ \
            DebugPrintCoord(current); \
        } \
    }

void DiagonalMove(Position* pos, Coord from, PieceColor color)
{
    LINE_MOVES(DIAGONAL_DIRECTIONS)
}

void CrossMove(Position* pos, Coord from, PieceColor color)
{
    LINE_MOVES(CROSS_DIRECTIONS)
}


void KnightMove(Position* pos, Coord from, PieceColor color)
{
    for(int move=0; move < 8; move++)
    {
        Coord current = from;

        COORD_ADD_SELF(current, KNIGHT_MOVES[move]);

        if(!validCoord(current)) continue;

        Piece* pieceAtCurrent = getPieceAtCoord(pos, current);

        if(pieceAtCurrent->type == NO_PIECE || (pieceAtCurrent->color != color && pieceAtCurrent->type != KING))
        {
            DebugPrintCoord(current);
        }
    }
}

void PawnMove(Position* pos, Coord from, PieceColor color)
{
    int movingDirection = (color == WHITE) ? 1 : -1;
    CoordRow pawnStartingPosition = (color ==  WHITE) ? ROW_2 : ROW_7;
    Coord captures[] = {{-1, movingDirection}, {1, movingDirection}};

    Coord current = from;

    for(int i=0; i < 1 + (from.row == pawnStartingPosition); i++)
    {
        current.row += movingDirection;

        if(!validCoord(current)) break;

        Piece* pieceAtCurrent = getPieceAtCoord(pos, current);

        if(pieceAtCurrent->type != NO_PIECE) break;

        DebugPrintCoord(current);

    }


    for(int move=0; move < 2; move++)
    {
        current = from;
        COORD_ADD_SELF(current, captures[move]);

        if(!validCoord(current)) continue;

        Piece* pieceAtCurrent = getPieceAtCoord(pos, current);

        if(pieceAtCurrent->type != NO_PIECE && pieceAtCurrent->color != color && pieceAtCurrent->type != KING ||
           COORD_EQUALS(current, pos->en_passant) && pieceAtCurrent->type == NO_PIECE && color == pos->color_playing) //en passant
        {
            DebugPrintCoord(current);
        }
    }



}
void KingMove(Position* pos, Coord from, PieceColor color)
{
    Coord current;
    for(int file=-1; file <= 1; file++)
        for(int row=-1; row <= 1; row++)
        {
            if(file == 0 && row == 0) continue;

            current = (Coord){from.file + file, from.row + row};

            if(!validCoord(current)) continue;

            Piece* pieceAtCurrent = getPieceAtCoord(pos, current);

            if(pieceAtCurrent->type == NO_PIECE || pieceAtCurrent->color != color && pieceAtCurrent->type != KING)
            {
                DebugPrintCoord(current);
            }


        }
}
