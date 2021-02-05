#include <stdio.h>

#include "coord.h"

Coord DEFAULT_INVALID_COORD = {-1, -1};

inline bool validCoord(Coord coord)
{
    return coord.file >= FILE_A && coord.file <= FILE_H && coord.row >= ROW_1 && coord.row <= ROW_8;
}

void DebugPrintCoordFull(Coord coord)
{
    PrintCoordAlgebraic(coord);
    printf(" (file: %d row: %d)\n", coord.file + 'a', coord.row + '1', coord.file, coord.row);
}

void PrintCoordAlgebraic(Coord coord)
{
    printf("%c%c", coord.file + 'a', coord.row + '1');
}