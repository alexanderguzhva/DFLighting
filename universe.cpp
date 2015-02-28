#include "universe.h"

#include <string>
#include <fstream>
#include <iostream>

#include "supportsplitstring.h"
#include "myintensity.h"
#include "lightvisibilitycachestruct.h"
#include "colorercell.h"
#include "tile.h"

#include "astarpathfinder.h"

namespace dflighting
{



//
Universe::Universe() : ComputingCells(80)
{
    worldMap = nullptr;
    lightWorldMap = nullptr;
}

//
Universe::~Universe()
{

}



//
void Universe::LoadDFMap()
{
    ////
    std::string filename("stdmap.dfmap");
    //std::string filename("/home/nop/projects/lighting/dfmaps/withdoors/meow2c.dfmap");
    //std::string filename("/home/nop/projects/lighting/dfmaps/withdoors/meow4c.dfmap");
    //std::string filename("/home/nop/projects/lighting/dfmaps/withdoors/meow1c.dfmap");
    std::ifstream ifile(filename.c_str(), std::ios::in);

    if (!ifile.is_open())
        return;

    ////
    std::string sDimensions;
    std::getline(ifile, sDimensions);

    std::vector<std::string> dimensionElements;
    SupportSplitString::Split(sDimensions, ' ', dimensionElements);

    ////
    worldMap = new WorldMap(
                        atoi(dimensionElements[0].c_str()) * 16,
                        atoi(dimensionElements[1].c_str()) * 16,
                        atoi(dimensionElements[2].c_str()));

    ////
    std::string sInorganicMats;
    std::getline(ifile, sInorganicMats);

    int nInorganicMats = atoi(sInorganicMats.c_str());

    for (int i = 0; i < nInorganicMats; i++)
    {
        std::string sInMat;
        std::getline(ifile, sInMat);

        std::vector<std::string> mats;
        SupportSplitString::Split(sInMat, ' ', mats);

        Material * newMat = new Material;
        newMat->index = atoi(mats[0].c_str());
        newMat->name = mats[1];

        worldMap->inorganic_material->push_back(newMat);
    }

    ////
    std::string sOrganicMats;
    std::getline(ifile, sOrganicMats);

    int nOrganicMats = atoi(sOrganicMats.c_str());

    for (int i = 0; i < nOrganicMats; i++)
    {
        std::string sInMat;
        std::getline(ifile, sInMat);

        std::vector<std::string> mats;
        SupportSplitString::Split(sInMat, ' ', mats);

        Material * newMat = new Material;
        newMat->index = atoi(mats[0].c_str());
        newMat->name = mats[1];

        worldMap->organic_material->push_back(newMat);
    }

/*
    for (int x = 0; x < worldMap->x_size; x++)
        for (int y = 0; y < worldMap->y_size; y++)
            for (int z = 0; z < worldMap->z_size; z++)
            {
                Tile * newTile = new Tile;
                newTile->X = x;
                newTile->Y = y;
                newTile->Z = z;
                worldMap->operator ()(x, y, z) = newTile;
                newTile->Flow = nullptr;
                newTile->Tile_Material = TileMaterialType::AIR;
                newTile->Tile_Type = TileType::EMPTY;
                newTile->ModifyPassableFlags();
                newTile->Material_Type = 0;
                newTile->Material_Index = 1;
                newTile->Mat = nullptr;

            }
*/


    ////
    int currentBlockX = 0;
    int currentBlockY = 0;
    int currentBlockZ = 0;
    std::string sLine;
    while (ifile.good())
    {
        std::getline(ifile, sLine);

        std::vector<std::string> data;
        SupportSplitString::Split(sLine, ' ', data);

        if (data.size() == 0)
            continue;

        if (data[0] == "C")
        {
            ////
            Tile * newTile = new Tile;

            int nextItem = 1;
            int localX = atoi(data[nextItem].c_str()); nextItem += 1;
            int localY = atoi(data[nextItem].c_str()); nextItem += 1;

            int globalX = localX + currentBlockX * 16;
            int globalY = localY + currentBlockY * 16;
            int globalZ = currentBlockZ;

            newTile->X = globalX;
            newTile->Y = globalY;
            newTile->Z = globalZ;

            (*worldMap)(globalX, globalY, globalZ) = newTile;

            ////
            if (data[nextItem] == "F")
            {
                nextItem += 1;

                int liquidType = atoi(data[nextItem].c_str()); nextItem += 1;
                int flowSize = atoi(data[nextItem].c_str()); nextItem += 1;

                FlowData * flow = new FlowData;
                flow->Liquid_type = (LiquidType)liquidType;
                flow->Flow_size = flowSize;
                newTile->Flow = flow;
            }

            ////
            if (data[nextItem] == "T")
            {
                nextItem += 1;

                int idt = atoi(data[nextItem].c_str()); nextItem += 1;
                newTile->Tile_Type = (TileType)idt;
                int idm = atoi(data[nextItem].c_str()); nextItem += 1;
                newTile->Tile_Material = (TileMaterialType)idt;
            }


            ////
            newTile->ModifyPassableFlags();


            ////
            if (nextItem == data.size())
                continue;


            ////
            if (data[nextItem] == "f")
            {

            }
            else if (data[nextItem] == "s")
            {
                nextItem += 1;

                int z0 = atoi(data[nextItem].c_str()); nextItem += 1;
                int mat = atoi(data[nextItem].c_str()); nextItem += 1;

                newTile->Material_Type = 0;
                newTile->Material_Index = mat;
                newTile->Mat = (*worldMap->inorganic_material)[mat];
            }
            else if (data[nextItem] == "m")
            {
                nextItem += 1;

                int z0 = atoi(data[nextItem].c_str()); nextItem += 1;
                int mat = atoi(data[nextItem].c_str()); nextItem += 1;

                newTile->Material_Type = 0;
                newTile->Material_Index = mat;
                newTile->Mat = (*worldMap->inorganic_material)[mat];
            }
            else if (data[nextItem] == "c")
            {
                int z0 = atoi(data[nextItem].c_str()); nextItem += 1;
                int z1 = atoi(data[nextItem].c_str()); nextItem += 1;

                newTile->Material_Type = 0;
                newTile->Material_Index = z0;
                newTile->Mat = (*worldMap->inorganic_material)[z0];
            }

        }
        else if (data[0] == "B")
        {
            currentBlockX = atoi(data[1].c_str());
            currentBlockY = atoi(data[2].c_str());
            currentBlockZ = atoi(data[3].c_str());
        }
        else if (data[0] == "Q")
        {
            if (data.size() != 1)
            {
                // forget about plants
                // printf("wow\n");
            }
        }
        else
        {
            break;
        }
    }

    ////
    int nBuildings = atoi(sLine.c_str());

    for (int i = 0; i < nBuildings; i++)
    {
        getline(ifile, sLine);

        std::vector<std::string> data;
        SupportSplitString::Split(sLine, ' ', data);

        if (data.size() == 0)
            continue;

        ////
        int tx1 = atoi(data[0].c_str());
        int ty1 = atoi(data[1].c_str());
        int tz = atoi(data[6].c_str());
        int tt = atoi(data[9].c_str());

        if (tt == 8)
        {
            //// door
            Tile * tile = worldMap->operator ()(tx1, ty1, tz);
            if (tile != nullptr)
            {
                tile->IsDoor = true;
                tile->IsDoorOpened = false;

                tile->ModifyPassableFlags();
            }
        }
    }


    ////
    ifile.close();

}

//
//
void Universe::CreateVisibilityCache(int i, int j, int k, Direction dir, int maxLayers)
{
    MyIntensity color(1);

    ////
    switch(dir)
    {
        case Direction::Left:
            ComputingCells.TraceVisibility3<Direction::Left>(i, j, k, color, maxLayers, worldMap);
            break;
        case Direction::Right:
            ComputingCells.TraceVisibility3<Direction::Right>(i, j, k, color, maxLayers, worldMap);
            break;
        case Direction::Top:
            ComputingCells.TraceVisibility3<Direction::Top>(i, j, k, color, maxLayers, worldMap);
            break;
        case Direction::Bottom:
            ComputingCells.TraceVisibility3<Direction::Bottom>(i, j, k, color, maxLayers, worldMap);
            break;
        case Direction::Up:
            ComputingCells.TraceVisibility3<Direction::Up>(i, j, k, color, maxLayers, worldMap);
            break;
        case Direction::Down:
            ComputingCells.TraceVisibility3<Direction::Down>(i, j, k, color, maxLayers, worldMap);
            break;
    }


    //// create cache
    int lightedTiles = ComputingCells.LightedTilesNext;

    //// I don't want to use vectors as Colorers is a pretty almost read-only thing

    LightVisibilityCacheStruct * lightVis = lightWorldMap->CreateLightCache(i, j, k, dir);
    //lightVis->Colorers = new std::vector<ColorerCell>;
    //lightVis->Colorers.reserve(lightedTiles);
    lightVis->Colorers = new ColorerCell[lightedTiles];
    lightVis->NColorers = lightedTiles;

    //// initialize layers info
    int nLayers = ComputingCells.LayersProcessed;
    lightVis->NLayers = nLayers;
    //lightVis->ColorersIndicesSum.resize(nLayers);
    //for (int i = 0; i < nLayers; i++)
    //    lightVis->ColorersIndicesSum[i] = ComputingCells.LightedTilesSumPerLayer[i];
    lightVis->ColorersIndicesSum = new int[nLayers];
    for (int i = 0; i < nLayers; i++)
        lightVis->ColorersIndicesSum[i] = ComputingCells.LightedTilesSumPerLayer[i];


    //// save colorers
    for (int iCell = 0; iCell < lightedTiles; iCell++ )
    {
        //Tile * cell = worldMap->LightedTiles->operator [](iCell);
        Tile * cell = ComputingCells.LightedTiles[iCell];

        ////
//        ColorerCell cCell;
  //      cCell.TargetWorldTile = cell;
    //    cCell.Intensity = cell->LightValueITraced;

      //  lightVis->Colorers.emplace_back(cCell);
        //lightVis->Colorers.emplace_back(ColorerCell(cell, cell->LightValueITraced));
        ColorerCell & colorerCell = lightVis->Colorers[iCell];
        colorerCell.TargetWorldTile = cell;
        colorerCell.Intensity = cell->LightValueITraced;
    }

    //// clear all
    //ClearVisibility();
    ComputingCells.ClearITraced();
}


//
LightVisibilityCacheStruct * Universe::GetOrCreateVisibilityCachedLight(int stx, int sty, int stz, Direction dir, int maxLayersToCache)
{
    LightVisibilityCacheStruct * light = lightWorldMap->operator ()(stx, sty, stz, dir);

    //// got a cache for this cell?
    if (light == nullptr)
    {
        //// spawn it!!!!
        CreateVisibilityCache(stx, sty, stz, dir, maxLayersToCache);

        //// reload
        light = lightWorldMap->operator ()(stx, sty, stz, dir);
    }
    /*
    else
    {
        //// cache
        if (light->NLayers < maxLayersToCache)
        {
            //// we've got less layers cached than we need. Let's render additional layers!
            CreateVisibilityCache(stx, sty, stz, dir, maxLayersToCache);
        }
    }
    */

    return light;
}




//
void Universe::TraceVisibilityCached(LightVisibilityCache1 * light1)
{
    ////
    struct timeval start, end;
    gettimeofday(&start, NULL);



    ////
    LightVisibilityCacheStruct * lightCache = lightWorldMap->operator ()(light1);
    MyColor & color = light1->Color;

    //// normal case
    for (int i = 0; i < lightCache->NColorers; i++)
    {
        //// color the tile
        ColorerCell * cell = &(lightCache->Colorers[i]);
        Tile * tile = cell->TargetWorldTile;
        tile->ColorValueI += color * cell->Intensity.I;

        //// this tile is affected by this light
        //// right now this check is done only for doors
        if (tile->IsDoor)
        {
            tile->LightedByLights.insert(light1);
            light1->TilesHit.push_back(tile);
        }
//        //// lighted tiles
  //      if (!tile->IsWithinLightedTilesArray)
    //    {
      //      tile->IsWithinLightedTilesArray = true;
        //    worldMap->LightedTiles->push_back(tile);
        //}

    }



    ////
    gettimeofday(&end, NULL);
    long seconds, useconds;

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    double mtime = ((seconds) * 1000 + useconds/1000.0);

    TraceVisibilityCachedTime += mtime;
    NTraceVisibilityCachedTime += 1;
}












void Universe::TraceVisibilityCached6(LightVisibilityCache6 * light6)
{
    //// affect intensities
    TraceVisibilityCached(light6->LeftLight);
    TraceVisibilityCached(light6->RightLight);
    TraceVisibilityCached(light6->TopLight);
    TraceVisibilityCached(light6->BottomLight);
    TraceVisibilityCached(light6->UpLight);
    TraceVisibilityCached(light6->DownLight);
}

//
void Universe::PlaceLight1ToMap(LightVisibilityCache1 * light1)
{
    int stx = light1->WorldX;
    int sty = light1->WorldY;
    int stz = light1->WorldZ;
    Direction dir = light1->Dir;

    //// emit or create caches
    LightVisibilityCacheStruct * lightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, dir, light1->MaxLayers);

    //// do not remove this lightcache from the cache
    LightCachesToRemove.erase(lightCache);


    //// notify cache about lights count
    lightCache->LightsHandled += 1;

    //// affect intensities
    TraceVisibilityCached(light1);
}

//
void Universe::PlaceLight6ToMap(LightVisibilityCache6 * light6)
{
    int stx = light6->WorldX;
    int sty = light6->WorldY;
    int stz = light6->WorldZ;

    //// emit or create caches
    LightVisibilityCacheStruct * leftLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, Direction::Left, light6->LeftLight->MaxLayers);
    LightVisibilityCacheStruct * rightLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, Direction::Right, light6->RightLight->MaxLayers);
    LightVisibilityCacheStruct * topLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, Direction::Top, light6->TopLight->MaxLayers);
    LightVisibilityCacheStruct * bottomLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, Direction::Bottom, light6->BottomLight->MaxLayers);
    LightVisibilityCacheStruct * upLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, Direction::Up, light6->UpLight->MaxLayers);
    LightVisibilityCacheStruct * downLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, Direction::Down, light6->DownLight->MaxLayers);

    //// notify cache about lights count
    leftLightCache->LightsHandled += 1;
    rightLightCache->LightsHandled += 1;
    topLightCache->LightsHandled += 1;
    bottomLightCache->LightsHandled += 1;
    upLightCache->LightsHandled += 1;
    downLightCache->LightsHandled += 1;

    LightVisibilityCacheStruct *lc2 = lightWorldMap->operator ()(light6->TopLight);

    //// affect intensities
    TraceVisibilityCached(light6->LeftLight);
    TraceVisibilityCached(light6->RightLight);
    TraceVisibilityCached(light6->TopLight);
    TraceVisibilityCached(light6->BottomLight);
    TraceVisibilityCached(light6->UpLight);
    TraceVisibilityCached(light6->DownLight);
}






//
void Universe::RemoveLight1FromMap(LightVisibilityCache1 * light1)
{
    int & x = light1->WorldX;
    int & y = light1->WorldY;
    int & z = light1->WorldZ;
    Direction & dir = light1->Dir;
    MyColor & color = light1->Color;

    ////
    LightVisibilityCacheStruct * lightCache = lightWorldMap->operator ()(light1);

    lightCache->LightsHandled -= 1;

    bool shouldRemoveCache = (lightCache->LightsHandled == 0);

    //// clear colorers
    for (int i = 0; i < lightCache->NColorers; i++)
    {
        //// tile
        ColorerCell * cell = &(lightCache->Colorers[i]);

        Tile * tile;
        tile = cell->TargetWorldTile;

        //// remove intensity
        tile->ColorValueI -= color * cell->Intensity.I;

        /*
        //// remove light from this tile. modify it later
        for (vector<LightVisibilityCache1*>::iterator iter = tile->LightedByLights.begin(); iter != tile->LightedByLights.end(); iter++)
        {
            if (*iter == light1)
            {
                if (tile->LightedByLights.size() == 1)
                {
                    tile->LightedByLights.clear();
                }
                else
                {
                    vector<LightVisibilityCache1*>::iterator iterLast = tile->LightedByLights.end() - 1;
                    *iter = *iterLast;
                    tile->LightedByLights.erase(iterLast);
                }
                break;
            }
        }
        */
    }

    for (Tile * tile : light1->TilesHit)
    {
        tile->LightedByLights.erase(light1);
        tile->PotentialLightCaches.insert(light1->GetIntDPoint());

        lightCache->PotentialTiles.insert(tile);
    }


    light1->TilesHit.clear();

    ////
    if (shouldRemoveCache)
        LightCachesToRemove.insert(lightCache);
}



//
void Universe::RemoveLight6FromMap(LightVisibilityCache6 * light6)
{
    RemoveLight1FromMap(light6->LeftLight);
    RemoveLight1FromMap(light6->RightLight);
    RemoveLight1FromMap(light6->TopLight);
    RemoveLight1FromMap(light6->BottomLight);
    RemoveLight1FromMap(light6->UpLight);
    RemoveLight1FromMap(light6->DownLight);
}


//
void Universe::ChangeLight1Color(LightVisibilityCache1 * light1, MyColor & newColor)
{
    ////
    LightVisibilityCacheStruct * lightCache = lightWorldMap->operator ()(light1);
    MyColor color = light1->Color - newColor;

    //// clear colorers
    for (int i = 0; i < lightCache->NColorers; i++)
    {
        //// tile
        ColorerCell * cell = &(lightCache->Colorers[i]);

        Tile * tile;
        tile = cell->TargetWorldTile;

        ////
        auto intensity = cell->Intensity.I;

        //// remove intensity
        tile->ColorValueI -= color * intensity;
    }

    light1->ChangeColor(newColor);
}

//
void Universe::ChangeLight6Color(LightVisibilityCache6 * light6, MyColor & newColor)
{
    ChangeLight1Color(light6->LeftLight, newColor);
    ChangeLight1Color(light6->RightLight, newColor);
    ChangeLight1Color(light6->TopLight, newColor);
    ChangeLight1Color(light6->BottomLight, newColor);
    ChangeLight1Color(light6->UpLight, newColor);
    ChangeLight1Color(light6->DownLight, newColor);

    light6->Color = newColor;
}





//
void Universe::TraceVisibilityCachedAsMemory(LightVisibilityCache1 * light1, bool addToLightedByMemoryLights)
{
    ////
    struct timeval start, end;
    gettimeofday(&start, NULL);


    ////
    LightVisibilityCacheStruct * lightCache = lightWorldMap->operator ()(light1);

    ////
    int maxLayer = light1->MaxLayers;
    int amountOfColorersToDealWith;
    if (lightCache->NLayers <= maxLayer)
    {
        amountOfColorersToDealWith = lightCache->NColorers;
    }
    else
    {
        amountOfColorersToDealWith = lightCache->ColorersIndicesSum[maxLayer];
    }

    ////
    for (int i = 0; i < amountOfColorersToDealWith; i++)
    {
        //// color the tile
        ColorerCell * cell = &(lightCache->Colorers[i]);
        Tile * tile = cell->TargetWorldTile;

        if (addToLightedByMemoryLights)
        {
            if (tile->IsDoor)
            {
                tile->LightedByMemoryLights.insert(light1);
                light1->MTilesHit.push_back(tile);
            }
        }

        //// lighted tiles
        if (!tile->IsMemoryAffected)
        {
            tile->IsMemoryAffected = true;
            tile->MemoryAmount = MaxMemoryTime;


            //worldMap->MemoryTiles.push_back(tile);
            int dlIndex = worldMap->MemoryTilesDL.AddItem(tile);
            tile->DLIndex = dlIndex;

            tile->InitializeCopyForMemory();
        }
        else
        {
            tile->MemoryAmount = MaxMemoryTime;
        }

        tile->ModifyCopyForMemory();
    }


    ////
    gettimeofday(&end, NULL);
    long seconds, useconds;

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    double mtime = ((seconds) * 1000 + useconds/1000.0);

    TraceVisibilityCachedAsMemoryTime += mtime;
    NTraceVisibilityCachedAsMemoryTime += 1;

}

void Universe::TraceVisibilityCached6AsMemory(LightVisibilityCache6 * light6, bool addToLightedByMemoryLights)
{
    //// affect intensities
    TraceVisibilityCachedAsMemory(light6->LeftLight, addToLightedByMemoryLights);
    TraceVisibilityCachedAsMemory(light6->RightLight, addToLightedByMemoryLights);
    TraceVisibilityCachedAsMemory(light6->TopLight, addToLightedByMemoryLights);
    TraceVisibilityCachedAsMemory(light6->BottomLight, addToLightedByMemoryLights);
    TraceVisibilityCachedAsMemory(light6->UpLight, addToLightedByMemoryLights);
    TraceVisibilityCachedAsMemory(light6->DownLight, addToLightedByMemoryLights);
}



//
void Universe::PlaceMemoryLight1ToMap(LightVisibilityCache1 * light1)
{
    int stx = light1->WorldX;
    int sty = light1->WorldY;
    int stz = light1->WorldZ;
    Direction dir = light1->Dir;

    //// emit or create caches
    LightVisibilityCacheStruct * lightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, dir, light1->MaxLayers);

    //// do not remove this lightcache from the cache
    LightCachesToRemove.erase(lightCache);


    //// notify cache about lights count
    lightCache->LightsHandled += 1;

    //// affect intensities
    TraceVisibilityCachedAsMemory(light1, true);
}

//
void Universe::PlaceMemoryLight6ToMap(LightVisibilityCache6 * light6)
{
    int stx = light6->WorldX;
    int sty = light6->WorldY;
    int stz = light6->WorldZ;

    //// emit or create caches
    LightVisibilityCacheStruct * leftLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, Direction::Left, light6->LeftLight->MaxLayers);
    LightVisibilityCacheStruct * rightLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, Direction::Right, light6->RightLight->MaxLayers);
    LightVisibilityCacheStruct * topLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, Direction::Top, light6->TopLight->MaxLayers);
    LightVisibilityCacheStruct * bottomLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, Direction::Bottom, light6->BottomLight->MaxLayers);
    LightVisibilityCacheStruct * upLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, Direction::Up, light6->UpLight->MaxLayers);
    LightVisibilityCacheStruct * downLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, Direction::Down, light6->DownLight->MaxLayers);

    //// notify cache about lights count
    leftLightCache->LightsHandled += 1;
    rightLightCache->LightsHandled += 1;
    topLightCache->LightsHandled += 1;
    bottomLightCache->LightsHandled += 1;
    upLightCache->LightsHandled += 1;
    downLightCache->LightsHandled += 1;

    //LightVisibilityCacheStruct *lc2 = lightWorldMap->operator ()(light6->TopLight);

    //// affect intensities
//    TraceVisibilityCachedAsMemory(light6->LeftLight, true);
//    TraceVisibilityCachedAsMemory(light6->RightLight, true);
//    TraceVisibilityCachedAsMemory(light6->TopLight, true);
//    TraceVisibilityCachedAsMemory(light6->BottomLight, true);
//    TraceVisibilityCachedAsMemory(light6->UpLight, true);
//    TraceVisibilityCachedAsMemory(light6->DownLight, true);
    TraceVisibilityCached6AsMemory(light6, true);
}


//
void Universe::RemoveMemoryLight1FromMap(LightVisibilityCache1 * light1)
{
    int & x = light1->WorldX;
    int & y = light1->WorldY;
    int & z = light1->WorldZ;
    Direction & dir = light1->Dir;
    MyColor & color = light1->Color;

    ////
    LightVisibilityCacheStruct * lightCache = lightWorldMap->operator ()(light1);

    lightCache->LightsHandled -= 1;

    bool shouldRemoveCache = (lightCache->LightsHandled == 0);

    //// clear colorers
    for (int i = 0; i < lightCache->NColorers; i++)
    {
        //// tile
        ColorerCell * cell = &(lightCache->Colorers[i]);

        Tile * tile = cell->TargetWorldTile;

        /*
        //// remove light from this tile. modify it later
        for (vector<LightVisibilityCache1*>::iterator iter = tile->LightedByMemoryLights.begin(); iter != tile->LightedByMemoryLights.end(); iter++)
        {
            if (*iter == light1)
            {
                if (tile->LightedByMemoryLights.size() == 1)
                {
                    tile->LightedByMemoryLights.clear();
                }
                else
                {
                    vector<LightVisibilityCache1*>::iterator iterLast = tile->LightedByMemoryLights.end() - 1;
                    *iter = *iterLast;
                    tile->LightedByMemoryLights.erase(iterLast);
                }
                break;
            }
        }*/
    }

    for (Tile * tile : light1->MTilesHit)
    {
        tile->LightedByMemoryLights.erase(light1);
        tile->PotentialLightCaches.insert(light1->GetIntDPoint());
        lightCache->PotentialTiles.insert(tile);
    }

    light1->MTilesHit.clear();

    ////
    if (shouldRemoveCache)
        LightCachesToRemove.insert(lightCache);
}


//
void Universe::RemoveMemoryLight6FromMap(LightVisibilityCache6 * light6)
{
    RemoveMemoryLight1FromMap(light6->LeftLight);
    RemoveMemoryLight1FromMap(light6->RightLight);
    RemoveMemoryLight1FromMap(light6->TopLight);
    RemoveMemoryLight1FromMap(light6->BottomLight);
    RemoveMemoryLight1FromMap(light6->UpLight);
    RemoveMemoryLight1FromMap(light6->DownLight);
}


//
void Universe::ChangeDoor(int x, int y, int z)
{
    Tile * tile = (*worldMap)(x, y, z);

    if (!tile->IsDoor)
        return;

    std::vector<LightVisibilityCache1*> uset(tile->LightedByLights.begin(), tile->LightedByLights.end());
    std::vector<LightVisibilityCache1*> usetm(tile->LightedByMemoryLights.begin(), tile->LightedByMemoryLights.end());

    ////
    LightCachesToRemove.clear();

    ////
    for (auto light1 : uset)
        RemoveLight1FromMap(light1);
    for (auto light1 : usetm)
        RemoveMemoryLight1FromMap(light1);

    tile->IsDoorOpened = !tile->IsDoorOpened;
    tile->ModifyPassableFlags();

    if (tile->IsMemoryAffected)
        tile->ModifyCopyForMemory();

    //// remove all caches associated with this tile
//    for (auto lightCache : tile->LightedByCaches)
  //      LightCachesToRemove.insert(lightCache);
    //tile->LightedByCaches.clear();

    // remove all potential light caches
    std::vector<MyIntDPoint> ptset(tile->PotentialLightCaches.begin(), tile->PotentialLightCaches.end());
    for (MyIntDPoint point : ptset)
    {
        LightVisibilityCacheStruct * potentialCache = lightWorldMap->operator ()(point);
        if (potentialCache != nullptr)
        {
            //// kill it
            lightWorldMap->DeleteLightCache(potentialCache);
        }
    }

    //// remove all removed caches
//    for(auto lightCache : LightCachesToRemove)
  //      lightWorldMap->DeleteLightCache(lightCache);

//    if (tile->IsDoorOpened)
    {
        for (auto light1 : usetm)
            PlaceMemoryLight1ToMap(light1);
        for (auto light1 : uset)
            PlaceLight1ToMap(light1);
    }
}


//
void Universe::TracePathfindableCells()
{
    for (int i = 0; i < worldMap->x_size; i++)
    {
        for (int j = 0; j < worldMap->y_size; j++)
        {
            for (int k = 0; k < worldMap->z_size; k++)
            {
                Tile * tile = (*worldMap)(i, j, k);
                if (tile == nullptr)
                    continue;

                if (tile->IsPathfinded)
                    PathfindableCells.push_back(tile);
            }
        }
    }
}


//
////
void Universe::CreateWalkingAgents()
{
    for (int i = 0; i < WALKING_DWARFS_COUNT; i++)
    {
        auto wa = new WalkingAgent();

        //// create a starting point
        //int id = myrandom(&WalkingRandseed, PathfindableCells.size());
        int id = walkingRng.next(PathfindableCells.size());
        int x = PathfindableCells[id]->X;
        int y = PathfindableCells[id]->Y;
        int z = PathfindableCells[id]->Z;

        wa->X = x;
        wa->Y = y;
        wa->Z = z;

        //// random picture
        //int pic = myrandom(&WalkingRandseed, DFDwarvesAllowedCells.size());
        int pic = walkingRng.next(DFDwarvesAllowedCells.size());
        wa->XPicture = DFDwarvesAllowedCells[pic].first;
        wa->YPicture = DFDwarvesAllowedCells[pic].second;

        WalkingAgents.push_back(wa);
    }
}



//
void Universe::IterateAgent(WalkingAgent * wa)
{
//    return;
    bool generateNewPath = false;
    if (wa->WalkingPath != nullptr)
    {
        wa->CurrentTile++;
        if (wa->CurrentTile == wa->WalkingPath->size())
        {
            //// reached the end
            generateNewPath = true;
        }
        else
        {
            wa->X = wa->WalkingPath->operator [](wa->CurrentTile)->X;
            wa->Y = wa->WalkingPath->operator [](wa->CurrentTile)->Y;
            wa->Z = wa->WalkingPath->operator [](wa->CurrentTile)->Z;
        }
    }
    else
    {
        generateNewPath = true;
    }

    ////
    if (!generateNewPath)
        return;

    ////
    if (wa->WalkingPath != nullptr)
        delete wa->WalkingPath;

    //// select random destination
    //int id = myrandom(&WalkingRandseed, PathfindableCells.size());
    int id = walkingRng.next(PathfindableCells.size());
    int x = PathfindableCells[id]->X;
    int y = PathfindableCells[id]->Y;
    int z = PathfindableCells[id]->Z;


    auto newPath = AStarPathFinder::FindPath(
                worldMap->operator ()(wa->X, wa->Y, wa->Z),
                PathfindableCells[id]);

    wa->WalkingPath = newPath;
    wa->CurrentTile = 0;
}


//
void Universe::IterateAgents()
{
    for (auto wa : WalkingAgents)
        IterateAgent(wa);
}


//
void Universe::TrackSaplingsNBushes()
{
    for (int i = 30; i <= 150; i++)
        for (int j = 30; j <= 150; j++)
            for (int k = 38; k <= 42; k++)
            {
                Tile * tile = (*worldMap)(i, j, k);
                if (tile == nullptr)
                    continue;

                ////
                if (tile->Tile_Type == TileType::SAPLING)
                    Saplings.push_back(tile);

                if (tile->Tile_Type == TileType::SHRUB)
                    Shrubs.push_back(tile);
            }
}


}
