#ifndef UNIVERSE_H
#define UNIVERSE_H



//// 0 = disable them
//// otherwise add walking dwarves (sources of light).
//// creates great overhead because of naive A* implementation
#define WALKING_DWARFS_COUNT 2


#include <unordered_set>
#include <vector>

#include "worldmap.h"
#include "lightworldmap.h"
#include "mapcomputecells.h"
#include "mapcomputecellfacedmap.h"

#include "lightvisibilitycachestruct.h"
#include "lightvisibilitycache1.h"
#include "lightvisibilitycache6.h"
#include "mycolor.h"
#include "tile.h"

#include "walkingagent.h"
#include "random.h"


namespace dflighting
{

//
const int MaxMemoryTime = 200;


//
class Universe
{
public:
    WorldMap * worldMap;
    LightWorldMap * lightWorldMap;

    //
    MapComputeCells ComputingCells;

    ////
    MapComputeCellFacedMap * MapComputeCellsFaced;


    //
    std::unordered_set<LightVisibilityCacheStruct *> LightCachesToRemove;

    //
    std::vector<Tile *> PathfindableCells;

    //
    std::vector<WalkingAgent *> WalkingAgents;
    Random walkingRng;

    //
    int WorldOffsetX;
    int WorldOffsetY;
    int WorldOffsetZ;

    LightVisibilityCache6 * MyLight;
    LightVisibilityCache6 * MyLightV;

    //
    std::vector<Tile*> Saplings;
    std::vector<Tile*> Shrubs;



    //
    std::vector<std::pair<int, int> > DFDwarvesAllowedCells;



    //
    double LastVisibilityProcessTime;
    double ClearVisibilityProcessTime;
    int NVisibilityTimes;

    ////
    int NTraceVisibilities;

    double TraceVisibilityCachedTime;
    int NTraceVisibilityCachedTime;
    double TraceVisibilityCachedAsMemoryTime;
    int NTraceVisibilityCachedAsMemoryTime;




    //
    Universe();    
    ~Universe();

    //
    void LoadDFMap();

    //
    void CreateVisibilityCache(int i, int j, int k, int dir, int maxLayers);

    //
    LightVisibilityCacheStruct * GetOrCreateVisibilityCachedLight(int stx, int sty, int stz, int dir, int maxLayersToCache);

    //
    void TraceVisibilityCached(LightVisibilityCache1 * light1);









    //
    void TraceVisibilityCached6(LightVisibilityCache6 * light6);

    //
    void PlaceLight1ToMap(LightVisibilityCache1 * light1);

    //
    void PlaceLight6ToMap(LightVisibilityCache6 * light6);

    //
    void RemoveLight1FromMap(LightVisibilityCache1 * light1);

    //
    void RemoveLight6FromMap(LightVisibilityCache6 * light6);

    //
    void ChangeLight1Color(LightVisibilityCache1 * light1, MyColor & newColor);

    //
    void ChangeLight6Color(LightVisibilityCache6 * light6, MyColor & newColor);

    //
    void TraceVisibilityCachedAsMemory(LightVisibilityCache1 * light1, bool addToLightedByMemoryLights);

    //
    void TraceVisibilityCached6AsMemory(LightVisibilityCache6 * light6, bool addToLightedByMemoryLights);

    //
    void PlaceMemoryLight1ToMap(LightVisibilityCache1 * light1);

    //
    void PlaceMemoryLight6ToMap(LightVisibilityCache6 * light6);

    //
    void RemoveMemoryLight1FromMap(LightVisibilityCache1 * light1);

    //
    void RemoveMemoryLight6FromMap(LightVisibilityCache6 * light6);


    //
    void ChangeDoor(int x, int y, int z);


    //
    void TracePathfindableCells();

    //
    void CreateWalkingAgents();

    void IterateAgent(WalkingAgent * wa);
    void IterateAgents();


    //
    void TrackSaplingsNBushes();
};


}

#endif // UNIVERSE_H
