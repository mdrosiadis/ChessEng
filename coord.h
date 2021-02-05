#ifndef CHEESENG_COORD_H
#define CHEESENG_COORD_H

#include <stdbool.h>


typedef enum COORD_FILE{FILE_A=0, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H} CoordFile;

typedef enum COORD_ROW {ROW_1=0, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7, ROW_8} CoordRow;


typedef struct coord
{
    CoordFile file;
    CoordRow  row;
} Coord;

extern Coord DEFAULT_INVALID_COORD;

#define COORD_EQUALS(a, b) (a.file == b.file && a.row == b.row)
#define IS_DEFAULT_INVALID_COORD(coord) (COORD_EQUALS(coord, DEFAULT_INVALID_COORD))

bool validCoord(Coord coord);

void DebugPrintCoordFull(Coord coord);
void PrintCoordAlgebraic(Coord coord);

#endif //CHEESENG_COORD_H
