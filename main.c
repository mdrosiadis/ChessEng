#include <stdio.h>

#include "move.h"
#include "position.h"



int main() {


    Position fen = CreatePositionFromFEN("rnb1kbnr/pppppppp/1q6/8/8/8/8/R3K2R w KQkq - 0 1");

    PositionDebugPrint(&fen);



    Move shortCastle = {.isCastling = SHORT_CASTLE};
    Move longCastle = {.isCastling = LONG_CASTLE};

    printf("%d%d\n", isLegalMove(&fen, &shortCastle), isLegalMove(&fen, &longCastle));


    printf("%d\n", isPositionLegal(&fen));

    LList(Move) moves = getLegalMoves(&fen);
    Move* m;
    LListFORPTR(Move, m, moves)
    {
        createMoveString(&fen, m);
        DebugPrintMove(m);
    }

    //
//    playMove(&fen, &exd5, &fen);
//    PositionDebugPrint(&fen);

    return 0;
}
