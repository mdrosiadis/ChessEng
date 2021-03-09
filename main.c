#include <stdio.h>
#include <string.h>

#include "move.h"
#include "position.h"

#include "evaluation.h"


int main() {

    char inputmove[25];
    Move pendingMove;


    Position fen = CreatePositionFromFEN("8/6kp/1pR3p1/8/4P3/2P2KPP/4r1r1/7R b - - 2 26");

    PositionState state;

    while((state = getPositionState(&fen)) == NORMAL || state == CHECK)
    {

        CreatePositionMetadata(&fen);
        PositionDebugPrint(&fen);
        CreateFENString(&fen);

        printf("\n");


        printf("UCI (q to stop, e for egnine move): ");
        fgets(inputmove, 24, stdin);
        inputmove[strlen(inputmove)-1] = 0; /* remove newline */

        if(!strcmp(inputmove, "q")) break;

        if(!strcmp(inputmove, "e"))
        {
            Move bestmove = GetBestMove(&fen);


            printf("Playing move: ");
            createMoveString(&fen, &bestmove);
            DebugPrintMove(&bestmove);



            playMove(&fen, &bestmove, &fen);


        }
        else {
            if (CreateMoveFromUCI(inputmove, &pendingMove)) {
                printf("Syntax correct!\n");

                if (doesMoveExist(&fen, &pendingMove)) {
                    printf("Move exists!\n");

                    if (isLegalMove(&fen, &pendingMove)) {
                        printf("Move is Legal! Move is: ");
                        createMoveString(&fen, &pendingMove);
                        DebugPrintMove(&pendingMove);


                        playMove(&fen, &pendingMove, &fen);

                    } else {
                        printf("Not a legal move!\n");
                    }
                } else {
                    printf("Move doesnt exist on the board!\n");
                }
            } else {
                printf("Bad UCI!\n");
            }
        }
    }

    printf("%d\n", state);
    PositionDebugPrint(&fen);
    CreateFENString(&fen);

    return 0;
}
