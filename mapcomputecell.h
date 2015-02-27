#ifndef MAPCOMPUTECELL_H
#define MAPCOMPUTECELL_H

#include <iostream>

#include "myintensity.h"

namespace dflighting
{


//
typedef float DirectionsTypeF[3][3];



//
class MapComputeCell
{
public:
    DirectionsTypeF Directions;
    MyIntensity Intensity;
    bool IsProcessMarked;
    bool IsWithinLightedCellsArray;


    //
    void Clear();
    void SaveToStream(std::ostream * stream);
    void LoadFromStream(std::istream * stream);
};


}

#endif // MAPCOMPUTECELL_H
