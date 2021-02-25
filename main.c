#include <stdio.h>
#include <string.h>

#include "move.h"
#include "position.h"



int main() {

    char inputmove[25];
    Move pendingMove;


    Position fen = CreatePositionFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    PositionState state;

    while((state = getPositionState(&fen)) == NORMAL || state == CHECK)
    {
        PositionDebugPrint(&fen);
        CreateFENString(&fen);
        printf("\n");

        if(fen.color_playing == WHITE)
        {
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
        else
        {
            // random black player
            LList(Move) moves = getLegalMoves(&fen);
            Move* move;
            LListFORPTR(Move, move, moves)
            {
                createMoveString(&fen, move);
                DebugPrintMove(move);
            }

            int randomIndex = rand() % moves.length;

            int i = 0;
            LListFORPTR(Move, move, moves)
            {
                if(randomIndex == i)
                {
                    playMove(&fen, move, &fen);
                    break;
                }

                i++;
            }

            LListFreeNodes(Move)(&moves);
        }
    }

    printf("%d\n", state);


    return 0;
}
