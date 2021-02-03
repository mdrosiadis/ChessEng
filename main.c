#include <stdio.h>

#include "piece.h"
#include "position.h"
#include "move.h"


#include "linkedlist.def.h"

typedef Move* MovePTR;

LListDeclareDefine(int);

LListDeclareDefine(float)




int main() {

    LListCreate(int, test);

    LListAppendData(int)(&test, 5);
    LListAppendData(int)(&test, 9);

    LListFOR(int, el, test)
    {
        printf("%d\n", el);
    }

    LListCreate(float, floatlist);

    LListAppendData(float)(&floatlist, 5.234);
    LListAppendData(float)(&floatlist, 3.1415);


    LListFOR(float, flel, floatlist)
    {
        printf("%f\n", flel);
    }


    LListFreeNodes(int)(&test);
    LListFreeNodes(float)(&floatlist);

    int i=0;
    LListFOR(float, l, floatlist)
    {
        printf("%d\n", i++);
    }


    return 0;
}
