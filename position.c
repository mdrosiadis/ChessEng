#include <stdio.h>
#include <assert.h>


#include "position.h"
#include "move.h"



const char castleTypes[] = {'K', 'Q', 'k', 'q'};

static const int DRAW_HALFMOVES = 100;

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

    printf("Half move clock: %d | Fullmove number: %d\n", pos->halfmoveClock, pos->fullmoveNumber);

    printf("En passant: ");
    if(validCoord(pos->en_passant))
        PrintCoordAlgebraic(pos->en_passant);
    else
        putchar('-');

    printf("\nCastling rights: ");
    for(int i=0; i < 4; i++) if(pos->castling_rights[i/2][i%2]) putchar(castleTypes[i]);

    printf("\n%s to move\n", pos->color_playing == WHITE ? "White" : "Black");
}

Position CreatePositionFromFEN(const char* FEN)
{
    Position newPos;
    int file = 0, row = BOARD_SIZE -1;

    newPos.en_passant = DEFAULT_INVALID_COORD;
    newPos.metadata = NULL;

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

    /* Skip space */
    assert(*FEN == ' ');
    FEN++;

    if(*FEN == '-')
    {
        newPos.en_passant = DEFAULT_INVALID_COORD;
        FEN++;
    }
    else
    {
        newPos.en_passant = CoordFromAlgebraic(FEN);
        assert(validCoord(newPos.en_passant));
        FEN+=2;
    }

    assert(*FEN == ' ');
    FEN++;

    assert(sscanf(FEN, "%d %d", &newPos.halfmoveClock, &newPos.fullmoveNumber) == 2);

    return newPos;
}

void CreateFENString(const Position *pos)
{
    for(int row = BOARD_SIZE-1; row >= 0; row--)
    {
        int noPieceRunning = 0;
        for(int file = 0; file < BOARD_SIZE; file++)
        {
            Piece currentPiece = getPieceAtCoord(pos, (Coord){file, row});

            if(currentPiece.type != NO_PIECE)
            {
                if(noPieceRunning) printf("%d", noPieceRunning);

                putchar(PieceFENChar(currentPiece));
                noPieceRunning = 0;
            }
            else
            {
                noPieceRunning++;
            }

        }

        if(noPieceRunning) printf("%d", noPieceRunning);

        if(row != 0) putchar('/');

    }

    printf(" %c ", pos->color_playing == WHITE ? 'w' : 'b');

    bool anyCastlingRights = false;
    for(int i=0; i < 4; i++)
    {
        if(pos->castling_rights[i/2][i%2])
        {
            putchar(castleTypes[i]);
            anyCastlingRights = true;
        }
    }

    if(!anyCastlingRights) putchar('-');

    putchar(' ');

    if(validCoord(pos->en_passant))
        PrintCoordAlgebraic(pos->en_passant);
    else
        putchar('-');

    printf(" %d %d", pos->halfmoveClock, pos->fullmoveNumber);

}

Piece getPieceAtCoord(const Position* pos, Coord coord)
{
    return validCoord(coord) ? pos->position_grid[coord.file][coord.row] : NO_PIECE_LITERAL;
}

void setPieceAtCoord(Position* pos, Coord coord, Piece piece)
{
    if(validCoord(coord)) pos->position_grid[coord.file][coord.row] = piece;
}



// Utility Function: Get the number (and locations) of squares attacking the target square
// param data: NULL for no list creation, pointer to Darray(Coord) to return the data
// return: number of squares / pieces found attacking the target square
// PURPOSE OF EXISTENCE OF THIS FUNCTION: dont create a list of potential attacks if we only need the number of them
int CoordsTargetingCoord(const Position* pos, Coord target, PieceColor color, MoveTypes castingTypes, Darray(Coord) *data)
{
    int numberOfAttacks = 0;
    PieceColor castingAs = OTHER_COLOR(color);

    for(MoveType moveType=MOVE_DIAGONAL; moveType < N_MOVE_TYPES; moveType++)
    {
        if(!castingTypes.types[moveType]) continue;

        Darray(Move) moves = MOVE_TYPE_FUNCTION_LOOKUP[moveType](pos, target, castingAs);

        for(unsigned int i = 0; i < moves.length; i++)
        {
            Piece pieceAtTarget = getPieceAtCoord(pos, moves.data[i].to);
            if(pieceAtTarget.type != NO_PIECE && PIECE_DATA[pieceAtTarget.type].move_types.types[moveType])
            {
                numberOfAttacks++;

                if(data) DarrayPush(Coord)(data, moves.data[i].to);
            }
        }

        DarrayFree(Move)(&moves);
    }

    return numberOfAttacks;
}


PositionState getPositionState(const Position *pos)
{
    PieceColor colorNotPlaying = OTHER_COLOR(pos->color_playing);

    if(! pos->metadata) CreatePositionMetadata(pos);

    //printf("pos state\n");
    return pos->metadata->state;
}

bool isInCheck(const Position *pos, PieceColor color)
{
    Coord kingPos;
    if(pos->metadata)
        kingPos = pos->metadata->kingPositions[color];
    else
    {
        bool found = false;
        Coord current;
        for(current.file = FILE_A; !found && current.file <= FILE_H; current.file++)
            for(current.row = ROW_1; current.row <= ROW_8; current.row++)
            {
                Piece pieceAtCurrent = getPieceAtCoord(pos, current);

                if(PieceEquals(pieceAtCurrent, (Piece){KING, color}))
                {
                    kingPos = current;
                    found = true;
                }
            }
    }

    //PrintCoordAlgebraic(kingPos);

    return CoordsTargetingCoord(pos, kingPos, OTHER_COLOR(color), (MoveTypes){1,1,1,1,1}, NULL) != 0;
}


bool isPositionLegal(const Position* pos)
{

    return !isInCheck(pos, OTHER_COLOR(pos->color_playing));
}

inline bool isPositionPlayable(const Position *pos)
{
    PositionState state = getPositionState(pos);

    return state == NORMAL || state == CHECK;
}



void CreatePositionMetadata(Position *pos)
{
    if(pos->metadata) return;

    pos->metadata = malloc(sizeof (PositionMetadata));


    pos->metadata->legalMoves = createLegalMoves(pos);
    Coord current;
    int found = 0;

    for(current.file = FILE_A; found < 2 && current.file <= FILE_H; current.file++)
        for(current.row = ROW_1; current.row <= ROW_8; current.row++)
        {
            Piece pieceAtCurrent = getPieceAtCoord(pos, current);

            if(pieceAtCurrent.type == KING)
            {
                pos->metadata->kingPositions[pieceAtCurrent.color] = current;
                found++;
            }
        }

    PieceColor colorNotPlaying = OTHER_COLOR(pos->color_playing);

    //printf("meta-check\n");
    // If the color not playing is in check, the position is invalid
    if(isInCheck(pos, colorNotPlaying))
        pos->metadata->state =  INVALID;

    else if(pos->halfmoveClock >= DRAW_HALFMOVES)
        pos->metadata->state = DRAW;

    else
    {
        /* Check for checks and legal moves.
     * If in check and no legal moves -> checkmate, if legal moves -> check.
     * If not in check and no legal moves -> draw*/

        bool inCheck = isInCheck(pos, pos->color_playing);

        int legalMoveCount = (int) pos->metadata->legalMoves.length;


        if(inCheck)
            pos->metadata->state = legalMoveCount > 0 ? CHECK : CHECKMATE;
        else
            pos->metadata->state = legalMoveCount > 0 ? NORMAL : DRAW;
    }

}

void FreeMetadata(PositionMetadata *meta)
{
    if(!meta) return;
    DarrayFree(Move)(&(meta->legalMoves));
    free(meta);
}

void ClearPositionMetadata(Position *pos)
{
    FreeMetadata(pos->metadata);
    pos->metadata = NULL;
}