#include "lightworldmap.h"


namespace dflighting
{


LightWorldMap::LightWorldMap(int xs, int ys, int zs)
{
    x_size = xs;
    y_size = ys;
    z_size = zs;

    LightTiles = new LightVisibilityCacheStruct*[6 * z_size * x_size * y_size];
    for (int i = 0; i < 6 * z_size * x_size * y_size; i++)
        LightTiles[i] = nullptr;
}

//
LightWorldMap::~LightWorldMap()
{
    if (LightTiles != nullptr)
    {
        delete [] LightTiles;
        LightTiles = nullptr;
    }

    ////
    //AllLightCaches.clear();
    AllLightCachesDL.Clear();
}


//
LightVisibilityCacheStruct * LightWorldMap::CreateLightCache(int x, int y, int z, int dir)
{
    LightVisibilityCacheStruct * light = new LightVisibilityCacheStruct(x, y, z, dir);
    light->CurrentCacheTime = MaxCacheTime;
    this->operator ()(x, y, z, dir) = light;

    //AllLightCaches.insert(light);
    int index = AllLightCachesDL.AddItem(light);
    light->DLIndex = index;

    return light;
}

//
void LightWorldMap::DeleteLightCache(LightVisibilityCacheStruct * light)
{
    ////AllLightCaches.erase(light);
    AllLightCachesDL.DeleteItem(light->DLIndex);

    //// tell potential tiles to forget me
    for (Tile * potentialTile : light->PotentialTiles)
        potentialTile->PotentialLightCaches.erase(light->GetIntDPoint());

    ////
    this->operator ()(light->WorldX, light->WorldY, light->WorldZ, light->Dir) = nullptr;
    delete light;
}


//
void LightWorldMap::ProcessLightCaches()
{
    /*
    std::vector<LightVisibilityCacheStruct *> itemsToRemove;

    for (LightVisibilityCacheStruct * lightCache : AllLightCaches)
    {
        if (lightCache->LightsHandled > 0)
        {
            lightCache->CurrentCacheTime = MaxCacheTime;
            continue;
        }

        ////
        lightCache->CurrentCacheTime -= 1;
        if (lightCache->CurrentCacheTime <= 0)
        {
            //// it's a subject for a removal
            itemsToRemove.push_back(lightCache);
        }
    }

    for (LightVisibilityCacheStruct * lightCache : itemsToRemove)
        DeleteLightCache(lightCache);
        */

    if (AllLightCachesDL.Count() == 0)
        return;

    int element = AllLightCachesDL.First();
    while (element != -1)
    {
        MyDoubleLinkedListElement<LightVisibilityCacheStruct> * currentItem = AllLightCachesDL.operator ()(element);
        LightVisibilityCacheStruct * lightCache = currentItem->Data;

        element = currentItem->Next;

        if (lightCache->LightsHandled > 0)
        {
            //// update time
            lightCache->CurrentCacheTime = MaxCacheTime;

            //// remove all potential tiles
            for (Tile * potentialTile : lightCache->PotentialTiles)
                potentialTile->PotentialLightCaches.erase(lightCache->GetIntDPoint());
            lightCache->PotentialTiles.clear();
        }
        else
        {
            lightCache->CurrentCacheTime -= 1;
            if (lightCache->CurrentCacheTime <= 0)
            {
                DeleteLightCache(lightCache);
                //AllLightCachesDL.DeleteItem(currentItem->MyID);
            }
        }
    }
}



}
