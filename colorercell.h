#ifndef COLORERCELL_H
#define COLORERCELL_H

#include "tile.h"
#include "myintensity.h"

namespace dflighting
{

//
struct ColorerCell
{
    Tile * TargetWorldTile;
    MyIntensity Intensity;

    ColorerCell() { }
    ColorerCell(Tile *& targetWorldTile, const MyIntensity & intensity)
    {
        TargetWorldTile = targetWorldTile;
        Intensity = intensity;
    }
};

}

#endif // COLORERCELL_H
