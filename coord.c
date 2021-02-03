#include <stdio.h>

#include "coord.h"


inline bool validCoord(Coord coord)
{
    return coord.file >= FILE_A && coord.file <= FILE_H && coord.row >= ROW_1 && coord.row <= ROW_8;
}

void DebugPrintCoord(Coord coord)
{
    printf("%c%c (file: %d row: %d)\n", coord.file + 'a', coord.row + '1', coord.file, coord.row);
}