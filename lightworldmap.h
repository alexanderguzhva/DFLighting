#ifndef LIGHTWORLDMAP_H
#define LIGHTWORLDMAP_H


#include "lightvisibilitycachestruct.h"
#include "mydoublelinkedlist.h"
#include "myintdpoint.h"
#include "lightvisibilitycache1.h"
#include "direction.h"

namespace dflighting
{

//
const int MaxCacheTime = 200;


//
class LightWorldMap
{
public:
    LightVisibilityCacheStruct ** LightTiles;

    //// std::unordered_set<LightVisibilityCacheStruct*> AllLightCaches;
    MyDoubleLinkedList<LightVisibilityCacheStruct> AllLightCachesDL;

    uint32_t x_size;
    uint32_t y_size;
    uint32_t z_size;

    //
    LightWorldMap(int xs, int ys, int zs);
    ~LightWorldMap();

    //
    LightVisibilityCacheStruct *& operator() (int x, int y, int z, Direction dir)
    {
        return LightTiles[x + x_size * (y + y_size * (z + static_cast<int>(dir) * z_size))];
    }

    //
    LightVisibilityCacheStruct *& operator() (MyIntDPoint & point)
    {
        return this->operator ()(point.X, point.Y, point.Z, point.Dir);
    }

    //
    LightVisibilityCacheStruct *& operator() (LightVisibilityCache1 & light1)
    {
        return this->operator ()(light1.WorldX, light1.WorldY, light1.WorldZ, light1.Dir);
    }

    //
    LightVisibilityCacheStruct *& operator() (LightVisibilityCache1 *& light1)
    {
        return this->operator ()(light1->WorldX, light1->WorldY, light1->WorldZ, light1->Dir);
    }

    //
    LightVisibilityCacheStruct * CreateLightCache(int x, int y, int z, Direction dir);

    //
    void DeleteLightCache(LightVisibilityCacheStruct * light);

    //
    void ProcessLightCaches();
};




}

#endif // LIGHTWORLDMAP_H
