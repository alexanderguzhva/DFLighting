#ifndef LIGHTVISIBILITYCACHE1_H
#define LIGHTVISIBILITYCACHE1_H

#include <vector>

#include "mycolor.h"
#include "tile.h"
#include "myintdpoint.h"

namespace dflighting
{


//
class LightVisibilityCache1
{
public:
    int WorldX;
    int WorldY;
    int WorldZ;
    Direction Dir;

    MyColor Color;
    int MaxLayers;

    //LightVisibilityCacheStruct * LightCache;

    std::vector<Tile*> TilesHit;
    std::vector<Tile*> MTilesHit;

    //
    LightVisibilityCache1(int x, int y, int z, Direction dir, MyColor color);

    //
    static int Color2MaxLayers(const MyColor &color);

    //
    void ChangeColor(const MyColor & newColor);

    //
    MyIntDPoint GetIntDPoint();
};


}


#endif // LIGHTVISIBILITYCACHE1_H
