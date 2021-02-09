#include <stdio.h>
#include <ctype.h>
#include <assert.h>


#include "position.h"
#include "move.h"


LListDefinitions(Coord)

const char castleTypes[] = {'K', 'Q', 'k', 'q'};

void PositionDebugPrint(const Position* pos)
{

    for(int row = BOARD_SIZE-1; row >= 0; row--)
    {
        for(int file = 0; file < BOARD_SIZE; file++)
        {
            putchar(PieceFENChar(getPieceAtCoord(pos, (Coord){file, row})));

        }

        putchar('\n');
    }

    printf("Castling rights: ");
    for(int i=0; i < 4; i++) if(pos->castling_rights[i/2][i%2]) putchar(castleTypes[i]);

    printf("\n%s to move\n", pos->color_playing == WHITE ? "White" : "Black");
}

Position CreatePositionFromFEN(const char* FEN)
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
    assert(*FEN == ' ');
    FEN++;
    // turn

    switch(*FEN++)
    {
        case 'w': newPos.color_playing = WHITE; break;
        case 'b': newPos.color_playing = BLACK; break;

        default: exit(-1);
    }

    for(FEN++; *FEN && *FEN != ' '; FEN++)
    {

        for(int i = 0; i < 4; i++)
        {
            if(*FEN == castleTypes[i])
            {
                newPos.castling_rights[i/2][i%2] = true;
                break;
            }
        }
    }



    return newPos;
}

Piece getPieceAtCoord(const Position* pos, Coord coord)
{
    return validCoord(coord) ? pos->position_grid[coord.file][coord.row] : NO_PIECE_LITERAL;
}

void setPieceAtCoord(Position* pos, Coord coord, Piece piece)
{
    if(validCoord(coord)) pos->position_grid[coord.file][coord.row] = piece;
}

//bool makeMove(Position* pos, Move* move)
//{
//    // make move, check if is legal, return true on success
//}

// Utility Function: Get the number (and locations) of squares attacking the target square
// param data: NULL for no list creation, pointer to LList(Coord) to return the data
// return: number of squares / pieces found attacking the target square
// PURPOSE OF EXISTENCE OF THIS FUNCTION: dont create a list of potential attacks if we only need the number of them
int CoordsTargetingCoord(const Position* pos, Coord target, PieceColor color, MoveTypes castingTypes, LList(Coord) *data)
{
    int numberOfAttacks = 0;
    PieceColor castingAs = OTHER_COLOR(color);

    for(MoveType moveType=MOVE_DIAGONAL; moveType < N_MOVE_TYPES; moveType++)
    {
        if(!castingTypes.types[moveType]) continue;

        LList(Move) moves = MOVE_TYPE_FUNCTION_LOOKUP[moveType](pos, target, castingAs);

        Move* move;
        LListFORPTR(Move, move, moves)
        {
            Piece pieceAtTarget = getPieceAtCoord(pos, move->to);
            if(move->isCapture && PIECE_DATA[pieceAtTarget.type].move_types.types[moveType])
            {
                numberOfAttacks++;

                if(data) LListAppendData(Coord)(data, move->to);
            }
        }

        LListFreeNodes(Move)(&moves);
    }

    return numberOfAttacks;
}


bool isPositionLegal(const Position* pos)
{
    PieceColor colorNotPlaying = OTHER_COLOR(pos->color_playing);


    Coord kingPositions[2] = {DEFAULT_INVALID_COORD, DEFAULT_INVALID_COORD};

    Coord current;
    for(current.file = FILE_A; current.file <= FILE_H; current.file++)
        for(current.row = ROW_1; current.row <= ROW_8; current.row++)
        {
            Piece pieceAtCurrent = getPieceAtCoord(pos, current);

            if(pieceAtCurrent.type == KING)
            {
                // More than one kings for one color in position -> INVALID
                if(!coordEquals(kingPositions[pieceAtCurrent.color], DEFAULT_INVALID_COORD))
                    return false;

                kingPositions[pieceAtCurrent.color] = current;

            }
        }

    // No kings found for some color -> INVALID
    if(IS_DEFAULT_INVALID_COORD(kingPositions[WHITE]) || IS_DEFAULT_INVALID_COORD(kingPositions[BLACK]))
        return false;


    // If the color not playing is in check, the position is invalid

    return CoordsTargetingCoord(pos, kingPositions[colorNotPlaying], pos->color_playing, (MoveTypes){1,1,1,1,1}, NULL) == 0;
}


