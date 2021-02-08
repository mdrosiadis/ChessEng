#include <stdio.h>

#include "move.h"
#include "position.h"



int main() {


    Position fen = CreatePositionFromFEN("rn1b1bnr/ppPppppp/4kq2/8/3Q4/3PK2N/PPP2PPP/RNB2B1R w - - 0 1");

    PositionDebugPrint(&fen);

    printf("%d\n", isPositionLegal(&fen));

    LList(Move) moves = getLegalMoves(&fen);
    Move* m;
    LListFORPTR(Move, m, moves)
    {
        DebugPrintMove(m);
    }
    return 0;
}
