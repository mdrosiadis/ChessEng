#include <stdio.h>
#include <math.h>

#include "evaluation.h"
#include "coord.h"
#include "linkedlist.def.h"

#define COLOR_MULTIPLIER(color) (1 - (int) (color) * 2)


static int PIECE_CENTIPAWNS[] = { 100, 310, 320, 500, 900, 0};

static int CHECKMATE_SCORE = 999999;

int PositionStaticEvaluation(const Position *pos)
{
    Coord current;

    int finalEvaluation = 0;

    int materialCount = 0;

    int piece_counts[2] = {0, 0};
    int conectivity_score[2] = {0, 0};


    for(current.file = FILE_A; current.file <= FILE_H; current.file++)
    {
        for(current.row = ROW_1; current.row <= ROW_8; current.row++)
        {
            Piece pieceAtCurrent = getPieceAtCoord(pos, current);

            if(pieceAtCurrent.type == NO_PIECE) continue;

            int colorMultiplier = COLOR_MULTIPLIER(pieceAtCurrent.color);

            piece_counts[pieceAtCurrent.color]++;
            materialCount += PIECE_CENTIPAWNS[pieceAtCurrent.type] * colorMultiplier;

            conectivity_score[pieceAtCurrent.color] += CoordsTargetingCoord(pos, current, pieceAtCurrent.color, (MoveTypes){1,1,1,1,1}, NULL);

            if(pieceAtCurrent.type == KING)
            {
                for(int df = -1; df <= 1; df++)
                    for(int dr = -1; dr <= 1; dr++)
                    {
                        Coord kingTarget = {current.file + df, current.row + dr};

                        if(!validCoord(kingTarget)) continue;

                        int attackModifier = 10;

                        if(CoordEquals(kingTarget, current)) attackModifier = 5;

                        LListCreate(Coord, attackers);
                        int attackCount = CoordsTargetingCoord(pos, kingTarget, OTHER_COLOR(pieceAtCurrent.color), (MoveTypes){1,1,1,1,1}, &attackers);


                        if(attackCount)
                        {
                            int attackTotal = 0;
                            Coord *at;
                            LListFORPTR(Coord, at, attackers)
                            {
                                attackTotal += PIECE_CENTIPAWNS[getPieceAtCoord(pos, *at).type];
                            }
                            attackTotal = - COLOR_MULTIPLIER(pieceAtCurrent.color) * (attackTotal * attackCount / attackModifier);

                            finalEvaluation += attackTotal;
                        }

                        LListFreeNodes(Move)(&attackers);
                    }
            }

        }
    }


    finalEvaluation += materialCount;

    for(int color = 0; color < 2; color++)
    {
        finalEvaluation += COLOR_MULTIPLIER(color) * (int)(((float) conectivity_score[color] / (float) pow(piece_counts[color], 2.0)) * 100);
    }

    finalEvaluation += COLOR_MULTIPLIER(pos->color_playing) * 25;

    return finalEvaluation;
}

static int alphaBetaPruning(const Position *pos, int depth, int alpha, int beta, Move *result)
{
    PositionState state = getPositionState(pos);


    if(state == CHECKMATE)
        return COLOR_MULTIPLIER(OTHER_COLOR(pos->color_playing)) * CHECKMATE_SCORE;
    else if(state == DRAW)
        return 0;

    if(depth == 0)
        return PositionStaticEvaluation(pos);

    LList(Move) moves = getLegalMoves(pos);
    Move *move;

    int value;

    if(pos->color_playing == WHITE)
    {
        value = -CHECKMATE_SCORE;
        LListFORPTR(Move, move, moves)
        {
            Position newPos;
            playMove(pos, move, &newPos);

            int bestSearched = alphaBetaPruning(&newPos, depth-1, alpha, beta, NULL);

            if(bestSearched > value)
            {
                value = bestSearched;
                if(result) *result = *move;
            }

            if(value > alpha) alpha = value;

            if(alpha >= beta) break;
        }


    }
    else
    {
        value = CHECKMATE_SCORE;
        LListFORPTR(Move, move, moves)
        {
            Position newPos;
            playMove(pos, move, &newPos);

            int bestSearched = alphaBetaPruning(&newPos, depth-1, alpha, beta, NULL);

            if(bestSearched < value)
            {
                value = bestSearched;
                if(result) *result = *move;
            }

            if(value < beta) beta = value;

            if(alpha >= beta) break;
        }

    }


    LListFreeNodes(Move)(&moves);
    return value;
}

Move GetBestMove(const Position *pos)
{
    Move bestmove;

    int eval = alphaBetaPruning(pos, 4, -CHECKMATE_SCORE, CHECKMATE_SCORE, &bestmove);

    printf("Move eval: %d\n", eval);

    return bestmove;
}