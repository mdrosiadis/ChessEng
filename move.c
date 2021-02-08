#include <stdio.h>

#include "piece.h"
#include "move.h"

LListDefinitions(Move)

static Coord DIAGONAL_DIRECTIONS[] = {{-1, -1}, {1, 1}, {-1, 1}, {1, -1}};
static Coord CROSS_DIRECTIONS[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
static Coord KNIGHT_MOVES[] = {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}};

LList(Move) (* const MOVE_TYPE_FUNCTION_LOOKUP[N_MOVE_TYPES])(Position*, Coord, PieceColor) =
        {DiagonalMove, CrossMove, KnightMove, PawnMove, KingMove};

#define COORD_ADD_SELF(a, b) a.file+=b.file, a.row+=b.row
// temporary
#define DebugPrintCoordFull(h) (h)


#define LINE_MOVES(DIRSET) \
    LListCreate(Move, moves);\
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
                if(pieceAtCurrent->color != color) \
                { \
                    DebugPrintCoordFull(current); \
                    Move newMove = {.from = from, .to = current, .isCapture = true, .captureSquare = current, .promotionType = NO_PIECE};\
                    LListAppendData(Move)(&moves, newMove); \
                } \
\
\
                break; \
            } \
\
            /* Otherwise, we can make a regular move on square current */ \
            DebugPrintCoordFull(current);\
            Move newMove = {.from = from, .to = current, .isCapture = false, .promotionType = NO_PIECE};\
            LListAppendData(Move)(&moves, newMove); \
        } \
    } \
    return moves;

LList(Move) DiagonalMove(Position* pos, Coord from, PieceColor color)
{
    LINE_MOVES(DIAGONAL_DIRECTIONS)
}

LList(Move) CrossMove(Position* pos, Coord from, PieceColor color)
{
    LINE_MOVES(CROSS_DIRECTIONS)
}


LList(Move) KnightMove(Position* pos, Coord from, PieceColor color)
{
    LListCreate(Move, moves);
    for(int move=0; move < 8; move++)
    {
        Coord current = from;

        COORD_ADD_SELF(current, KNIGHT_MOVES[move]);

        if(!validCoord(current)) continue;

        Piece* pieceAtCurrent = getPieceAtCoord(pos, current);

        if(pieceAtCurrent->type == NO_PIECE || pieceAtCurrent->color != color)
        {
            DebugPrintCoordFull(current);
            Move newMove = {.from = from, .to = current, .isCapture = (pieceAtCurrent->type != NO_PIECE), .captureSquare = current, .promotionType = NO_PIECE};
            LListAppendData(Move)(&moves, newMove);
        }
    }

    return moves;
}

LList(Move) PawnMove(Position* pos, Coord from, PieceColor color)
{
    LListCreate(Move, moves);
    int movingDirection = (color == WHITE) ? 1 : -1;
    CoordRow pawnStartingPosition = (color ==  WHITE) ? ROW_2 : ROW_7;
    CoordRow promotionRow = (color ==  WHITE) ? ROW_8 : ROW_1;
    Coord captures[] = {{-1, movingDirection}, {1, movingDirection}};

    Coord current = from;

    for(int i=0; i < 1 + (from.row == pawnStartingPosition); i++)
    {
        current.row += movingDirection;

        if(!validCoord(current)) break;

        Piece* pieceAtCurrent = getPieceAtCoord(pos, current);

        if(pieceAtCurrent->type != NO_PIECE) break;

        DebugPrintCoordFull(current);
        Move newMove = {.from = from, .to = current, .isCapture = false,  .promotionType = NO_PIECE};

        if(current.row == promotionRow)
        {
            for(PieceType type = KNIGHT; type <= QUEEN; type++)
            {
                newMove.promotionType = type;
                LListAppendData(Move)(&moves, newMove);
            }
        }
        else
        {
            LListAppendData(Move)(&moves, newMove);
        }

    }


    for(int move=0; move < 2; move++)
    {
        current = from;
        COORD_ADD_SELF(current, captures[move]);

        if(!validCoord(current)) continue;

        Piece* pieceAtCurrent = getPieceAtCoord(pos, current);

        if(pieceAtCurrent->type != NO_PIECE && pieceAtCurrent->color != color)
        {
            DebugPrintCoordFull(current);
            Move newMove = {.from = from, .to = current, .isCapture = true, .captureSquare = current, .promotionType = NO_PIECE};

            if(current.row == promotionRow)
            {
                for(PieceType type = KNIGHT; type <= QUEEN; type++)
                {
                    newMove.promotionType = type;
                    LListAppendData(Move)(&moves, newMove);
                }
            }
            else
            {
                LListAppendData(Move)(&moves, newMove);
            }

        }
        else if(COORD_EQUALS(current, pos->en_passant) && pieceAtCurrent->type == NO_PIECE && color == pos->color_playing) //en passant
        {
            Coord enPassantCaptureSquare = {pos->en_passant.file, pos->en_passant.row - movingDirection};
            DebugPrintCoordFull(current);
            Move newMove = {.from = from, .to = current, .isCapture = true, .captureSquare = enPassantCaptureSquare, .promotionType = NO_PIECE};
            LListAppendData(Move)(&moves, newMove);
        }
    }

    return moves;
}


LList(Move) KingMove(Position* pos, Coord from, PieceColor color)
{
    LListCreate(Move, moves);
    Coord current;
    for(int file=-1; file <= 1; file++)
        for(int row=-1; row <= 1; row++)
        {
            if(file == 0 && row == 0) continue;

            current = (Coord){from.file + file, from.row + row};

            if(!validCoord(current)) continue;

            Piece* pieceAtCurrent = getPieceAtCoord(pos, current);

            if(pieceAtCurrent->type == NO_PIECE || pieceAtCurrent->color != color)
            {
                DebugPrintCoordFull(current);
                Move newMove = {.from = from, .to = current, .isCapture = (pieceAtCurrent->type != NO_PIECE), .captureSquare = current, .promotionType = NO_PIECE};
                LListAppendData(Move)(&moves, newMove);
            }

        }

    return moves;
}

// Given that move can be made
bool isLegalMove(Position* pos, Move* move)
{
    Piece *capture = getPieceAtCoord(pos, move->captureSquare);
    Piece *from = getPieceAtCoord(pos, move->from);
    Piece *to = getPieceAtCoord(pos, move->to);

    Piece captureHold = *capture;
    Piece pieceMoving = *from;

    pos->color_playing = OTHER_COLOR(pos->color_playing);
    *capture = NO_PIECE_LITERAL;
    *to = *from;
    *from = NO_PIECE_LITERAL;

    if(move->promotionType != NO_PIECE) to->type = move->promotionType;

    bool legal = isPositionLegal(pos);

    pos->color_playing = OTHER_COLOR(pos->color_playing);
    *from = pieceMoving;
    *to = NO_PIECE_LITERAL;
    *capture = captureHold;

    return legal;
}

LList(Move) getLegalMoves(Position* pos)
{
    LListCreate(Move, allMoves);
    Coord current;

    for(current.file = FILE_A; current.file <= FILE_H; current.file++) {
        for (current.row = ROW_1; current.row <= ROW_8; current.row++)
        {
            Piece *pieceAtCurrent = getPieceAtCoord(pos, current);

            if (pieceAtCurrent->color != pos->color_playing) continue;

            for (MoveType type = 0; type < N_MOVE_TYPES; type++) {
                if (!PIECE_DATA[pieceAtCurrent->type].move_types.types[type]) continue;

                LListExtend(Move)(&allMoves, MOVE_TYPE_FUNCTION_LOOKUP[type](pos, current, pos->color_playing));
            }
        }
    }

    Move* move;

    LListFORPTR(Move, move, allMoves)
    {
        if(!isLegalMove(pos, move)) LList_Move_RemoveNode(&allMoves, node);
    }



    return allMoves;
}

void DebugPrintMove(Move* move)
{
    PrintCoordAlgebraic(move->from);
    printf(" - ");
    PrintCoordAlgebraic(move->to);

    if(move->isCapture)
    {
        printf(" x at ");
        PrintCoordAlgebraic(move->captureSquare);
    }

    if(move->promotionType != NO_PIECE)
    {
        printf(" = %c", PIECE_DATA[move->promotionType].symbol);

    }

    printf("\n");
}