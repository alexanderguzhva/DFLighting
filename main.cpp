//// initial version by alexanderguzhva@gmail.com


//// I hate it, but has to implement it
//#define EGA_COLORS

//// I used up to 80. 80 takes a lot(LOT) of time to generate and is 29M file, btw
//#define GENERATE_DEPTH_FILE
#define GENERATE_RENDER_DEPTH 40


#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <queue>
#include <stack>

#include <time.h>

#include <fstream>
#include <sstream>

#include <thread>

using namespace std;

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_gfxBlitFunc.h>
#include <SDL/SDL_gfxPrimitives.h>

#include <sys/time.h>


#include "random.h"
#include "mydoublelinkedlist.h"
#include "mycolor.h"
#include "myintensity.h"
#include "egacolorsmapper.h"
#include "myintpoint.h"
#include "myintdpoint.h"

#include "mydoublepoint.h"
#include "mydoubleeqpoint.h"

#include "mapcomputecell.h"

#include "my3dface.h"
#include "my3dfacedcell.h"
#include "mapcomputecellfacedmap.h"

#include "supportsplitstring.h"

#include "tilestructures.h"
#include "tile.h"
#include "worldmap.h"

#include "colorercell.h"
#include "lightvisibilitycachestruct.h"
#include "lightvisibilitycache1.h"
#include "lightvisibilitycache6.h"
#include "lightworldmap.h"

#include "astarpath.h"
#include "astarpriorityqueue.h"
#include "astarpathfinder.h"

#include "walkingagent.h"

#include "mapcomputecells.h"

#include "universe.h"

using namespace dflighting;



const int WorldXSize = 20;
const int WorldYSize = 20;
const int WorldZSize = 20;


dflighting::EGAColorsMapper EGACMapper;

dflighting::Universe Unv;























//
SDL_Surface * Surf_Display;


//
bool Initialize()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        return false;
    }

    int width = 1024;
    int height = 576;

    if((Surf_Display = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL)
    {
        fprintf( stderr, "Video mode set failed: %s\n",
                     SDL_GetError( ) );

        return false;
    }

    return true;
}






//
void ShutDown()
{
    if (Surf_Display != NULL)
        SDL_FreeSurface(Surf_Display);
    SDL_Quit();
}






//
SDL_Surface * DFPics;
void LoadDFPng()
{
#ifdef EGA_COLORS
    //std::string filename("/home/nop/projects/lighting/dfimages/curses_square_16x16_m.png");
    std::string filename("curses_square_16x16_m.png");
#else
    //std::string filename("/home/nop/projects/lighting/dfimages/ph.bmp");
    std::string filename("ph.bmp");
#endif
    DFPics = IMG_Load(filename.c_str());
}




///////////
/////// A* from Eric Lippert








////
SDL_Surface * DFDwarvesPics;
void LoadDFDwarvesPng()
{
    //std::string filename("/home/nop/projects/lighting/dfimages/dwarves.png");
    std::string filename("dwarves.png");
    DFDwarvesPics = IMG_Load(filename.c_str());
}


//

void LoadDFDwarvesSetBannedCells()
{
    std::vector<std::pair<int, int> > DFDwarvesBannedCells;
    DFDwarvesBannedCells.push_back(std::pair<int, int>(0, 6));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(0, 7));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(1, 8));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(2, 5));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(2, 6));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(2, 7));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(3, 6));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(3, 7));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(3, 8));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(3, 9));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(4, 1));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(4, 2));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(4, 3));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(4, 4));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(4, 5));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(4, 6));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(5, 4));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(5, 5));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(5, 6));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(6, 4));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(6, 5));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(7, 6));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(7, 7));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(8, 6));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(8, 7));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(8, 8));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(11, 8));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(11, 9));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(11, 10));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(11, 11));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(18, 10));
    for (int i = 0; i <= 11; i++)
        DFDwarvesBannedCells.push_back(std::pair<int, int>(19, 11));
    DFDwarvesBannedCells.push_back(std::pair<int, int>(20, 10));

    for (int i = 0; i < 21; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            //// check if allowed
            bool allowed = true;
            for (int k = 0; k < DFDwarvesBannedCells.size(); k++)
                if (DFDwarvesBannedCells[k].first == j &&
                    DFDwarvesBannedCells[k].second == i)
                {
                    allowed = false;
                    break;
                }

            if (!allowed)
                continue;

            Unv.DFDwarvesAllowedCells.push_back(std::pair<int, int>(j, i));
        }
    }
}









//
void ProcessKeyDown(SDL_Event * sdlEvent)
{
    int minShift = 1;
    int maxShift = 8;

    int offsetX = 0;
    int offsetY = 0;
    int offsetZ = 0;

    bool recalculateLighting = false;

    //PrintKeyInfo( &(sdlEvent->key) );
    SDL_KeyboardEvent & key = sdlEvent->key;
    switch(sdlEvent->key.keysym.sym)
    {
        case SDLK_UP:
        case SDLK_KP8:
            offsetY -= (key.keysym.mod & KMOD_SHIFT) ? maxShift : minShift;
            recalculateLighting = true;
            break;
        case SDLK_DOWN:
        case SDLK_KP2:
            offsetY += (key.keysym.mod & KMOD_SHIFT) ? maxShift : minShift;
            recalculateLighting = true;
            break;
        case SDLK_RIGHT:
        case SDLK_KP6:
            offsetX += (key.keysym.mod & KMOD_SHIFT) ? maxShift : minShift;
            recalculateLighting = true;
            break;
        case SDLK_LEFT:
        case SDLK_KP4:
            offsetX -= (key.keysym.mod & KMOD_SHIFT) ? maxShift : minShift;
            recalculateLighting = true;
            break;
        case SDLK_COMMA:
            offsetZ += 1;
            recalculateLighting = true;
            break;
        case SDLK_PERIOD:
            offsetZ -= 1;
            recalculateLighting = true;
            break;
//        case SDLK_q:
            //// change to floor. banned it somewhy? maybe was debugging smth
//            LightCachesToRemove.clear();
  //          ChangeTileType2(WorldOffsetX, WorldOffsetY, WorldOffsetZ, TileType::FLOOR, TileMaterialType::SOIL);
    //        for(auto light : LightCachesToRemove)
      //          lightWorldMap->DeleteLight(light);
            break;
//        case SDLK_w:
        //// change to wall. banned it somewhy? maybe was debugging smth
//            LightCachesToRemove.clear();
//
  //          ChangeTileType2(WorldOffsetX, WorldOffsetY, WorldOffsetZ, TileType::WALL, TileMaterialType::LAVA_STONE);
//
  //          for(auto light : LightCachesToRemove)
    //            lightWorldMap->DeleteLight(light);
            break;
//        case SDLK_c:
//            LightCachesToRemove.clear();
//
  //          ChangeDoor(WorldOffsetX, WorldOffsetY, WorldOffsetZ);
//
  //          for(auto light : LightCachesToRemove)
    //            lightWorldMap->DeleteLight(light);
            break;

        case SDLK_i:
            ////
            Unv.IterateAgents();
            break;
        case SDLK_SPACE:
            break;
        default:
            break;
    }



    ////
    if (recalculateLighting)
    {
        ////
        Unv.LastVisibilityProcessTime = 0;
        Unv.NVisibilityTimes = 0;


        ////
        //ClearVisibility();

  //      RenderPointSourceCached(WorldOffsetX, WorldOffsetY, WorldOffsetZ, -20000);

        Unv.RemoveMemoryLight6FromMap(Unv.MyLight);
        delete Unv.MyLight;
        Unv.RemoveLight6FromMap(Unv.MyLightV);
        delete Unv.MyLightV;

        Unv.WorldOffsetX += offsetX;
        Unv.WorldOffsetY += offsetY;
        Unv.WorldOffsetZ += offsetZ;

        Unv.MyLight = new LightVisibilityCache6(Unv.WorldOffsetX, Unv.WorldOffsetY, Unv.WorldOffsetZ, MyColor(50000, 50000, 50000));
        Unv.PlaceMemoryLight6ToMap(Unv.MyLight);

        Unv.MyLightV = new LightVisibilityCache6(Unv.WorldOffsetX, Unv.WorldOffsetY, Unv.WorldOffsetZ, MyColor(10000, 10000, 10000));
        Unv.PlaceLight6ToMap(Unv.MyLightV);

  //      RenderPointSourceCached(WorldOffsetX, WorldOffsetY, WorldOffsetZ, +20000);

        /*
        TraceVisibility2(WorldOffsetX, WorldOffsetY, WorldOffsetZ, 0);
        TraceVisibility2(WorldOffsetX, WorldOffsetY, WorldOffsetZ, 1);
        TraceVisibility2(WorldOffsetX, WorldOffsetY, WorldOffsetZ, 2);
        TraceVisibility2(WorldOffsetX, WorldOffsetY, WorldOffsetZ, 3);
        TraceVisibility2(WorldOffsetX, WorldOffsetY, WorldOffsetZ, 4);
        TraceVisibility2(WorldOffsetX, WorldOffsetY, WorldOffsetZ, 5);

        ////
        NVisibilityTimes += 1;
        */
    }

}



//
void ProcessMouseButtonDown(SDL_Event * sdlEvent)
{
    int xCellSize = 16;
    int yCellSize = 16;

    int nx = Surf_Display->w / xCellSize;
    int ny = Surf_Display->h / yCellSize;

    ////
    int centerx = nx / 2;
    int centery = ny / 2;

    int cpx = - centerx + Unv.WorldOffsetX + sdlEvent->motion.x / xCellSize;
    int cpy = - centery + Unv.WorldOffsetY + sdlEvent->motion.y / yCellSize;
    int cpz = Unv.WorldOffsetZ;

//    LightCachesToRemove.clear();

    Unv.ChangeDoor(cpx, cpy, cpz);

//    for(auto light : LightCachesToRemove)
  //      lightWorldMap->DeleteLight(light);


}




//
void Tile2ImageXY(Tile * tile, int * x, int * y)
{
    //// water or magma
    if (tile->Flow != nullptr)
    {
        *x = (int)tile->Flow->Flow_size;
        *y = 3;
        return;
    }

    //// non-mineral stone
    if (tile->Tile_Type == TileType::WALL && tile->Tile_Material == TileMaterialType::STONE)
    {
        *x = 3;
        *y = 8;
        return;
    }

    //// mineral stone
    if (tile->Tile_Type == TileType::WALL && tile->Tile_Material == TileMaterialType::MINERAL)
    {
        *x = 10;
        *y = 2;
        return;
    }

    ////
    if (tile->Tile_Type == TileType::FLOOR )
    {
        *x = 12;
        *y = 2;
        return;
    }

    ////
    if (tile->Tile_Type == TileType::STAIR_UP)
    {
        *x = 12;
        *y = 3;
        return;
    }

    ////
    if (tile->Tile_Type == TileType::STAIR_DOWN)
    {
        *x = 14;
        *y = 3;
        return;
    }

    ////
    if (tile->Tile_Type == TileType::STAIR_UPDOWN)
    {
        *x = 8;
        *y = 5;
        return;
    }

    ////
    if (tile->Tile_Type == TileType::WALL && tile->Tile_Material == TileMaterialType::LAVA_STONE)
    {
        *x = 3;
        *y = 8;
        return;
    }

    if (tile->Tile_Type == TileType::EMPTY)
    {
        *x = 0;
        *y = 2;
        return;
    }

    if (tile->Tile_Type == TileType::TREE)
    {
        *x = 2;
        *y = 8;
        //*x = 5;
        //*y = 0;
        return;
    }

    //// wall
    //// x = 7, y = 12

    //// floor:
    //// ramp up: 14,1
    //// ramp down: 15,1

    //// unknown: 15, 3
    //// lava: 0, 11
    //// water: 14, 7

    if (tile->Tile_Type == TileType::RAMP_TOP)
    {
        *x = 15;
        *y = 1;
        return;
    }

    if (tile->Tile_Type == TileType::RAMP)
    {
        *x = 14;
        *y = 1;
        return;
    }

    if (tile->Tile_Type == TileType::SAPLING)
    {
        *x = 12;
        *y = 1;
        return;
    }

    if (tile->Tile_Type == TileType::SHRUB)
    {
        *x = 2;
        *y = 2;
        return;
    }

    if (tile->Tile_Type == TileType::PEBBLES)
    {
        *x = 8;
        *y = 15;
        return;
    }

    if (tile->Tile_Type == TileType::BOULDER)
    {
        *x = 7;
        *y = 0;
        return;
    }

    *x = 15;
    *y = 3;
}


void Render()
{
    //// 112 89 40
    //Tile * tTile = worldMap->operator ()(112, 89, 40);

    int xCellSize = 16;
    int yCellSize = 16;

    int nx = Surf_Display->w / xCellSize;
    int ny = Surf_Display->h / yCellSize;

    ////
    int centerx = nx / 2;
    int centery = ny / 2;

    ////
    const float ambientLight = 0.25f;

    for (int ix = 0; ix < nx; ix++)
    {
        for (int iy = 0; iy < ny; iy++)
        {
            int cpx = ix - centerx + Unv.WorldOffsetX;
            int cpy = iy - centery + Unv.WorldOffsetY;
            int cpz = Unv.WorldOffsetZ;

            ////
            int imagex = 0;
            int imagey = 2;

            //float lightlevel = -1;
            MyColor lightlevelI;

            bool pathfinded = false;
            bool lightCached = false;

            bool lightCached0 = false;
            bool lightCached1 = false;
            bool lightCached2 = false;
            bool lightCached3 = false;
            bool lightCached4 = false;
            bool lightCached5 = false;

            bool door = false;
            bool doorOpened = false;

            bool isMemorized = false;
            bool isMemorizedFresh = false;

            Tile * tile = nullptr;

            int memoryAmount = 0;

            if (cpx >= 0 && cpx < Unv.worldMap->x_size &&
                cpy >= 0 && cpy < Unv.worldMap->y_size &&
                cpz >= 0 && cpz < Unv.worldMap->z_size)
            {
                tile = (*(Unv.worldMap))(cpx, cpy, cpz);
                if (tile != nullptr)
                {
                    Tile2ImageXY(tile, &imagex, &imagey);
                    //lightlevel = tile->LightValueF;
                    lightlevelI = tile->ColorValueI;

                    pathfinded = tile->IsPathfinded;

                    door = tile->IsDoor;
                    doorOpened = tile->IsDoorOpened;

                    isMemorized = tile->IsMemoryAffected;
                    isMemorizedFresh = (tile->MemoryAmount == MaxMemoryTime);

                    memoryAmount = tile->MemoryAmount;
                }

                ////
                lightCached0 = (Unv.lightWorldMap->operator ()(cpx, cpy, cpz, Direction::Left) != nullptr);
                lightCached1 = (Unv.lightWorldMap->operator ()(cpx, cpy, cpz, Direction::Right) != nullptr);
                lightCached2 = (Unv.lightWorldMap->operator ()(cpx, cpy, cpz, Direction::Top) != nullptr);
                lightCached3 = (Unv.lightWorldMap->operator ()(cpx, cpy, cpz, Direction::Bottom) != nullptr);
                lightCached4 = (Unv.lightWorldMap->operator ()(cpx, cpy, cpz, Direction::Up) != nullptr);
                lightCached5 = (Unv.lightWorldMap->operator ()(cpx, cpy, cpz, Direction::Down) != nullptr);
            }



            if (door)
            {
                if (!doorOpened)
                {
                    imagex = 5;
                    imagey = 12;
                }
            }


            ////
            SDL_Rect rect;
            rect.x = imagex * xCellSize;
            rect.y = imagey * yCellSize;
            rect.w = xCellSize;
            rect.h = yCellSize;

            SDL_Rect destRect;
            destRect.x = ix * xCellSize;
            destRect.y = iy * yCellSize;
            destRect.w = xCellSize;
            destRect.h = yCellSize;

            if (isMemorized <= 0)
            {
                boxRGBA(Surf_Display,
                        destRect.x,
                        destRect.y,
                        destRect.x + xCellSize - 1,
                        destRect.y + yCellSize - 1,
                        0,
                        0,
                        0,
                        255);
                continue;
            }



            if (!isMemorizedFresh)
            {
                if (tile != nullptr && tile->CopyForMemory != nullptr)
                {
                    Tile2ImageXY(tile->CopyForMemory, &imagex, &imagey);
                    //lightlevel = tile->LightValueF;
                    lightlevelI = tile->CopyForMemory->ColorValueI;

                    pathfinded = tile->CopyForMemory->IsPathfinded;

                    door = tile->CopyForMemory->IsDoor;
                    doorOpened = tile->CopyForMemory->IsDoorOpened;
                }
            }

            if (door)
            {
                if (!doorOpened)
                {
                    imagex = 5;
                    imagey = 12;
                }
            }

/*
            boxRGBA(Surf_Display,
                    destRect.x,
                    destRect.y,
                    destRect.x + xCellSize - 1,
                    destRect.y + yCellSize - 1,
                    0,
                    0,
                    0,
                    255);
*/

            //// go lighting
            //if (lightlevelI.HighEnough())
            {
#ifdef onecolor
                unsigned int cli = 128-min((unsigned int)128, (unsigned int)(lightlevelI.I));

                boxRGBA(Surf_Display,
                        destRect.x,
                        destRect.y,
                        destRect.x + xCellSize - 1,
                        destRect.y + yCellSize - 1,
                        0,
                        0,
                        0,
                        cli);

                /*
                boxRGBA(Surf_Display,
                        destRect.x,
                        destRect.y,
                        destRect.x + xCellSize - 1,
                        destRect.y + yCellSize - 1,
                        255,
                        0,
                        0,
                        min((unsigned int)128, (unsigned int)(lightlevelI.I)));
                */

                /*
                boxRGBA(Surf_Display,
                        destRect.x,
                        destRect.y,
                        destRect.x + xCellSize - 1,
                        destRect.y + yCellSize - 1,
                        0,
                        0,
                        0,
                        255 - min((unsigned int)255, (unsigned int)(lightlevelI.I)));
                */

#else

                int rv = min(255, (int)(lightlevelI.R));
                int gv = min(255, (int)(lightlevelI.G));
                int bv = min(255, (int)(lightlevelI.B));

                float rva = ((float)rv / 255) * (1.0f - ambientLight) + ambientLight;
                float gva = ((float)gv / 255) * (1.0f - ambientLight) + ambientLight;
                float bva = ((float)bv / 255) * (1.0f - ambientLight) + ambientLight;
                if (rva < 0) rva = 0; if (rva > 1) rva = 1;
                if (gva < 0) gva = 0; if (gva > 1) gva = 1;
                if (bva < 0) bva = 0; if (bva > 1) bva = 1;


                if (isMemorized && !isMemorizedFresh)
                {
                    bool bwMode = false;

                    //// draw in BW
                    float mul = (sqrt((double)memoryAmount / MaxMemoryTime));
                    rva = rva * mul;
                    gva = gva * mul;
                    bva = bva * mul;


                    for (int cy = 0; cy < yCellSize; cy++)
                    {
                        for (int cx = 0; cx < xCellSize; cx++)
                        {
                            Uint32 px = ((Uint32*)DFPics->pixels)[DFPics->w * (imagey * yCellSize + cy) + imagex * xCellSize + cx];

                            Uint8 rg;
                            Uint8 gg;
                            Uint8 bg;

                            //rva = 0 -> rg * 0.25
                            //rva = 1 -> 0.5 * rg + 0.5 * 255

                            SDL_GetRGB(px, DFPics->format, &rg, &gg, &bg);

                            //// decaying
                            int luma = (int)(0.2126f * rg * rva + 0.7152 * gg * gva + 0.0722 * bg * bva);
                            if (luma > 255)
                                luma = 255;

                            rg = luma;
                            gg = luma;
                            bg = min(255, luma * 4 / 3);

#ifdef EGA_COLORS

                            rg = (rg * EGACMapper.EGAColorsMappingQuantity) / 256;
                            gg = (gg * EGACMapper.EGAColorsMappingQuantity) / 256;
                            bg = (bg * EGACMapper.EGAColorsMappingQuantity) / 256;

                            MyColor mc = EGACMapper.EGAColorsMapping[(rg * EGACMapper.EGAColorsMappingQuantity + gg) * EGACMapper.EGAColorsMappingQuantity + bg];
                            rg = mc.R;
                            gg = mc.G;
                            bg = mc.B;

#endif
                            ((Uint32*)Surf_Display->pixels)[Surf_Display->w * (iy * yCellSize + cy) + ix * xCellSize + cx] =
                                    SDL_MapRGB(Surf_Display->format, rg, gg, bg);
                        }
                    }
                }
                else
                {
                    //// draw in color

                    for (int cy = 0; cy < yCellSize; cy++)
                    {
                        for (int cx = 0; cx < xCellSize; cx++)
                        {
                            Uint32 px = ((Uint32*)DFPics->pixels)[DFPics->w * (imagey * yCellSize + cy) + imagex * xCellSize + cx];

                            Uint8 rg;
                            Uint8 gg;
                            Uint8 bg;

                            //rva = 0 -> rg * 0.25
                            //rva = 1 -> 0.5 * rg + 0.5 * 255

                            SDL_GetRGB(px, DFPics->format, &rg, &gg, &bg);

                            rg = (Uint8)(rg * rva);
                            gg = (Uint8)(gg * gva);
                            bg = (Uint8)(bg * bva);

#ifdef EGA_COLORS
                            rg = (rg * EGACMapper.EGAColorsMappingQuantity) / 256;
                            gg = (gg * EGACMapper.EGAColorsMappingQuantity) / 256;
                            bg = (bg * EGACMapper.EGAColorsMappingQuantity) / 256;

                            MyColor mc = EGACMapper.EGAColorsMapping[(rg * EGACMapper.EGAColorsMappingQuantity + gg) * EGACMapper.EGAColorsMappingQuantity + bg];
                            rg = mc.R;
                            gg = mc.G;
                            bg = mc.B;
#endif

                            ((Uint32*)Surf_Display->pixels)[Surf_Display->w * (iy * yCellSize + cy) + ix * xCellSize + cx] =
                                    SDL_MapRGB(Surf_Display->format, rg, gg, bg);
                        }
                    }

                }

#endif
            }
            /*
            else
            {

                //SDL_BlitSurface(DFPics, &rect, Surf_Display, &destRect);

                boxRGBA(Surf_Display,
                        destRect.x,
                        destRect.y,
                        destRect.x + xCellSize - 1,
                        destRect.y + yCellSize - 1,
                        0,
                        0,
                        0,
                        128);

            }
*/

            /*
            //// neighbours
            for (auto t : PathfindableCells)
            {
                if (t->X == cpx && t->Y == cpy && t->Z == cpz)
                {
                    boxRGBA(Surf_Display,
                            destRect.x,
                            destRect.y,
                            destRect.x + xCellSize - 1,
                            destRect.y + yCellSize - 1,
                            0,
                            0,
                            255,
                            128);
                }
            }
            */

            /*
            ////
            if (pathfinded)
            {
                boxRGBA(Surf_Display,
                        destRect.x,
                        destRect.y,
                        destRect.x + xCellSize - 1,
                        destRect.y + yCellSize - 1,
                        64,
                        0,
                        64,
                        128);
            }
*/




/*
            if (lightCached0)
                rectangleRGBA(Surf_Display, destRect.x + 0, destRect.y + 0, destRect.x + 3, destRect.y + 3, 128, 64, 0, 192);
            if (lightCached1)
                rectangleRGBA(Surf_Display, destRect.x + 4, destRect.y + 0, destRect.x + 7, destRect.y + 3, 128, 64, 0, 192);
            if (lightCached2)
                rectangleRGBA(Surf_Display, destRect.x + 8, destRect.y + 0, destRect.x + 11, destRect.y + 3, 128, 64, 0, 192);
            if (lightCached3)
                rectangleRGBA(Surf_Display, destRect.x + 0, destRect.y + 4, destRect.x + 3, destRect.y + 7, 128, 64, 0, 192);
            if (lightCached4)
                rectangleRGBA(Surf_Display, destRect.x + 4, destRect.y + 4, destRect.x + 7, destRect.y + 7, 128, 64, 0, 192);
            if (lightCached5)
                rectangleRGBA(Surf_Display, destRect.x + 8, destRect.y + 4, destRect.x + 11, destRect.y + 7, 128, 64, 0, 192);
*/

            //// selector
            if (cpx == Unv.WorldOffsetX && cpy == Unv.WorldOffsetY && cpz == Unv.WorldOffsetZ)
            {
                SDL_Rect destRect;
                destRect.x = ix * xCellSize;
                destRect.y = iy * yCellSize;
                destRect.w = xCellSize;
                destRect.h = yCellSize;


                boxRGBA(Surf_Display,
                        destRect.x,
                        destRect.y,
                        destRect.x + xCellSize - 1,
                        destRect.y + yCellSize - 1,
                        0,
                        255,
                        0,
                        128);
            }
        }
    }

    /*
    SDL_Rect aRect;
    aRect.x = 10 * xCellSize;
    aRect.y = 10 * yCellSize;
    aRect.w = 100;
    aRect.h = 100;
    SDL_SetAlpha(Surf_Display, SDL_SRCALPHA, 64);
    SDL_FillRect(Surf_Display, &aRect, SDL_MapRGBA(Surf_Display->format, 255, 0, 0, 0));
    */
    //boxRGBA(Surf_Display, 100, 100, 200, 200, 255, 0, 0, 63);

    // draw dwarves
    int minx = 0 - centerx + Unv.WorldOffsetX;
    int miny = 0 - centery + Unv.WorldOffsetY;
    int maxx = nx - 1 - centerx + Unv.WorldOffsetX;
    int maxy = ny - 1 - centery + Unv.WorldOffsetY;
    int cz = Unv.WorldOffsetZ;

    for (auto wa : Unv.WalkingAgents)
    {
        ////
        if (wa->Z != cz) continue;
        if (wa->X < minx || wa->X > maxx ||
            wa->Y < miny || wa->Y > maxy)
            continue;

        MyColor lightlevelI;
        Tile * tile = (*(Unv.worldMap))(wa->X, wa->Y, wa->Z);
        if (tile != nullptr)
            lightlevelI = tile->ColorValueI;

        int ix = wa->X - minx;
        int iy = wa->Y - miny;



        int rv = min(255, (int)(lightlevelI.R));
        int gv = min(255, (int)(lightlevelI.G));
        int bv = min(255, (int)(lightlevelI.B));

        float rva = ((float)rv / 255) * (1.0f - ambientLight) + ambientLight;
        float gva = ((float)gv / 255) * (1.0f - ambientLight) + ambientLight;
        float bva = ((float)bv / 255) * (1.0f - ambientLight) + ambientLight;

        for (int cy = 0; cy < yCellSize; cy++)
        {
            for (int cx = 0; cx < xCellSize; cx++)
            {
                Uint32 px = ((Uint32*)DFDwarvesPics->pixels)[DFDwarvesPics->w * (wa->YPicture * yCellSize + cy) + wa->XPicture * xCellSize + cx];
                //Uint32 px = ((Uint32*)DFPics->pixels)[DFPics->w * (0 * yCellSize + cy) + 1 * xCellSize + cx];

                Uint8 rg;
                Uint8 gg;
                Uint8 bg;

                //rva = 0 -> rg * 0.25
                //rva = 1 -> 0.5 * rg + 0.5 * 255

                SDL_GetRGB(px, DFDwarvesPics->format, &rg, &gg, &bg);
                rg = (Uint8)(rg * rva);
                gg = (Uint8)(gg * gva);
                bg = (Uint8)(bg * bva);

                ((Uint32*)Surf_Display->pixels)[Surf_Display->w * (iy * yCellSize + cy) + ix * xCellSize + cx] =
                        SDL_MapRGB(Surf_Display->format, rg, gg, bg);
            }
        }

        /*
        ////
        SDL_Rect rect;
        rect.x = wa->XPicture * xCellSize;
        rect.y = wa->YPicture * yCellSize;
        rect.w = xCellSize;
        rect.h = yCellSize;

        SDL_Rect destRect;
        destRect.x = ix * xCellSize;
        destRect.y = iy * yCellSize;
        destRect.w = xCellSize;
        destRect.h = yCellSize;

        SDL_BlitSurface(DFDwarvesPics, &rect, Surf_Display, &destRect);
        */
    }

    SDL_UpdateRect(Surf_Display, 0, 0, Surf_Display->w, Surf_Display->h);


}




//// for generating max % of light scattered
void generateLightCacheFile()
{
    ////
    Unv.worldMap = new WorldMap(200, 200, 200);


    ///
    Unv.lightWorldMap = new LightWorldMap(200, 200, 200);

    // render light
    LightVisibilityCache1 lc(90, 90, 90, Direction::Left, MyColor(0, 0, 0));
    lc.MaxLayers = GENERATE_RENDER_DEPTH;
    Unv.PlaceLight1ToMap(&lc);

    ////
    int depth = GENERATE_RENDER_DEPTH;
    MapComputeCells mcs(depth);
    mcs.GenerateLinesCombined(65536 * 8 * depth);

    stringstream ss;
    //ss << "/home/nop/projects/lighting/ccs/ccsM";
    ss << "lightmap";
    //ss << (depth);

    ofstream outputCCsFile;
    outputCCsFile.open(ss.str().c_str(), ios_base::binary);
    mcs.SaveToStream(&outputCCsFile);
    outputCCsFile.close();
}


//
int main(int argc, char** argv)
{
#ifdef GENERATE_DEPTH_FILE
    generateLightCacheFile();

    return 0;
#endif



    ////
    if (!Initialize())
        return -1;

    ////
    //main2();
    ////
    //ShutDown();
    //return 0;

    std::cout << "Bug: dwarf sees 8 elements forward, player - 40.";
    std::cout << "But if player goes to a cell that has a lighting cache "
              << "created by dwarf then it will see a light of 8, not 40" <<
                 std::endl;

    EGACMapper.CreateMapping();



    ////
    Unv.LoadDFMap();
    Unv.worldMap->CreateNeighbours();

    LoadDFPng();

    LoadDFDwarvesPng();
    LoadDFDwarvesSetBannedCells();

    //WorldOffsetX = 85;
  //  WorldOffsetY = 150;
//    WorldOffsetZ = 26;

//    WorldOffsetX = 91;
  //  WorldOffsetY = 141;
    //WorldOffsetZ = 30;

//    WorldOffsetX = 101;
  //  WorldOffsetY = 88;
    //WorldOffsetZ = 42;

  //  WorldOffsetX = 113;
 //   WorldOffsetY = 92;
 //   WorldOffsetZ = 40;

//    WorldOffsetX = 106;
//   WorldOffsetY = 93;
//    WorldOffsetZ = 41;

    //WorldOffsetX = 80;
  //  WorldOffsetY = 95;
//    WorldOffsetZ = 42;

    Unv.WorldOffsetX = 109;
    Unv.WorldOffsetY = 79;
    Unv.WorldOffsetZ = 155;


    //return 0;
    //// 100 96 39
    //// 121 99 40



    //LoadUniformPoints();
    //GenerateUniformPoints();
    //GenerateLines();

    /*
    for (int i = 1; i <= 20; i++)
    {
        MapComputeCells mcs(5 * i);
        mcs.GenerateLinesCombined(65536 * 8 * i * 5);

        stringstream ss;
        ss << "/home/nop/projects/lighting/ccs/ccs";
        ss << (5 * i);

        ofstream outputCCsFile;
        outputCCsFile.open(ss.str().c_str(), ios_base::binary);
        mcs.SaveToStream(&outputCCsFile);
        outputCCsFile.close();
    }
    */


    ifstream inputCCsFile;
    //inputCCsFile.open("/home/nop/projects/lighting/ccs/ok/ccs80", ios_base::binary);
    //inputCCsFile.open("/home/nop/projects/lighting/ccs/ccsM40", ios_base::binary);
    inputCCsFile.open("lightmap", ios_base::binary);
    Unv.ComputingCells.LoadFromStream(&inputCCsFile);
    inputCCsFile.close();

    ///
    Unv.lightWorldMap = new LightWorldMap(Unv.worldMap->x_size, Unv.worldMap->y_size, Unv.worldMap->z_size);

/*
 * for generating max % of light scattered
 *
    // render light
    LightVisibilityCache1 lc(90, 90, 90, 0, MyColor(0, 0, 0));
    lc.MaxLayers = 80;
    PlaceLight1ToMap(&lc);

    //// analyze colorers

    ofstream CCsFile;
    CCsFile.open("/home/nop/projects/lighting/ccs/ok/max80");

    for (int i = 0; i < 80; i++)
    {
        int firstItem = lc.LightCache->ColorersIndicesSum[i];
        int lastItem = lc.LightCache->ColorersIndicesSum[i + 1];

        float maxIntensity = 0;
        for (int j = firstItem; j < lastItem; j++)
            if (lc.LightCache->Colorers[j].Intensity.I > maxIntensity)
                maxIntensity = lc.LightCache->Colorers[j].Intensity.I;

        stringstream sss;
        sss << maxIntensity << std::endl;
        CCsFile << sss.str();
        //// cout << maxIntensity << std::endl;
    }

    CCsFile.close();
*/

    //    TraceVisibility();

    //// 120 100 40
    //// 110  90 40


    struct timeval startP, endP;
    gettimeofday(&startP, NULL);


//    Tile * initialTile = worldMap->operator ()(120, 100, 40);
  //  Tile * finalTile = worldMap->operator ()(65, 65, 40);

    //// for file 4c
    //Tile * initialTile = worldMap->operator ()(85, 150, 30);
    //Tile * finalTile = worldMap->operator ()(75, 165, 25);


    Tile * initialTile = Unv.worldMap->operator ()(110, 80, 155);
    Tile * finalTile = Unv.worldMap->operator ()(58, 70, 109);

    auto renderPath = AStarPathFinder::FindPath(initialTile, finalTile);

    ////
    gettimeofday(&endP, NULL);
    double secondsP, usecondsP;

    secondsP  = endP.tv_sec  - startP.tv_sec;
    usecondsP = endP.tv_usec - startP.tv_usec;

    double mtimeP = ((secondsP) * 1000 + usecondsP/1000.0);

    printf("Tracing pathfindable cells took %f msec\n", mtimeP);

    //// trace pathable cells
    Unv.TracePathfindableCells();

    if (renderPath != nullptr)
        delete renderPath;



    //WalkingRandseed = 13123121;
    Unv.walkingRng = dflighting::Random(13123121);
    Unv.CreateWalkingAgents();

    ////
    Unv.LastVisibilityProcessTime = 0;
    Unv.NVisibilityTimes = 0;

    /*
    double vs;

    for (int i = 0; i < 10000; i++)
    {
        RenderRecalcVisibility();
        vs += (*worldMap)(WorldOffsetX + 1, WorldOffsetY + 2, WorldOffsetZ)->LightValueI;
    }

    double visTimesQ = (NVisibilityTimes == 0) ? 0 : LastVisibilityProcessTime / NVisibilityTimes;
    cout << visTimesQ << std::endl;
    cout << vs << std::endl;
    */

    Unv.TrackSaplingsNBushes();

    //RenderRecalcVisibility();
    ////
    Unv.NTraceVisibilities = 0;

    struct timeval start, end;
    gettimeofday(&start, NULL);


    //CreateVisibilityCache();

    ////
    gettimeofday(&end, NULL);
    double seconds, useconds;

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    double mtime = ((seconds) * 1000 + useconds/1000.0);

    printf("Creating cache took %f msec\n", mtime);
    printf("Average %f msec per TraceVisibility2\n", mtime / Unv.NTraceVisibilities);

    Unv.LastVisibilityProcessTime = 0;
    Unv.ClearVisibilityProcessTime = 0;


//    RenderRecalcVisibilityCached(1.0f);
//    RenderPointSourceCached(WorldOffsetX, WorldOffsetY, WorldOffsetZ, 20000);

    //return 0;

//    Tile * tile = (*worldMap)(WorldOffsetX, WorldOffsetY, WorldOffsetZ);
//    cout << tile->LightedBy.size() << std::endl;

//    ClearCache(WorldOffsetX, WorldOffsetY, WorldOffsetZ);
//    ClearVisibility();
  //  RenderRecalcVisibilityCached();


    //
    long nEmptyCells = 0;
    for (int i = 0; i < Unv.worldMap->x_size; i++)
        for (int j = 0; j < Unv.worldMap->y_size; j++)
            for (int k = 0; k < Unv.worldMap->z_size; k++)
            {
                Tile * tile = (*(Unv.worldMap))(i, j, k);
                if (tile == nullptr)
                    continue;

                if (tile->IsPassableByLight)
                    nEmptyCells += 1;
            }

    cout << "Empty cells count = " << nEmptyCells << std::endl;


    //// place static lights
    LightVisibilityCache6 light6_1(110, 72, 155, MyColor(40000, 10000, 10000));
    Unv.PlaceLight6ToMap(&light6_1);

    LightVisibilityCache6 light6_2(110, 88, 155, MyColor(10000, 10000, 40000));
    Unv.PlaceLight6ToMap(&light6_2);

    LightVisibilityCache6 light6_3(90, 76, 155, MyColor(80000, 80000, 80000));
    Unv.PlaceLight6ToMap(&light6_3);

    //// more lights
    LightVisibilityCache6 light6_11(96, 58, 155, MyColor(1400000, 00000, 00000));
    Unv.PlaceLight6ToMap(&light6_11);

    LightVisibilityCache6 light6_12(96, 52, 155, MyColor(00000, 1400000, 00000));
    Unv.PlaceLight6ToMap(&light6_12);

    LightVisibilityCache6 light6_13(96, 46, 155, MyColor(00000, 00000, 1400000));
    Unv.PlaceLight6ToMap(&light6_13);

    //// more lights
    LightVisibilityCache6 light6_21(96, 42, 155, MyColor(5000, 5000, 5000));
    Unv.PlaceLight6ToMap(&light6_21);

    LightVisibilityCache6 light6_22(82, 40, 155, MyColor(15000, 15000, 15000));
    Unv.PlaceLight6ToMap(&light6_22);

    //// large one outside
    LightVisibilityCache6 light6_31(126, 82, 158, MyColor(1115000, 1115000, 1115000));
    Unv.PlaceLight6ToMap(&light6_31);

    ////
    LightVisibilityCache6 light6_d3(72, 62, 155, MyColor(150000, 150000, 150000));
    Unv.PlaceLight6ToMap(&light6_d3);


    //// my light
    Unv.MyLight = new LightVisibilityCache6(Unv.WorldOffsetX, Unv.WorldOffsetY, Unv.WorldOffsetZ, MyColor(50000, 50000, 50000));
    Unv.PlaceMemoryLight6ToMap(Unv.MyLight);
    Unv.MyLightV = new LightVisibilityCache6(Unv.WorldOffsetX, Unv.WorldOffsetY, Unv.WorldOffsetZ, MyColor(10000, 10000, 10000));
    Unv.PlaceLight6ToMap(Unv.MyLightV);

    ////
//    InitializeWorld();

    double dfLightingTime = 0;
    int NDFLighting = 0;

    ////
    SDL_Event sdlEvent;

    bool needToTerminate = false;

    int ttime = 0;




    while (true)
    {
        while (SDL_PollEvent(&sdlEvent))
        {
            switch(sdlEvent.type)
            {
                case SDL_QUIT:
                    needToTerminate = true;
                    break;
                //case SDL_MOUSEMOTION:
                //    ProcessMouseMotion(&sdlEvent);
                //    break;
                case SDL_MOUSEBUTTONDOWN:
                    ProcessMouseButtonDown(&sdlEvent);
                    break;
                case SDL_KEYDOWN:
                    ProcessKeyDown(&sdlEvent);
                    break;
                //case SDL_KEYUP:
                //    ProcessKeyUp(&sdlEvent);
                //    break;
            }
        }

        if (needToTerminate)
            break;


        struct timeval startall, endall;
        gettimeofday(&startall, NULL);


        ////
        Tile * currentTile = Unv.worldMap->operator ()(Unv.WorldOffsetX, Unv.WorldOffsetY, Unv.WorldOffsetZ);

        double visTimes = (Unv.NVisibilityTimes == 0) ? 0 : Unv.LastVisibilityProcessTime / Unv.NVisibilityTimes;
        double dfTimes = (NDFLighting == 0) ? 0 : 1 / (dfLightingTime / 1000 / NDFLighting);
        //double clrTimes = (NVisibilityTimes == 0) ? 0 : ClearVisibilityProcessTime / NVisibilityTimes;
        std::stringstream sTitle;
        sTitle << "DF lighting prj. ";
        //sTitle << "vis " << visTimes << " ms. ";
        //sTitle << "light fps " << dfTimes << ".";
        //sTitle << "clr " << clrTimes << " ms. ";


//        sTitle << "TrV2 " << ((ComputingCells.NTimeTaken == 0) ? 0 : 1 / (ComputingCells.TimeTaken / 1000 / ComputingCells.NTimeTaken)) << " fps. ";
  //      sTitle << "TrVC " << ((NTraceVisibilityCachedTime == 0) ? 0 : 1 / (TraceVisibilityCachedTime / 1000 / NTraceVisibilityCachedTime)) << " fps. ";
    //    sTitle << "TrVMC " << ((NTraceVisibilityCachedAsMemoryTime == 0) ? 0 : 1 / (TraceVisibilityCachedAsMemoryTime / 1000 / NTraceVisibilityCachedAsMemoryTime)) << " fps. ";
        sTitle << "DX = " << Unv.WorldOffsetX << " ";
        sTitle << "DY = " << Unv.WorldOffsetY << " ";
        sTitle << "DZ = " << Unv.WorldOffsetZ << " ";

      //  if (currentTile != nullptr)
    //        sTitle << "L " << currentTile->ColorValueI.R << " "
  //                          << currentTile->ColorValueI.G << " "
//                            << currentTile->ColorValueI.B << " ";

        //sTitle << "M " << worldMap->MemoryTiles->size() << ". ";



        ////
        //RenderRecalcVisibilityCached();

        //// let every dwarf emits light

        struct timeval startd, endd;
        gettimeofday(&startd, NULL);


        //for (auto wa : WalkingAgents)
        //    RenderPointSourceCached(wa->X, wa->Y, wa->Z, 30000);



        double tvf2 = 0.1f * ttime;

        int tx1 = 82 + 6 * cos(tvf2);
        int ty1 = 94 + 6 * sin(tvf2);
        int tx2 = 82 + 6 * cos(tvf2 + 3.14159);
        int ty2 = 94 + 6 * sin(tvf2 + 3.14159);

        LightVisibilityCache6 light6_d1(tx1, ty1, 155, MyColor(0000, 140000, 00000));
        Unv.PlaceLight6ToMap(&light6_d1);

        LightVisibilityCache6 light6_d2(tx2, ty2, 155, MyColor(140000, 00000, 00000));
        Unv.PlaceLight6ToMap(&light6_d2);


        double tvf = cos(0.03f*ttime);
        tvf = tvf*tvf;
        int tvi = (int)(150000 * tvf);
        MyColor newFloatingColor(tvi, tvi, tvi);
        Unv.ChangeLight6Color(&light6_d3, newFloatingColor);

        ttime += 1;

        // 74 145 30 30000
        //RenderPointSourceCached(WorldOffsetX, WorldOffsetY, WorldOffsetZ, 30000);
        //MyColor d(30000);
        //TraceVisibilityCached(87, 74, WorldOffsetZ, d, 0);
        //TraceVisibilityCached(87, 60, WorldOffsetZ, d, 0);

        ////
        gettimeofday(&endd, NULL);
        double secondsd, usecondsd;

        secondsd  = endd.tv_sec  - startd.tv_sec;
        usecondsd = endd.tv_usec - startd.tv_usec;

        double mtimed = ((secondsd) * 1000 + usecondsd/1000.0);


        //sTitle << "MTd " << mtimed << " ms. ";


        dfLightingTime += mtimed;
        NDFLighting += 1;


        /*
        struct timespec sleepTime;
        struct timespec returnTime;

        if (mtimed < 60)
        {
            sleepTime.tv_sec = 0;
            sleepTime.tv_nsec = (long)(60 * 1000000 - mtimed * 1000000);


            nanosleep(&sleepTime, &returnTime);
        }
*/


        ////
        ////
        struct timeval startrt, endrt;
        gettimeofday(&startrt, NULL);

        Render();

        gettimeofday(&endrt, NULL);
        double secondsrt, usecondsrt;

        secondsrt  = endrt.tv_sec  - startrt.tv_sec;
        usecondsrt = endrt.tv_usec - startrt.tv_usec;

        double mtimert = ((secondsrt) * 1000 + usecondsrt/1000.0);








        ////
        Unv.LightCachesToRemove.clear();

        Unv.RemoveLight6FromMap(&light6_d1);
        Unv.RemoveLight6FromMap(&light6_d2);
        //RemoveLight6FromMap(&light6_d3);

        for(auto lightCache : Unv.LightCachesToRemove)
            Unv.lightWorldMap->DeleteLightCache(lightCache);

        ////
        struct timeval startmt, endmt;
        gettimeofday(&startmt, NULL);

        Unv.worldMap->ProcessMemoryTiles();

        ////
        gettimeofday(&endmt, NULL);
        double secondsmt, usecondsmt;

        secondsmt  = endmt.tv_sec  - startmt.tv_sec;
        usecondsmt = endmt.tv_usec - startmt.tv_usec;

        double mtimemt = ((secondsmt) * 1000 + usecondsmt/1000.0);

        //sTitle << "MT " << mtimemt << " ms. ";











        ////
        Unv.LightCachesToRemove.clear();


        //// remove lights
        for (WalkingAgent * wa : Unv.WalkingAgents)
        {
            if (wa->MemoryLight != nullptr)
            {
                Unv.RemoveMemoryLight6FromMap(wa->MemoryLight);
                delete wa->MemoryLight;
            }
        }


        for (WalkingAgent * wa : Unv.WalkingAgents)
        {
            if (wa->Light != nullptr)
            {
                Unv.RemoveLight6FromMap(wa->Light);
                delete wa->Light;
            }
        }

        struct timeval startr, endr;
        gettimeofday(&startr, NULL);

        //for(auto lightCache : LightCachesToRemove)
        //    lightWorldMap->DeleteLightCache(lightCache);
        Unv.lightWorldMap->ProcessLightCaches();

        ////
        gettimeofday(&endr, NULL);
        double secondsr, usecondsr;

        secondsr  = endr.tv_sec  - startr.tv_sec;
        usecondsr = endr.tv_usec - startr.tv_usec;

        double mtimer = ((secondsr) * 1000 + usecondsr/1000.0);


        //sTitle << "MTr " << mtimer << " ms. ";



        Tile * t = Unv.worldMap->operator ()(109, 77, 155);std::vector<MyColor> EGAColors;

        int EGAColorsMappingQuantity = 32;
        MyColor * EGAColorsMapping;

        //sTitle << "Q " << t->LightedByMemoryLights.size() << ". ";





        ////
        Unv.IterateAgents();

        for (WalkingAgent * wa : Unv.WalkingAgents)
        {
            wa->MemoryLight = new LightVisibilityCache6(wa->X, wa->Y, wa->Z, MyColor(5000, 5000, 5000));
            Unv.PlaceMemoryLight6ToMap(wa->MemoryLight);
        }


        for (WalkingAgent * wa : Unv.WalkingAgents)
        {
            wa->Light = new LightVisibilityCache6(wa->X, wa->Y, wa->Z, MyColor(3000, 3000, 3000));
            Unv.PlaceLight6ToMap(wa->Light);
        }


        ////
        Unv.TraceVisibilityCached6AsMemory(Unv.MyLight, false);



        gettimeofday(&endall, NULL);
        double secondsall, usecondsall;

        secondsall  = endall.tv_sec  - startall.tv_sec;
        usecondsall = endall.tv_usec - startall.tv_usec;

        double mtimeall = ((secondsall) * 1000 + usecondsall/1000.0);




        struct timespec sleepTime;
        struct timespec returnTime;


        if (mtimeall < 40)
        {
            sleepTime.tv_sec = 0;
            sleepTime.tv_nsec = (long)(40 * 1000000 - mtimeall * 1000000);


            nanosleep(&sleepTime, &returnTime);
        }



        sTitle << "MTs " << Unv.worldMap->MemoryTilesDL.Count() << " ms. ";
        sTitle << "MT " << mtimemt  << " ms. ";
        //sTitle << "LCaches " << lightWorldMap->AllLightCaches.size() << ". ";
        //sTitle << "LCaches " << lightWorldMap->AllLightCachesDL.Count() << ". ";
        sTitle << "R " << mtimer  << " ms. ";
        sTitle << "Rend " << mtimert  << " ms. ";
        sTitle << "C " << mtimeall - mtimert  << " ms. ";
//        sTitle << "A " << mtimeall << " ms. ";

        ////
        SDL_WM_SetCaption(sTitle.str().c_str(), 0);

    }

     ////
    ShutDown();

    return 0;
}



