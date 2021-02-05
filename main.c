#include <stdio.h>

#include "position.h"


int main() {


    Position fen = CreatePositionFromFEN("r1b2bnr/p1pnpppp/1p6/2k1Q3/4N3/q3B3/PPPPPPPP/RN1KKB1R w - - 0 1");

    PositionDebugPrint(&fen);
    Coord* coord;
    printf("White\n");
    LListFORPTR(Coord, coord, CoordsTargetingCoord(&fen, (Coord){FILE_C, ROW_5}, WHITE))
    {
        PrintCoordAlgebraic(*coord);
        printf("\n");

    }

    printf("Black\n");
    LListFORPTR(Coord, coord, CoordsTargetingCoord(&fen, (Coord){FILE_C, ROW_5}, BLACK))
    {
        PrintCoordAlgebraic(*coord);
        printf("\n");

    }

    return 0;
}
