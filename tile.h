#ifndef TILE_H
#define TILE_H

#include <unordered_set>

#include "mycolor.h"
#include "myintensity.h"

#include "myintdpoint.h"

#include "tilestructures.h"


namespace dflighting
{


class LightVisibilityCacheStruct;
class LightVisibilityCache1;


class Tile
{
public:
    uint32_t X;
    uint32_t Y;
    uint32_t Z;

    TileType Tile_Type;
    TileMaterialType Tile_Material;
    uint32_t Material_Index;
    uint32_t Material_Type;
    Material * Mat;

    FlowData * Flow;

    MyColor ColorValueI;

    MyIntensity LightValueITraced;
    bool IsWithinLightedTilesTraced;

    int MemoryAmount;
    bool IsMemoryAffected;

    bool IsPassableByLight;
    bool IsPassableByLight_HoleUp;
    bool IsPassableByLight_HoleDown;

    bool IsPassableByDwarf;
    bool IsPassableByDwarf_HoleUp;
    bool IsPassableByDwarf_HoleDown;

    bool IsDoor;
    bool IsDoorOpened;

    std::unordered_set<LightVisibilityCache1 *> LightedByLights;
    std::unordered_set<LightVisibilityCache1 *> LightedByMemoryLights;
    std::unordered_set<MyIntDPoint, MyIntDPointHasher> PotentialLightCaches;

    std::unordered_set<Tile *> Neighbours;

    bool IsPathfinded;

    // copied version of this cell
    Tile * CopyForMemory;

    int DLIndex;

    //
    Tile();
    ~Tile();

    //
    void ModifyPassableFlags();
    void InitializeCopyForMemory();
    void ModifyCopyForMemory();
    void DeleteCopyForMemory();
};



}

#endif // TILE_H
