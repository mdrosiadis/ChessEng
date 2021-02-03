#include <stdio.h>

#include "piece.h"
#include "position.h"
#include "move.h"


#include "linkedlist.h"

int main() {

//    Position fen = CreatePositionFromFEN("r1bq2nr/1pppp1pp/p1n3b1/3k1p2/1N2Q3/3P1K2/PPP1PPPP/RNB2B1R w - - 0 1");
//
//    PositionDebugPrint(&fen);
//
//    fen.en_passant = (Coord){FILE_F, ROW_6};
//
//    //DiagonalMove(&fen, (Coord){FILE_E, ROW_4}, BLACK);
//    //CrossMove(&fen, (Coord){FILE_E, ROW_4}, BLACK);
//    //KnightMove(&fen, (Coord){FILE_B, ROW_4}, BLACK);
//    //printf("-\n");
//
//
//    KingMove(&fen, (Coord){FILE_E, ROW_1}, WHITE);


    LListCreate(test);

    LListAppendData(&test, 6);
    LListAppendData(&test, 2);
    LListAppendData(&test, 5);
    LListAppendData(&test, 6);
    LListAppendData(&test, 8);


    LListFOR(elem, test)
    {
        printf("%d\n", elem);
    }

    LListFree(&test);
    return 0;
}
