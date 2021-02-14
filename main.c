#include <stdio.h>
#include <string.h>

#include "move.h"
#include "position.h"



int main() {

    char inputmove[25];
    Move pendingMove;

    Position fen = CreatePositionFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    // getLegalMoves(&fen);

    while(isPositionPlayable(&fen))
    {
        PositionDebugPrint(&fen);
        printf("UCI (q to stop): ");
        fgets(inputmove, 24, stdin);
        inputmove[strlen(inputmove)-1] = 0; /* remove newline */

        if(!strcmp(inputmove, "q")) break;

        if(CreateMoveFromUCI(inputmove, &pendingMove))
        {
            printf("Syntax correct!\n");

            if(doesMoveExist(&fen, &pendingMove))
            {
                printf("Move exists!\n");

                if(isLegalMove(&fen, &pendingMove))
                {
                    printf("Move is Legal! Move is: ");
                    createMoveString(&fen, &pendingMove);
                    DebugPrintMove(&pendingMove);
                    playMove(&fen, &pendingMove, &fen);
                }
                else
                {
                    printf("Not a legal move!\n");
                }
            }
            else
            {
                printf("Move doesnt exist on the board!\n");
            }
        }
        else
        {
            printf("Bad UCI!\n");
        }
    }


    return 0;
}
