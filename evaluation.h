#ifndef CHEESENG_EVALUATION_H
#define CHEESENG_EVALUATION_H

#include <stdbool.h>
#include "position.h"
#include "move.h"

typedef struct poseval
{
    int evaluation_score;
    int forced_win;
} Evaluation;

int EvaluationCompare(Evaluation a, Evaluation b);

int PositionStaticEvaluation(const Position *pos);

Move GetBestMove(const Position *pos);

#endif //CHEESENG_EVALUATION_H
