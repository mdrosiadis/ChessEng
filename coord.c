#include <stdio.h>

#include "coord.h"

const Coord DEFAULT_INVALID_COORD = {-1, -1};

inline bool validCoord(Coord coord)
{
    return coord.file >= FILE_A && coord.file <= FILE_H && coord.row >= ROW_1 && coord.row <= ROW_8;
}

void DebugPrintCoordFull(Coord coord)
{
    PrintCoordAlgebraic(coord);
    printf(" (file: %d row: %d)\n", coord.file + 'a', coord.row + '1', coord.file, coord.row);
}

inline bool coordEquals(Coord a, Coord b)
{
    return a.file == b.file && a.row == b.row;
}

inline char CoordFileToChar(CoordFile file)
{
    return file + 'a';
}

inline char CoordRowToChar(CoordRow row)
{
    return row + '1';
}

void PrintCoordAlgebraic(Coord coord)
{
    printf("%c%c", CoordFileToChar(coord.file), CoordRowToChar(coord.row));
}