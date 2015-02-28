#ifndef LIGHTVISIBILITYCACHESTRUCT_H
#define LIGHTVISIBILITYCACHESTRUCT_H

#include <vector>
#include <unordered_set>

#include "tile.h"
#include "colorercell.h"
#include "myintpoint.h"

#include "direction.h"

namespace dflighting
{


//
class LightVisibilityCacheStruct
{
public:
    int WorldX;
    int WorldY;
    int WorldZ;
    Direction Dir;

    int LightsHandled;

    //std::vector<ColorerCell> Colorers;
    //std::vector<int> ColorersIndicesSum;
    ColorerCell * Colorers;
    int NColorers;
    int * ColorersIndicesSum;

    //
    int NLayers;

    std::vector<MyIntPoint> TracePoints;
    int NTracePoints;

    int CurrentCacheTime;

    int DLIndex;

    std::unordered_set<Tile*> PotentialTiles;

    //
    LightVisibilityCacheStruct(int x, int y, int z, Direction dir)
    {
        WorldX = x;
        WorldY = y;
        WorldZ = z;
        Dir = dir;

        Colorers = nullptr;
        ColorersIndicesSum = nullptr;
        NColorers = 0;

        NTracePoints = 0;

        LightsHandled = 0;
        NLayers = 0;

        CurrentCacheTime = 0;
    }

    //
    ~LightVisibilityCacheStruct()
    {
        if (Colorers != nullptr)
        {
            delete [] Colorers;
            Colorers = nullptr;
        }

        if (ColorersIndicesSum != nullptr)
        {
            delete [] ColorersIndicesSum;
            ColorersIndicesSum = nullptr;
        }
        NColorers = 0;

        //Colorers.clear();

        //ColorersIndicesSum.clear();

        ////
        TracePoints.clear();
    }

    //
    MyIntDPoint GetIntDPoint()
    {
        return MyIntDPoint(WorldX, WorldY, WorldZ, Dir);
    }

};



}


#endif // LIGHTVISIBILITYCACHESTRUCT_H
