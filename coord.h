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

extern const Coord DEFAULT_INVALID_COORD;


#define IS_DEFAULT_INVALID_COORD(coord) (CoordEquals(coord, DEFAULT_INVALID_COORD))

bool validCoord(Coord coord);

char CoordFileToChar(CoordFile file);
char CoordRowToChar(CoordRow row);

bool CoordEquals(Coord a, Coord b);

void DebugPrintCoordFull(Coord coord);
void PrintCoordAlgebraic(Coord coord);

Coord CoordFromAlgebraic(const char *alg);

#endif //CHEESENG_COORD_H
