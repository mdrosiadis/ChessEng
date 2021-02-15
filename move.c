#include <stdio.h>
#include <string.h>

#include "piece.h"
#include "move.h"
#include "coord.h"

LListDefinitions(Move)

static const Coord DIAGONAL_DIRECTIONS[] = {{-1, -1}, {1, 1}, {-1, 1}, {1, -1}};
static const Coord CROSS_DIRECTIONS[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
static const Coord KNIGHT_MOVES[] = {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}};

static const Coord CASTLING_KING_START_COORD[2] = {{FILE_E, ROW_1}, {FILE_E, ROW_8}};
static const Coord CASTLING_ROOK_START_COORD[2][2] = {{{FILE_H, ROW_1}, {FILE_A, ROW_1}}, {{FILE_H, ROW_8}, {FILE_A, ROW_8}}};

static const Coord CASTLING_KING_TARGET_COORD[2][2] = {{{FILE_G, ROW_1}, {FILE_C, ROW_1}}, {{FILE_G, ROW_8}, {FILE_C, ROW_8}}};
static const Coord CASTLING_ROOK_TARGET_COORD[2][2] = {{{FILE_F, ROW_1}, {FILE_D, ROW_1}}, {{FILE_F, ROW_8}, {FILE_D, ROW_8}}};

static const CoordRow PAWN_STARTING_ROW[2]  = {ROW_2, ROW_7};
static const CoordRow PAWN_PROMOTION_ROW[2] = {ROW_8, ROW_1};

static const int PAWN_MOVING_DIRECTION[2] = {1, -1};

LList(Move) (* const MOVE_TYPE_FUNCTION_LOOKUP[N_MOVE_TYPES])(const Position*, Coord, PieceColor) =
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
            Piece pieceAtCurrent = getPieceAtCoord(pos, current); \
\
            /* Piece hit! If enemy, it can be a capture unless its the enemy king */ \
            Move newMove = CLEAR_MOVE;\
            newMove.from = from;  newMove.to = current;\
            if(pieceAtCurrent.type != NO_PIECE) \
            { \
\
                if(pieceAtCurrent.color != color) \
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

LList(Move) DiagonalMove(const Position* pos, Coord from, PieceColor color)
{
    LINE_MOVES(DIAGONAL_DIRECTIONS)
}

LList(Move) CrossMove(const Position* pos, Coord from, PieceColor color)
{
    LINE_MOVES(CROSS_DIRECTIONS)
}


LList(Move) KnightMove(const Position* pos, Coord from, PieceColor color)
{
    LListCreate(Move, moves);
    for(int move=0; move < 8; move++)
    {
        Coord current = from;

        COORD_ADD_SELF(current, KNIGHT_MOVES[move]);

        if(!validCoord(current)) continue;

        Piece pieceAtCurrent = getPieceAtCoord(pos, current);

        if(pieceAtCurrent.type == NO_PIECE || pieceAtCurrent.color != color)
        {
            Move newMove = CLEAR_MOVE;
            newMove.from = from; newMove.to = current; newMove.isCapture = (pieceAtCurrent.type != NO_PIECE);
            newMove.captureSquare = current;
            LListAppendData(Move)(&moves, newMove);
        }
    }

    return moves;
}

LList(Move) PawnMove(const Position* pos, Coord from, PieceColor color)
{
    LListCreate(Move, moves);

    Coord captures[] = {{-1, PAWN_MOVING_DIRECTION[color]}, {1, PAWN_MOVING_DIRECTION[color]}};

    Coord current = from;

    for(int i=0; i < 1 + (from.row == PAWN_STARTING_ROW[color]); i++)
    {
        current.row += PAWN_MOVING_DIRECTION[color];

        if(!validCoord(current)) break;

        Piece pieceAtCurrent = getPieceAtCoord(pos, current);

        if(pieceAtCurrent.type != NO_PIECE) break;

        Move newMove = CLEAR_MOVE;
        newMove.from = from; newMove.to = current;

        if(current.row == PAWN_PROMOTION_ROW[color])
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

        Piece pieceAtCurrent = getPieceAtCoord(pos, current);

        if(pieceAtCurrent.type != NO_PIECE && pieceAtCurrent.color != color)
        {
            Move newMove = CLEAR_MOVE;
            newMove.from = from; newMove.to = current; newMove.isCapture = true; newMove.captureSquare = current;

            if(current.row == PAWN_PROMOTION_ROW[color])
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
        else if(coordEquals(current, pos->en_passant) && pieceAtCurrent.type == NO_PIECE && color == pos->color_playing) //en passant
        {
            Coord enPassantCaptureSquare = {pos->en_passant.file, pos->en_passant.row - PAWN_MOVING_DIRECTION[color]};

            Move newMove = CLEAR_MOVE;
            newMove.from = from; newMove.to = current; newMove.isCapture = true; newMove.captureSquare = enPassantCaptureSquare;
            LListAppendData(Move)(&moves, newMove);
        }
    }

    return moves;
}


LList(Move) KingMove(const Position* pos, Coord from, PieceColor color)
{
    LListCreate(Move, moves);
    Coord current;
    for(int file=-1; file <= 1; file++)
        for(int row=-1; row <= 1; row++)
        {
            if(file == 0 && row == 0) continue;

            current = (Coord){from.file + file, from.row + row};

            if(!validCoord(current)) continue;

            Piece pieceAtCurrent = getPieceAtCoord(pos, current);

            if(pieceAtCurrent.type == NO_PIECE || pieceAtCurrent.color != color)
            {
                Move newMove = CLEAR_MOVE;
                newMove.from = from; newMove.to = current; newMove.isCapture = (pieceAtCurrent.type != NO_PIECE);
                newMove.captureSquare = current;
                LListAppendData(Move)(&moves, newMove);
            }

        }

    return moves;
}

// Given that move can be made
bool isLegalMove(const Position* pos, Move* move)
{
    Position new;
    if(move->isCastling != NO_CASTLE)
    {
        Coord cur = CASTLING_KING_START_COORD[pos->color_playing];
        Coord dir = {move->isCastling == SHORT_CASTLE ? 1 : -1};

        for(int i=0; i <= 2; i++, COORD_ADD_SELF(cur, dir))
        {
            if(CoordsTargetingCoord(pos, cur, OTHER_COLOR((pos->color_playing)), (MoveTypes){1,1,1,1,1}, NULL) != 0) return false;
        }
    }

    playMove(pos, move, &new);



    return isPositionLegal(&new);
}

LList(Move) getLegalMoves(const Position* pos)
{
    LListCreate(Move, allMoves);
    Coord current;

    for(current.file = FILE_A; current.file <= FILE_H; current.file++) {
        for (current.row = ROW_1; current.row <= ROW_8; current.row++)
        {
            Piece pieceAtCurrent = getPieceAtCoord(pos, current);

            if (pieceAtCurrent.color != pos->color_playing) continue;

            for (MoveType type = 0; type < N_MOVE_TYPES; type++) {
                if (!PIECE_DATA[pieceAtCurrent.type].move_types.types[type]) continue;

                LListExtend(Move)(&allMoves, MOVE_TYPE_FUNCTION_LOOKUP[type](pos, current, pos->color_playing));
            }
        }
    }

    if(doesMoveExist(pos, &SHORT_CASTLE_MOVE)) LListAppendData(Move)(&allMoves, SHORT_CASTLE_MOVE);
    if(doesMoveExist(pos, &LONG_CASTLE_MOVE))  LListAppendData(Move)(&allMoves, LONG_CASTLE_MOVE);

    LListCreate(Move, legalMoves);
    Move* move;

    LListFORPTR(Move, move, allMoves)
    {
        if(isLegalMove(pos, move)) LListAppendData(Move)(&legalMoves, *move);
    }

    LListFreeNodes(Move)(&allMoves);


    return legalMoves;
}

void createMoveString(const Position* pos, Move* move)
{
    if(move->isCastling != NO_CASTLE)
    {
        move->algebraicNotation[0] = 0;
        strcpy(move->algebraicNotation, move->isCastling == SHORT_CASTLE ? "0-0" : "0-0-0");

        return;
    }

    int stringIndex = 0;
    LListCreate(Coord, extraPiecesMovingThere);
    Piece pieceMoving = getPieceAtCoord(pos, move->from);
    CoordsTargetingCoord(pos, move->to, pieceMoving.color, PIECE_DATA[pieceMoving.type].move_types, &extraPiecesMovingThere);

    bool specifyFile = false, specifyRow = false;



    if(pieceMoving.type != PAWN)
    {
        Coord *c;
        LListFORPTR(Coord, c, extraPiecesMovingThere)
        {
            if(coordEquals(move->from, *c) || (getPieceAtCoord(pos, *c).type != pieceMoving.type)) continue;
            else if(move->from.row == c->row)   specifyFile = true;
            else if(move->from.file == c->file) specifyRow = true;
        }

        move->algebraicNotation[stringIndex++] = PIECE_DATA[pieceMoving.type].symbol;
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

    Position played;
    PositionState state;

    playMove(pos, move, &played);
    state = getPositionState(&played);

    switch(state)
    {
        case CHECK:
            move->algebraicNotation[stringIndex++] = '+';
            break;

        case CHECKMATE:
            move->algebraicNotation[stringIndex++] = '#';
            break;

    }
    move->algebraicNotation[stringIndex] = 0;

}

void playMove(const Position *pos, Move *move, Position *newPosition)
{
    *newPosition = *pos;
    newPosition->en_passant = DEFAULT_INVALID_COORD;

    if(move->isCastling != NO_CASTLE)
    {

        setPieceAtCoord(newPosition, CASTLING_KING_START_COORD[pos->color_playing], NO_PIECE_LITERAL);
        setPieceAtCoord(newPosition, CASTLING_ROOK_START_COORD[pos->color_playing][move->isCastling], NO_PIECE_LITERAL);

        setPieceAtCoord(newPosition, CASTLING_KING_TARGET_COORD[pos->color_playing][move->isCastling], (Piece){KING, newPosition->color_playing});
        setPieceAtCoord(newPosition, CASTLING_ROOK_TARGET_COORD[pos->color_playing][move->isCastling], (Piece){ROOK, newPosition->color_playing});

        newPosition->castling_rights[newPosition->color_playing][SHORT_CASTLE] = false;
        newPosition->castling_rights[newPosition->color_playing][LONG_CASTLE]  = false;

    }
    else
    {
        Piece pieceAtFromCache = getPieceAtCoord(pos, move->from);
        Piece pieceMoving      = getPieceAtCoord(pos, move->from);

        if(pieceMoving.type == KING)
        {
            newPosition->castling_rights[pos->color_playing][SHORT_CASTLE] = false;
            newPosition->castling_rights[pos->color_playing][LONG_CASTLE] = false;
        }
        else if(pieceMoving.type == ROOK)
        {
            for(CastlingMove castleType = SHORT_CASTLE; castleType <=LONG_CASTLE; castleType++)
            {
                if(coordEquals(move->from, CASTLING_ROOK_START_COORD[pos->color_playing][castleType]))
                {
                    newPosition->castling_rights[pos->color_playing][castleType] = false;
                }
            }
        }
        else if(pieceMoving.type == PAWN && move->from.row == PAWN_STARTING_ROW[pos->color_playing]
                                         && move->to.row == (PAWN_STARTING_ROW[pos->color_playing] + 2 * PAWN_MOVING_DIRECTION[pos->color_playing]))
        {

            if(PieceEquals(getPieceAtCoord(pos, (Coord){move->to.file-1, move->to.row}), (Piece){PAWN, OTHER_COLOR(pos->color_playing)}) ||
               PieceEquals(getPieceAtCoord(pos, (Coord){move->to.file+1, move->to.row}), (Piece){PAWN, OTHER_COLOR(pos->color_playing)}) )
            {
                newPosition->en_passant = (Coord){move->from.file, move->from.row + PAWN_MOVING_DIRECTION[pos->color_playing]};
            }
        }



        if(move->isCapture) setPieceAtCoord(newPosition, move->captureSquare, NO_PIECE_LITERAL);
        setPieceAtCoord(newPosition, move->to, pieceAtFromCache);
        setPieceAtCoord(newPosition, move->from, NO_PIECE_LITERAL);

        if(move->promotionType != NO_PIECE) setPieceAtCoord(newPosition, move->to, (Piece){move->promotionType, pos->color_playing});
    }

    newPosition->color_playing = OTHER_COLOR(newPosition->color_playing);
}

// Return if the move exist on the board. If it does, fill all the move data on *move
bool doesMoveExist(const Position *pos, Move *move)
{
    if(!validCoord(move->from) || !validCoord(move->to)) return false;

    if(move->isCastling != NO_CASTLE)
    {
        if(!(pos->castling_rights[pos->color_playing][move->isCastling])) return false;


        if(!(PieceEquals(getPieceAtCoord(pos, CASTLING_KING_START_COORD[pos->color_playing]), (Piece){KING, pos->color_playing}) &&
             PieceEquals(getPieceAtCoord(pos, CASTLING_ROOK_START_COORD[pos->color_playing][move->isCastling]), (Piece){ROOK, pos->color_playing}))) return false;

        Coord start = (move->isCastling == SHORT_CASTLE) ? CASTLING_KING_START_COORD[pos->color_playing] : CASTLING_ROOK_START_COORD[pos->color_playing][move->isCastling];
        Coord stop  = (move->isCastling == SHORT_CASTLE) ? CASTLING_ROOK_START_COORD[pos->color_playing][move->isCastling] : CASTLING_KING_START_COORD[pos->color_playing];

        Coord direction = {1, 0};
        for(COORD_ADD_SELF(start, direction); !coordEquals(start, stop); COORD_ADD_SELF(start, direction))
        {
            if(!PieceEquals(getPieceAtCoord(pos, start), NO_PIECE_LITERAL)) return false;
        }

        return true;
    }

    Piece pieceAtFrom = getPieceAtCoord(pos, move->from);

    if(pieceAtFrom.color != pos->color_playing) return false;

    bool moveFound = false;
    for(MoveType type = 0; !moveFound && type < N_MOVE_TYPES; type++)
    {
        if(!PIECE_DATA[pieceAtFrom.type].move_types.types[type]) continue;

        LList(Move) moves = MOVE_TYPE_FUNCTION_LOOKUP[type](pos, move->from, pos->color_playing);
        Move *cur;
        LListFORPTR(Move, cur, moves)
        {
            if(coordEquals(cur->to, move->to) && move->promotionType == cur->promotionType)
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

bool CreateMoveFromUCI(const char *uci, Move *move)
{
    const size_t len = strlen(uci);

    if(!(len == 4 || len == 5)) return false;

    *move = CLEAR_MOVE;

    move->from = CoordFromAlgebraic(uci);
    move->to   = CoordFromAlgebraic(uci + 2);

    for(PieceColor color = WHITE; color <= BLACK; color++)
    {
        if(!coordEquals(move->from, CASTLING_KING_START_COORD[color])) continue;

        for(CastlingMove castle = SHORT_CASTLE; castle <= LONG_CASTLE; castle++)
        {
            if(coordEquals(move->to, CASTLING_KING_TARGET_COORD[color][castle]))
            {
                move->isCastling = castle;
            }
        }
    }


    if(uci[4]) move->promotionType = PieceFromFENChar(uci[4]).type;

    return !coordEquals(move->from, DEFAULT_INVALID_COORD) && !coordEquals(move->to, DEFAULT_INVALID_COORD);
}

void DebugPrintMove(const Move* move)
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