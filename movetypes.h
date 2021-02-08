#ifndef CHEESENG_MOVETYPES_H
#define CHEESENG_MOVETYPES_H

#define N_MOVE_TYPES 5

typedef struct movetypes
{
    bool types[N_MOVE_TYPES];
} MoveTypes;


typedef enum move_type{MOVE_DIAGONAL, MOVE_CROSS, MOVE_KNIGHT, MOVE_PAWN, MOVE_KING} MoveType;

#endif //CHEESENG_MOVETYPES_H
