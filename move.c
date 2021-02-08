#include <stdio.h>
#include <string.h>

#include "piece.h"
#include "move.h"
#include "coord.h"

LListDefinitions(Move)

static Coord DIAGONAL_DIRECTIONS[] = {{-1, -1}, {1, 1}, {-1, 1}, {1, -1}};
static Coord CROSS_DIRECTIONS[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
static Coord KNIGHT_MOVES[] = {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}};

LList(Move) (* const MOVE_TYPE_FUNCTION_LOOKUP[N_MOVE_TYPES])(Position*, Coord, PieceColor) =
        {DiagonalMove, CrossMove, KnightMove, PawnMove, KingMove};

#define COORD_ADD_SELF(a, b) a.file+=b.file, a.row+=b.row

static Move CLEAR_MOVE = {.isCastling = NO_CASTLE, .algebraicNotation = "", .isCapture = false, .promotionType = NO_PIECE};

static Move SHORT_CASTLE_MOVE = {.isCastling = SHORT_CASTLE};
static Move LONG_CASTLE_MOVE  = {.isCastling = LONG_CASTLE };

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
            Move newMove = CLEAR_MOVE;\
            newMove.from = from;  newMove.to = current;\
            if(pieceAtCurrent->type != NO_PIECE) \
            { \
\
                if(pieceAtCurrent->color != color) \
                {\
                    newMove.isCapture = true; newMove.captureSquare = current;\
                    LListAppendData(Move)(&moves, newMove); \
                } \
\
\
                break; \
            } \
\
            /* Otherwise, we can make a regular move on square current */ \
            newMove.isCapture = false;\
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
            Move newMove = CLEAR_MOVE;
            newMove.from = from; newMove.to = current; newMove.isCapture = (pieceAtCurrent->type != NO_PIECE);
            newMove.captureSquare = current;
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

        Move newMove = CLEAR_MOVE;
        newMove.from = from; newMove.to = current;

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
            Move newMove = CLEAR_MOVE;
            newMove.from = from; newMove.to = current; newMove.isCapture = true; newMove.captureSquare = current;

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
        else if(coordEquals(current, pos->en_passant) && pieceAtCurrent->type == NO_PIECE && color == pos->color_playing) //en passant
        {
            Coord enPassantCaptureSquare = {pos->en_passant.file, pos->en_passant.row - movingDirection};

            Move newMove = CLEAR_MOVE;
            newMove.from = from; newMove.to = current; newMove.isCapture = true; newMove.captureSquare = enPassantCaptureSquare;
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
                Move newMove = CLEAR_MOVE;
                newMove.from = from; newMove.to = current; newMove.isCapture = (pieceAtCurrent->type != NO_PIECE);
                newMove.captureSquare = current;
                LListAppendData(Move)(&moves, newMove);
            }

        }

    return moves;
}

// Given that move can be made
bool isLegalMove(Position* pos, Move* move)
{
    Position new;
    if(move->isCastling != NO_CASTLE)
    {
        Coord kingP = {FILE_E, pos->color_playing == WHITE ? ROW_1 : ROW_8};
        Coord dir = {move->isCastling == SHORT_CASTLE ? 1 : -1};

        for(int i=0; i <= 2; i++, COORD_ADD_SELF(kingP, dir))
        {
            if(CoordsTargetingCoord(pos, kingP, OTHER_COLOR((pos->color_playing)), (MoveTypes){1,1,1,1,1}, NULL) != 0) return false;
        }
    }

    playMove(pos, move, &new);



    return isPositionLegal(&new);
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

    if(doesMoveExist(pos, &SHORT_CASTLE_MOVE)) LListAppendData(Move)(&allMoves, SHORT_CASTLE_MOVE);
    if(doesMoveExist(pos, &LONG_CASTLE_MOVE))  LListAppendData(Move)(&allMoves, LONG_CASTLE_MOVE);

    Move* move;

    LListFORPTR(Move, move, allMoves)
    {
        if(!isLegalMove(pos, move)) LList_Move_RemoveNode(&allMoves, node);
    }



    return allMoves;
}

void createMoveString(Position* pos, Move* move)
{
    if(move->isCastling != NO_CASTLE)
    {
        move->algebraicNotation[0] = 0;
        strcpy(move->algebraicNotation, move->isCastling == SHORT_CASTLE ? "0-0" : "0-0-0");

        return;
    }

    int stringIndex = 0;
    LListCreate(Coord, extraPiecesMovingThere);
    Piece* pieceMoving = getPieceAtCoord(pos, move->from);
    CoordsTargetingCoord(pos, move->to, pieceMoving->color, PIECE_DATA[pieceMoving->type].move_types, &extraPiecesMovingThere);

    bool specifyFile = false, specifyRow = false;



    if(pieceMoving->type != PAWN)
    {
        Coord *c;
        LListFORPTR(Coord, c, extraPiecesMovingThere)
        {
            if(coordEquals(move->from, *c) || (getPieceAtCoord(pos, *c)->type != pieceMoving->type)) continue;
            else if(move->from.row == c->row)   specifyFile = true;
            else if(move->from.file == c->file) specifyRow = true;
        }

        move->algebraicNotation[stringIndex++] = PIECE_DATA[pieceMoving->type].symbol;
    }
    else if(move->isCapture)
    {
        specifyFile = true;
    }

    if(specifyFile) move->algebraicNotation[stringIndex++] = CoordFileToChar(move->from.file);
    if(specifyRow)  move->algebraicNotation[stringIndex++] = CoordRowToChar(move->from.row);

    if(move->isCapture) move->algebraicNotation[stringIndex++] = 'x';

    move->algebraicNotation[stringIndex++] = CoordFileToChar(move->to.file);
    move->algebraicNotation[stringIndex++] = CoordRowToChar(move->to.row);

    if(move->promotionType != NO_PIECE)
    {
        move->algebraicNotation[stringIndex++] = '=';
        move->algebraicNotation[stringIndex++] = PIECE_DATA[move->promotionType].symbol;
    }

    move->algebraicNotation[stringIndex] = 0;

}

void playMove(const Position *pos, Move *move, Position *newPosition)
{
    *newPosition = *pos;

    if(move->isCastling != NO_CASTLE)
    {
        CoordFile rookFile = (move->isCastling == SHORT_CASTLE) ? FILE_H : FILE_A;
        CoordFile rookTargetFile = (move->isCastling == SHORT_CASTLE) ? FILE_F : FILE_D;
        CoordFile kingTargetFile = (move->isCastling == SHORT_CASTLE) ? FILE_G : FILE_C;
        CoordRow castlingRow = (pos->color_playing == WHITE) ? ROW_1 : ROW_8;

        Coord kingPos = {FILE_E, castlingRow};
        Coord rookPos = {rookFile, castlingRow};

        Coord newKingPos = {kingTargetFile, castlingRow};
        Coord newRookPos = {rookTargetFile, castlingRow};

        *getPieceAtCoord(newPosition, kingPos) = NO_PIECE_LITERAL;
        *getPieceAtCoord(newPosition, rookPos) = NO_PIECE_LITERAL;

        *getPieceAtCoord(newPosition, newKingPos) = (Piece){KING, newPosition->color_playing};
        *getPieceAtCoord(newPosition, newRookPos) = (Piece){ROOK, newPosition->color_playing};

        newPosition->castling_rights[newPosition->color_playing][SHORT_CASTLE] = false;
        newPosition->castling_rights[newPosition->color_playing][LONG_CASTLE]  = false;

    }
    else
    {
        Piece *capture = getPieceAtCoord(newPosition, move->captureSquare);
        Piece *from = getPieceAtCoord(newPosition, move->from);
        Piece *to = getPieceAtCoord(newPosition, move->to);


        *capture = NO_PIECE_LITERAL;
        *to = *from;
        *from = NO_PIECE_LITERAL;

        if(move->promotionType != NO_PIECE) to->type = move->promotionType;
    }

    newPosition->color_playing = OTHER_COLOR(newPosition->color_playing);
}

// Return if the move exist on the board. If it does, fill all the move data on *move
bool doesMoveExist(Position *pos, Move *move)
{

    if(move->isCastling != NO_CASTLE)
    {
        if(!(pos->castling_rights[pos->color_playing][move->isCastling])) return false;

        CoordFile rookFile = (move->isCastling == SHORT_CASTLE) ? FILE_H : FILE_A;
        CoordRow castlingRow = (pos->color_playing == WHITE) ? ROW_1 : ROW_8;

        Coord kingPos = {FILE_E, castlingRow};
        Coord rookPos = {rookFile, castlingRow};

        if(!(PieceEquals(*getPieceAtCoord(pos, kingPos), (Piece){KING, pos->color_playing}) &&
             PieceEquals(*getPieceAtCoord(pos, rookPos), (Piece){ROOK, pos->color_playing}))) return false;

        Coord start = (move->isCastling == SHORT_CASTLE) ? kingPos : rookPos;
        Coord stop  = (move->isCastling == SHORT_CASTLE) ? rookPos : kingPos;

        Coord direction = {1, 0};
        for(COORD_ADD_SELF(start, direction); !coordEquals(start, stop); COORD_ADD_SELF(start, direction))
        {
            if(!PieceEquals(*getPieceAtCoord(pos, start), NO_PIECE_LITERAL)) return false;
        }

        return true;
    }

    Piece* pieceAtFrom = getPieceAtCoord(pos, move->from);

    if(pieceAtFrom->color != pos->color_playing) return false;

    bool moveFound = false;
    for(MoveType type = 0; !moveFound && type < N_MOVE_TYPES; type++)
    {
        LList(Move) moves = MOVE_TYPE_FUNCTION_LOOKUP[type](pos, move->from, pos->color_playing);
        Move *cur;
        LListFORPTR(Move, cur, moves)
        {
            if(coordEquals(cur->to, move->to))
            {
                // Get the move created by the engine (all data)
                *move = *cur;
                moveFound = true;
                break;
            }
        }

        LListFreeNodes(Move)(&moves);
    }

    return moveFound;
}


void DebugPrintMove(Move* move)
{

    if(move->isCastling == NO_CASTLE)
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

    }

    printf(" (%s)", move->algebraicNotation);

    printf("\n");
}