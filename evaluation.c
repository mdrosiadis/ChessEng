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

            if(current.file >= FILE_D &&  current.file <= FILE_E && current.row >= ROW_4 && current.row <= ROW_5)
                materialCount += PIECE_CENTIPAWNS[pieceAtCurrent.type] * colorMultiplier / 10; // center bonus

            if(pieceAtCurrent.type == KING)
            {

                for(int df = -1; df <= 1; df++)
                    for(int dr = -1; dr <= 1; dr++)
                    {
                        Coord kingTarget = {current.file + df, current.row + dr};

                        if(!validCoord(kingTarget)) continue;

                        int attackModifier = 5;

                        if(CoordEquals(kingTarget, current)) attackModifier = 2;

                        Piece p = getPieceAtCoord(pos, kingTarget);
                        if(PieceEquals(p, (Piece){ROOK, pieceAtCurrent.color}))
                            finalEvaluation += colorMultiplier * 85; // "castling" buff

                        Darray(Coord) attackers = DarrayInit(Coord)(10);
                        int attackCount = CoordsTargetingCoord(pos, kingTarget, OTHER_COLOR(pieceAtCurrent.color), (MoveTypes){1,1,1,1,1}, &attackers);


                        if(attackCount)
                        {
                            int attackTotal = 0;

                            for(unsigned int attInd = 0; attInd < attackers.length; attInd++)
                            {
                                attackTotal += PIECE_CENTIPAWNS[getPieceAtCoord(pos, attackers.data[attInd]).type];
                            }
                            attackTotal = - COLOR_MULTIPLIER(pieceAtCurrent.color) * (attackTotal * attackCount / attackModifier);

                            finalEvaluation += attackTotal;
                        }

                        DarrayFree(Coord)(&attackers);

                        Darray(Coord) defenders = DarrayInit(Coord)(10);
                        int defendCount = CoordsTargetingCoord(pos, kingTarget, pieceAtCurrent.color, (MoveTypes){1,1,1,1,1}, &defenders);


                        if(defendCount)
                        {
                            int defendTotal = 0;

                            for(unsigned int dInd = 0; dInd < attackers.length; dInd++)
                            {
                                defendTotal += PIECE_CENTIPAWNS[getPieceAtCoord(pos, attackers.data[dInd]).type];
                            }
                            defendTotal =  COLOR_MULTIPLIER(pieceAtCurrent.color) * (defendTotal * attackCount / 20);

                            finalEvaluation += defendTotal;
                        }

                        DarrayFree(Coord)(&attackers);
                    }
            }

        }
    }


    finalEvaluation += materialCount;

    for(int color = 0; color < 2; color++)
    {
        finalEvaluation += COLOR_MULTIPLIER(color) * (int)(((float) conectivity_score[color] / (float) pow(piece_counts[color], 2.0)) * 25);
    }

    finalEvaluation += COLOR_MULTIPLIER(pos->color_playing) * 25;

    return finalEvaluation;
}

static int calls;
static int* StaticEvaluationHelperArray;
static Position* globalPos;

struct evalpospair
{
    Position pos;
    int static_eval;
    Move *move;
};

typedef struct evalpospair EvalPosPair;

static int PositionInitalSortingComp_white(const void *a, const void *b)
{
    return ((EvalPosPair*) b)->static_eval - ((EvalPosPair*) a)->static_eval;
}

static int PositionInitalSortingComp_black(const void *a, const void *b)
{
    return ((EvalPosPair*) a)->static_eval - ((EvalPosPair*) b)->static_eval;
}

DarrayDeclarations(EvalPosPair)
DarrayDefinitions(EvalPosPair)

static int MovePriorityComp(const void *a, const void *b)
{
    return PIECE_CENTIPAWNS[getPieceAtCoord(globalPos, ((Move*)b)->from).type] -
           PIECE_CENTIPAWNS[getPieceAtCoord(globalPos, ((Move*)a)->from).type];
}


static int alphaBetaPruning(const Position *pos, int depth, int alpha, int beta, Move *result)
{
    PositionState state = getPositionState(pos);
    calls++;
//    printf("Call to alphabeta: depth %d, calls %d\n", depth, calls++);

    if(state == CHECKMATE)
        return COLOR_MULTIPLIER(OTHER_COLOR(pos->color_playing)) * CHECKMATE_SCORE;
    else if(state == DRAW)
        return 0;

    if(depth == 0)
        return PositionStaticEvaluation(pos);

    Darray(Move) moves = getLegalMoves(pos);


    globalPos = pos;
    qsort(moves.data, moves.length, sizeof(Move), MovePriorityComp);



    Darray(EvalPosPair) sortedResPos = DarrayInit(EvalPosPair)(moves.length);
    //StaticEvaluationHelperArray = malloc(moves.length * sizeof(int));
    for(unsigned int i = 0; i < moves.length; i++)
    {
        playMove(pos, &moves.data[i], &sortedResPos.data[i].pos);
        sortedResPos.data[i].static_eval = PositionStaticEvaluation(&sortedResPos.data[i].pos);
        sortedResPos.data[i].move = &moves.data[i];
        sortedResPos.length++;
    }

    // TODO: all sort of memory leaks here

    int value;

    if(pos->color_playing == WHITE)
    {
        value = -CHECKMATE_SCORE;

        qsort(sortedResPos.data, sortedResPos.length, sizeof(EvalPosPair), PositionInitalSortingComp_white);


        for(unsigned int i = 0; i < sortedResPos.length; i++)
        {

            int bestSearched = alphaBetaPruning(&sortedResPos.data[i].pos, depth-1, alpha, beta, NULL);

            if(bestSearched >= value)
            {
                value = bestSearched;
                if(result)
                    *result = *sortedResPos.data[i].move;
            }

            if(value > alpha) alpha = value;

            if(alpha >= beta) break;
        }


    }
    else
    {
        qsort(sortedResPos.data, sortedResPos.length, sizeof(EvalPosPair), PositionInitalSortingComp_black);

        value = CHECKMATE_SCORE;
        for(unsigned int i = 0; i < sortedResPos.length; i++)
        {


            int bestSearched = alphaBetaPruning(&sortedResPos.data[i].pos, depth-1, alpha, beta, NULL);

            if(bestSearched <= value)
            {
                value = bestSearched;
                if(result)
                    *result = *sortedResPos.data[i].move;
            }

            if(value < beta) beta = value;

            if(alpha >= beta) break;
        }

    }

    DarrayFree(EvalPosPair)(&sortedResPos);
    DarrayFree(Move)(&moves);
    return value;
}

Move GetBestMove(const Position *pos)
{
    Move bestmove;
    calls = 0;
    int eval = alphaBetaPruning(pos, 5, -CHECKMATE_SCORE, CHECKMATE_SCORE, &bestmove);
    printf("Total calls alphabeta: %d\n", calls++);
    printf("Move eval: %d\n", eval);

    return bestmove;
}