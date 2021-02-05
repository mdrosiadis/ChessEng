#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "position.h"
#include "move.h"
#include "linkedlist.def.h"

LListDefinitions(Coord)


void PositionDebugPrint(Position* pos)
{

    for(int row = BOARD_SIZE-1; row >= 0; row--)
    {
        for(int file = 0; file < BOARD_SIZE; file++)
        {
            putchar(PieceFENChar(&(pos->position_grid[file][row])));

        }

        putchar('\n');
    }

    printf("%s to move\n", pos->color_playing == WHITE ? "White" : "Black");
}

Position CreatePositionFromFEN(char* FEN)
{
    Position newPos;
    int file = 0, row = BOARD_SIZE -1;

    // Position data
    for(; *FEN && !(row == 0 && file == BOARD_SIZE); FEN++)
    {
        // printf("file %d row %d FEN: %s\n", file, row, FEN);

        if(*FEN >= '1' && *FEN <= '8')
        {
            int empty = *FEN - '0';

            assert(file+empty <= BOARD_SIZE);

            for(unsigned int i = 0; i < empty; i++, file++)
                newPos.position_grid[file][row] = NO_PIECE_LITERAL;
        }
        else if(*FEN == '/')
        {
            assert(file == BOARD_SIZE);

            row--;
            file = 0;
        }
        else
        {
            newPos.position_grid[file][row] = PieceFromFENChar(*FEN);
            file++;
        }

    }

    // skip space
    assert(row == 0 && file == BOARD_SIZE);

    assert(*FEN++ == ' ');

    // turn

    switch(*FEN++)
    {
        case 'w': newPos.color_playing = WHITE; break;
        case 'b': newPos.color_playing = BLACK; break;

        default: exit(-1);
    }



    return newPos;
}

Piece* getPieceAtCoord(Position* pos, Coord coord)
{
    return validCoord(coord) ? &(pos->position_grid[coord.file][coord.row]) : NULL;
}

//bool makeMove(Position* pos, Move* move)
//{
//    // make move, check if is legal, return true on success
//}


LList(Coord) CoordsTargetingCoord(Position* pos, Coord target, PieceColor color)
{
    PieceColor castingAs = OTHER_COLOR(color);
    LListCreate(Coord, coordsTargeting);

    for(MoveType moveType=MOVE_DIAGONAL; moveType < N_MOVE_TYPES; moveType++)
    {
        LList(Move) moves = MOVE_TYPE_FUNCTION_LOOKUP[moveType](pos, target, castingAs);

        Move* move;
        LListFORPTR(Move, move, moves)
        {
            Piece* pieceAtTarget = getPieceAtCoord(pos, move->to);
            if(move->isCapture && PIECE_DATA[pieceAtTarget->type].moveTypes[moveType])
            {
                LListAppendData(Coord)(&coordsTargeting, move->to);
            }
        }

        LListFreeNodes(Move)(&moves);
    }

    return coordsTargeting;
}


bool isPositionLegal(Position* pos)
{
    PieceColor colorNotPlaying = OTHER_COLOR(pos->color_playing);


    Coord kingPositions[2] = {DEFAULT_INVALID_COORD, DEFAULT_INVALID_COORD};

    Coord current;
    for(current.file = FILE_A; current.file <= FILE_H; current.file++)
        for(current.row = ROW_1; current.row <= ROW_8; current.row++)
        {
            Piece* pieceAtCurrent = getPieceAtCoord(pos, current);

            if(pieceAtCurrent->type == KING)
            {
                // More than one kings for one color in position -> INVALID
                if(!COORD_EQUALS(kingPositions[pieceAtCurrent->color], DEFAULT_INVALID_COORD))
                    return false;

                kingPositions[pieceAtCurrent->color] = current;

            }
        }

    // No kings found for some color -> INVALID
    if(IS_DEFAULT_INVALID_COORD(kingPositions[WHITE]) || IS_DEFAULT_INVALID_COORD(kingPositions[BLACK]))
        return false;


    // If the color not playing is in check, the position is invalid

    bool checksFound = false;

    for(MoveType moveType=MOVE_DIAGONAL; !checksFound && moveType < N_MOVE_TYPES; moveType++)
    {
        LList(Move) moves = MOVE_TYPE_FUNCTION_LOOKUP[moveType](pos, kingPositions[colorNotPlaying], colorNotPlaying);

        Move* move;
        LListFORPTR(Move, move, moves)
        {
            Piece* pieceAtTarget = getPieceAtCoord(pos, move->to);
            if(move->isCapture && PIECE_DATA[pieceAtTarget->type].moveTypes[moveType])
            {
                checksFound = true;
                break;
            }
        }

        LListFreeNodes(Move)(&moves);
    }

    return !checksFound;
}


void getLegalMoves(Position* pos)
{

}