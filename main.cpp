//// initial version by alexanderguzhva@gmail.com

//// 0 = disable them
//// otherwise add walking dwarves (sources of light).
//// creates great overhead because of naive A* implementation
#define WALKING_DWARFS_COUNT 2

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

using namespace dflighting;



const int WorldXSize = 20;
const int WorldYSize = 20;
const int WorldZSize = 20;


dflighting::EGAColorsMapper EGACMapper;






















//
int WorldOffsetX;
int WorldOffsetY;
int WorldOffsetZ;


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
int NGenPointsUFEdge = 65536 * 8 * 64;






////
MapComputeCellFacedMap * MapComputeCellsFaced;








//
const int MaxMemoryTime = 200;




//
WorldMap * worldMap;


//
void LoadDFMap()
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
    string sDimensions;
    getline(ifile, sDimensions);

    std::vector<std::string> dimensionElements;
    SupportSplitString::Split(sDimensions, ' ', dimensionElements);

    ////
    worldMap = new WorldMap(
                        atoi(dimensionElements[0].c_str()) * 16,
                        atoi(dimensionElements[1].c_str()) * 16,
                        atoi(dimensionElements[2].c_str()));

    ////
    string sInorganicMats;
    getline(ifile, sInorganicMats);

    int nInorganicMats = atoi(sInorganicMats.c_str());

    for (int i = 0; i < nInorganicMats; i++)
    {
        string sInMat;
        getline(ifile, sInMat);

        std::vector<std::string> mats;
        SupportSplitString::Split(sInMat, ' ', mats);

        Material * newMat = new Material;
        newMat->index = atoi(mats[0].c_str());
        newMat->name = mats[1];

        worldMap->inorganic_material->push_back(newMat);
    }

    ////
    string sOrganicMats;
    getline(ifile, sOrganicMats);

    int nOrganicMats = atoi(sOrganicMats.c_str());

    for (int i = 0; i < nOrganicMats; i++)
    {
        string sInMat;
        getline(ifile, sInMat);

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
    string sLine;
    while (ifile.good())
    {
        getline(ifile, sLine);

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

//
double LastVisibilityProcessTime;
double ClearVisibilityProcessTime;
int NVisibilityTimes;










// find maximum of a and b
#define MAX(a,b) (((a)>(b))?(a):(b))

// absolute value of a
#define ABS(a) (((a)<0) ? -(a) : (a))

// take sign of a, either -1, 0, or 1
#define ZSGN(a) (((a)<0) ? -1 : (a)>0 ? 1 : 0)


class MapComputeCells
{
private:
    //
    static void line3d(int x1, int y1, int z1, int x2, int y2, int z2,
                int xmin, int xmax, int ymin, int ymax, int zmin, int zmax,
                std::vector<MyIntPoint> * vec)
    {
        int xd, yd, zd;
        int x, y, z;
        int ax, ay, az;
        int sx, sy, sz;
        int dx, dy, dz;

        dx = x2 - x1;
        dy = y2 - y1;
        dz = z2 - z1;

        ax = ABS(dx) << 1;
        ay = ABS(dy) << 1;
        az = ABS(dz) << 1;

        sx = ZSGN(dx);
        sy = ZSGN(dy);
        sz = ZSGN(dz);

        x = x1;
        y = y1;
        z = z1;

        if (ax >= MAX(ay, az))            /* x dominant */
        {
            yd = ay - (ax >> 1);
            zd = az - (ax >> 1);
            for (;;)
            {
                if (x < xmin || x > xmax || y < ymin || y > ymax || z < zmin || z > zmax)
                    return;

                vec->push_back(MyIntPoint(x, y, z));
                if (x == x2)
                {
                    return;
                }

                if (yd >= 0)
                {
                    y += sy;
                    yd -= ax;
                }

                if (zd >= 0)
                {
                    z += sz;
                    zd -= ax;
                }

                x += sx;
                yd += ay;
                zd += az;
            }
        }
        else if (ay >= MAX(ax, az))            /* y dominant */
        {
            xd = ax - (ay >> 1);
            zd = az - (ay >> 1);
            for (;;)
            {
                if (x < xmin || x > xmax || y < ymin || y > ymax || z < zmin || z > zmax)
                    return;

                vec->push_back(MyIntPoint(x, y, z));
                if (y == y2)
                {
                    return;
                }

                if (xd >= 0)
                {
                    x += sx;
                    xd -= ay;
                }

                if (zd >= 0)
                {
                    z += sz;
                    zd -= ay;
                }

                y += sy;
                xd += ax;
                zd += az;
            }
        }
        else if (az >= MAX(ax, ay))            /* z dominant */
        {
            xd = ax - (az >> 1);
            yd = ay - (az >> 1);
            for (;;)
            {
                if (x < xmin || x > xmax || y < ymin || y > ymax || z < zmin || z > zmax)
                    return;

                vec->push_back(MyIntPoint(x, y, z));
                if (z == z2)
                {
                    return;
                }

                if (xd >= 0)
                {
                    x += sx;
                    xd -= az;
                }

                if (yd >= 0)
                {
                    y += sy;
                    yd -= az;
                }

                z += sz;
                xd += ax;
                yd += ay;
            }
        }
    }

public:
    MapComputeCell * ComputingCells;
    int NLayers;

    Tile ** LightedTiles;
    int LightedTilesNext;

    int * LightedTilesSumPerLayer;
    int LayersProcessed;

    int x_size;
    int y_size;
    int z_size;

    ////
    MyIntPoint * TracePointsA;
    MyIntPoint * TracePointsB;

    double TimeTaken;
    int NTimeTaken;

    //
    MapComputeCells(int nLayers)
    {
        NLayers = nLayers;

        x_size = (2 * NLayers + 1);
        y_size = (2 * NLayers + 1);
        z_size = NLayers + 1;

        int amount = z_size * x_size * y_size;

        ////
        ComputingCells = new MapComputeCell[amount];

        for (int i = 0; i < amount; i++)
            ComputingCells[i].Clear();

        ////
        LightedTiles = new Tile *[amount];

        for (int i = 0; i < amount; i++)
            LightedTiles[i] = nullptr;

        ////
        LightedTilesSumPerLayer = new int[z_size];
        for (int i = 0; i < z_size; i++)
            LightedTilesSumPerLayer[i] = 0;

        LayersProcessed = 0;

        ////
        //TracePointsNextA = 0;
        //TracePointsNextB = 0;

        TracePointsA = new MyIntPoint [amount];
        TracePointsB = new MyIntPoint [amount];

        ////
        TimeTaken = 0;
        NTimeTaken = 0;

    }

    //
    int GetIndex(int x, int y, int z) const
    {
        return x + x_size * (y + y_size * z);
    }

    //
    MapComputeCell & operator() (int x, int y, int z)
    {
        return ComputingCells[GetIndex(x + NLayers, y + NLayers, z)];
    }

    //
    ~MapComputeCells()
    {
        if (ComputingCells == nullptr)
        {
            delete [] ComputingCells;
            ComputingCells = nullptr;
        }

        ////
        if (LightedTiles != nullptr)
        {
            delete [] LightedTiles;
            LightedTiles = nullptr;
        }

        ////
        if (LightedTilesSumPerLayer != nullptr)
        {
            delete [] LightedTilesSumPerLayer;
            LightedTilesSumPerLayer = nullptr;
        }

        ////
        if (TracePointsA != nullptr)
        {
            delete [] TracePointsA;
            TracePointsA = nullptr;
        }

        if (TracePointsB != nullptr)
        {
            delete [] TracePointsB;
            TracePointsB = nullptr;
        }
    }

    //
    void ClearComputingCells()
    {
        //// number of points on the sphere
        for (int i = -NLayers; i <= NLayers; i++)
        {
            for (int j = -NLayers; j <= NLayers; j++)
            {
                for (int k = 0; k <= NLayers; k++)
                {
                    MapComputeCell * cell = &(this->operator ()(i, j, k));
                    cell->Clear();
                }
            }
        }
    }

    //
    void GenerateLinesCombined(int nGenPointsUF)
    {
        ClearComputingCells();

        double sqrt33 = 1.0 / sqrt(3);

        ////
        dflighting::Random rng(111);
        for (int i = 0; i < nGenPointsUF; i++)
        {
            //double z = myrandom(&initialRandom);
            double z = rng.nextDouble() * (1.0 - sqrt33) + sqrt33;

            double t = 2 * 3.14159265358 * rng.nextDouble();
            double r = sqrt(1 - z * z);
            double x = r * cos(t);
            double y = r * sin(t);

            ////
            if (!(z >=0 &&
                  -z <= x && x <= z &&
                  -z <= y && y <= z))
                continue;

            ////
            double dx = 200 * NLayers * x;
            double dy = 200 * NLayers * y;
            double dz = 200 * NLayers * z;

            ////
            std::vector<MyIntPoint> vec;
            line3d(0, 0, 0, (int)dx, (int)dy, (int)dz, -NLayers, NLayers, -NLayers, NLayers, 0, NLayers, &vec);

            for (int k = 1; k < vec.size(); k++)
            {
                int ax = vec[k].X;
                int ay = vec[k].Y;
                int az = vec[k].Z;

                int px = vec[k - 1].X;
                int py = vec[k - 1].Y;
                int pz = vec[k - 1].Z;

                this->operator ()(px, py, pz).Directions[ax - px + 1][ay - py + 1] += 1;
                //ComputingCells[pz][px + NLayers][py + NLayers].Directions[ax - px + 1][ay - py + 1] += 1;
            }
        }

        ////
        //double minLayerValue = 1;
        //double maxLayerValue = 1;

        //std::vector<double> minValues;
        //std::vector<double> maxValues;

        //// update amountsDir
        for (int k = 0; k <= NLayers; k++)
        {
            //minValues.push_back(minLayerValue);
            //maxValues.push_back(maxLayerValue);

            for (int i = -k; i <= k; i++)
            {
                for (int j = -k; j <= k; j++)
                {
                    MapComputeCell * cell = &(this->operator ()(i, j, k)); // &(ComputingCells[k][i + NLayers][j + NLayers]);

                    int sum = 0;
                    for (int ax = 0; ax < 3; ax++)
                        for (int ay = 0; ay < 3; ay++)
                            sum += cell->Directions[ax][ay];

                    if (sum == 0)
                        continue;

                    double inv = 1.0 / (double)sum;
                    for (int ax = 0; ax < 3; ax++)
                        for (int ay = 0; ay < 3; ay++)
                            cell->Directions[ax][ay] *= inv;
                }
            }

            //// least possible value corresponds to i = k && j = k
            //MapComputeCell & cellMin = this->operator ()(k, k, k);
            //double minTransferValue = cellMin.Directions[2][2];

            //minLayerValue *= minTransferValue;

            ////
            //MapComputeCell & cellMax = this->operator ()(0, 0, k);
            //double maxTransferValue = cellMax.Directions[1][1];

            //maxLayerValue *= maxTransferValue;

            ////
            //cout << "1";
        }
    }



    //// 0 = left, 1 = right, 2 = top, 3 = bottom, 4 = up, 5 = down
    template<int dir>
    void TraceVisibility3(int stx, int sty, int stz, const MyIntensity & intensity, int maxLayers)
    {
        ////
        struct timeval start, end;
        gettimeofday(&start, NULL);

        //// set up lighted tiles buffer




        //// clean existing visibility
        //std::vector<MyIntPoint> nextPoints;
        MyIntPoint * pointsA = &TracePointsA[0];
        int TracePointsNextA = 0;

        int nextPointsA = TracePointsNextA;

        //std::vector<MyIntPoint> currentPoints;
        MyIntPoint * pointsB = &TracePointsB[0];
        int TracePointsNextB = 0;

        //currentPoints.push_back(MyIntPoint(0, 0, 0));
        TracePointsB[0] = MyIntPoint(0, 0, 0);
        TracePointsNextB += 1;

        int nextPointsB = TracePointsNextB;


        //// A = future buffer
        //// B = current buffer

        ////
        MapComputeCell * initialCell = &(this->operator ()(0, 0, 0));

        initialCell->Intensity = intensity;
        initialCell->IsProcessMarked = false;
        initialCell->IsWithinLightedCellsArray = false;

        ////
        LightedTilesNext = 0;

        Tile * initialWorldTile = (*worldMap)(stx, sty, stz);
        if (initialWorldTile != nullptr)
        {
            if (intensity.HighEnough())
            {
                initialWorldTile->LightValueITraced = intensity;

                if (!initialWorldTile->IsWithinLightedTilesTraced)
                {
                    initialWorldTile->IsWithinLightedTilesTraced = true;

                    LightedTiles[LightedTilesNext] = initialWorldTile;
                    LightedTilesNext += 1;
                }
            }
        }

        ////
        //ComputingCellsLighted[ComputingCellsLightedNextIndex++] = initialCell;
        //allCellsProcessed.push_back(initialCell);

        ////
        LayersProcessed = 0;

        ////
        int layersToProcess = min(maxLayers, NLayers);

        ////
        int layer = 0;
        for (layer = 0; layer < layersToProcess; layer++)
        //for (int layer = 0; layer < layersToProcess; layer++)
        {
            LayersProcessed += 1;

            ////
            bool shouldBreak = false;
            switch(dir)
            {
                case 0:
                    shouldBreak = (stx - layer - 1 < 0);
                    break;
                case 1:
                    shouldBreak = (stx + layer + 1 >= worldMap->x_size);
                    break;
                case 2:
                    shouldBreak = (sty - layer - 1 < 0);
                    break;
                case 3:
                    shouldBreak = (sty + layer + 1 >= worldMap->y_size);
                    break;
                case 4:
                    shouldBreak = (stz + layer + 1 >= worldMap->z_size);
                    break;
                case 5:
                    shouldBreak = (stz - layer - 1 < 0);
                    break;
                default:
                    shouldBreak = false;
                    break;
            }

            /*
            if (shouldBreak)
            {
                //// clear computing cells
                for (int i = 0; i < nextPointsB; i++)
                {
                    int cpx = pointsB[i].X;
                    int cpy = pointsB[i].Y;
                    int cpz = pointsB[i].Z;

                    int x = cpx;
                    int y = cpy;
                    int z = cpz;

                    MapComputeCell * cell = &(this->operator ()(x, y, z));
                    cell->IsProcessMarked = false;
                    cell->IsWithinLightedCellsArray = false;
                    cell->Intensity = 0;
                }

                //// save total number of points
                LightedTilesSumPerLayer[layer] = LightedTilesNext;

                ////
                break;
            }
            */
            if (shouldBreak)
                break;

            ////
            for (int i = 0; i < nextPointsB; i++)
            {
                int cpx = pointsB[i].X;
                int cpy = pointsB[i].Y;
                int cpz = pointsB[i].Z;

                int x = cpx;
                int y = cpy;
                int z = cpz;

                ////
                MapComputeCell * cell = &(this->operator ()(x, y, z));
                cell->IsProcessMarked = false;
                cell->IsWithinLightedCellsArray = false;
                MyIntensity cellAmountsI = cell->Intensity;
                cell->Intensity = 0;

                int minax = (cpx <= 0) ? 0 : 1;
                int maxax = (cpx >= 0) ? 2 : 1;
                int minay = (cpy <= 0) ? 0 : 1;
                int maxay = (cpy >= 0) ? 2 : 1;

                for (int ax = minax; ax <= maxax; ax++)
                {
                    int targetx = x + ax - 1;
                    //if (targetx < 0 || targetx > 2 * NLayers)
                    //    continue;

                    ////
                    for (int ay = minay; ay <= maxay; ay++)
                    {
                        int targety = y + ay - 1;
                    //    if (targety < 0 || targety > 2 * NLayers)
                    //        continue;

                        int targetz = layer + 1;

                        ////
                        int wcurrentx;
                        int wcurrenty;
                        int wcurrentz;

                        int wtargetx;
                        int wtargety;
                        int wtargetz;
                        switch(dir)
                        {
                            case 0:
                                //// left
                                wcurrentx = stx - layer;
                                wcurrenty = cpy + sty;
                                wcurrentz = stz + cpx;

                                wtargetx = wcurrentx - 1;
                                wtargety = wcurrenty + (ay - 1);
                                wtargetz = wcurrentz + (ax - 1);


                                if (wtargety < 0 || wtargety >= worldMap->y_size ||
                                    wtargetz < 0 || wtargetz >= worldMap->z_size)
                                {
                                    continue;
                                }

                                break;
                            case 1:
                                //// right
                                wcurrentx = stx + layer;
                                wcurrenty = cpy + sty;
                                wcurrentz = stz + cpx;

                                wtargetx = wcurrentx + 1;
                                wtargety = wcurrenty + (ay - 1);
                                wtargetz = wcurrentz + (ax - 1);

                                if (wtargety < 0 || wtargety >= worldMap->y_size ||
                                    wtargetz < 0 || wtargetz >= worldMap->z_size)
                                {
                                    continue;
                                }

                                break;
                            case 2:
                                //// bottom
                                wcurrentx = cpx + stx;
                                wcurrenty = sty - layer;
                                wcurrentz = cpy + stz;

                                wtargetx = wcurrentx + (ax - 1);
                                wtargety = wcurrenty - 1;
                                wtargetz = wcurrentz + (ay - 1);

                                if (wtargetx < 0 || wtargetx >= worldMap->x_size ||
                                    wtargetz < 0 || wtargetz >= worldMap->z_size)
                                {
                                    continue;
                                }

                                break;
                            case 3:
                                //// down
                                wcurrentx = cpx + stx;
                                wcurrenty = sty + layer;
                                wcurrentz = cpy + stz;

                                wtargetx = wcurrentx + (ax - 1);
                                wtargety = wcurrenty + 1;
                                wtargetz = wcurrentz + (ay - 1);

                                if (wtargetx < 0 || wtargetx >= worldMap->x_size ||
                                    wtargetz < 0 || wtargetz >= worldMap->z_size)
                                {
                                    continue;
                                }

                                break;
                            case 4:
                                //// up
                                wcurrentx = cpx + stx;
                                wcurrenty = cpy + sty;
                                wcurrentz = stz + layer;

                                wtargetx = wcurrentx + (ax - 1);
                                wtargety = wcurrenty + (ay - 1);
                                wtargetz = wcurrentz + 1;

                                if (wtargetx < 0 || wtargetx >= worldMap->x_size ||
                                    wtargety < 0 || wtargety >= worldMap->y_size)
                                {
                                    continue;
                                }

                                break;
                            case 5:
                                //// down
                                wcurrentx = cpx + stx;
                                wcurrenty = cpy + sty;
                                wcurrentz = stz - layer;

                                wtargetx = wcurrentx + (ax - 1);
                                wtargety = wcurrenty + (ay - 1);
                                wtargetz = wcurrentz - 1;

                                if (wtargetx < 0 || wtargetx >= worldMap->x_size ||
                                    wtargety < 0 || wtargety >= worldMap->y_size)
                                {
                                    continue;
                                }

                                break;
                        }

    //                    if (wtargetx < 0 || wtargetx >= worldMap->x_size ||
      //                      wtargety < 0 || wtargety >= worldMap->y_size ||
        //                    wtargetz < 0 || wtargetz >= worldMap->z_size)
          //              {
            //                continue;
              //          }

                        ////
                        Tile * worldTile = (*worldMap)(wtargetx, wtargety, wtargetz);
                        if (worldTile == nullptr)
                            continue;

                        MyIntensity amountToAddI = (cellAmountsI * cell->Directions[ax][ay]);

    //                    if (wtargetx == 70)
      //                      if (wtargety < 65)
        //                    amountToAddI *= 1.0 / (65 - wtargety);
          //              else
            //                    amountToAddI *= 1.0 / sqrt(abs(75-wtargety) + 1);

                        if (!amountToAddI.HighEnough())
                            continue;

                        if (wtargetz > wcurrentz)
                        {
                            //// go up
                            bool canGoUp = false;

                            //// can one go up?
                            Tile * currentwTile = (*worldMap)(wcurrentx, wcurrenty, wcurrentz);
                            if (currentwTile != nullptr)
                                //canGoUp = (currentwTile->IsPassable_HoleDown && worldTile->IsPassable_HoleUp);
                                canGoUp = (currentwTile->IsPassableByLight_HoleUp && worldTile->IsPassableByLight_HoleDown);

                                //canGoUp = (currentwTile->IsPassable_HoleUp &&
                                //           ((worldTile->IsPassable && worldTile->IsPassable_HoleDown) ||
                                //            (!worldTile->IsPassable)));

                            ////
                            if (!canGoUp)
                                continue;
                        }
                        else if (wtargetz < wcurrentz)
                        {
                            //// go down
                            bool canGoDown = false;

                            //// can one go down
                            Tile * currentwTile = (*worldMap)(wcurrentx, wcurrenty, wcurrentz);
                            if (currentwTile != nullptr)
                                //canGoDown = (currentwTile->IsPassable_HoleUp && worldTile->IsPassable_HoleDown);
                                canGoDown = (currentwTile->IsPassableByLight_HoleDown && worldTile->IsPassableByLight_HoleUp);

                                //canGoDown = (currentwTile->IsPassable_HoleDown &&
                                //             ((worldTile->IsPassable && worldTile->IsPassable_HoleUp) ||
                                //              (!worldTile->IsPassable)));

                            if (!canGoDown)
                                continue;
                        }
                        else
                        {
                            //// same level
                        }

                        ////
                        worldTile->LightValueITraced += amountToAddI;

                        if (!worldTile->IsWithinLightedTilesTraced)
                        {
                            worldTile->IsWithinLightedTilesTraced = true;

                            LightedTiles[LightedTilesNext] = worldTile;
                            LightedTilesNext += 1;
                        }

                        ////
                        if (!worldTile->IsPassableByLight)
                            continue;




                        ////
                        MapComputeCell * targetCell = &(this->operator ()(targetx, targety, targetz));
                        //targetCell->AmountsV += amountToAdd;
                        targetCell->Intensity += amountToAddI;

                        if (targetCell->IsProcessMarked)
                            continue;

                        targetCell->IsProcessMarked = true;

                        ////
                        pointsA[nextPointsA] = MyIntPoint(targetx, targety, targetz);
                        nextPointsA += 1;
                        //nextPoints.push_back(MyIntPoint(targetx - NLayers, targety - NLayers, targetz));

                    }
                }
            }

            //// save total number of points
            LightedTilesSumPerLayer[layer] = LightedTilesNext;

            //// no points?
            bool shouldBreak2 = false;
            if (nextPointsA == 0)
                shouldBreak2 = true;
            ////
            //currentPoints.clear();
            //currentPoints.assign(nextPoints.begin(), nextPoints.end());
            //nextPoints.clear();

            //// A = future buffer
            //// B = current buffer

            //// Swap A & B
            nextPointsB = nextPointsA;
            nextPointsA = 0;

            //// swap
            MyIntPoint * pswap = pointsA;
            pointsA = pointsB;
            pointsB = pswap;

            if (shouldBreak2)
                break;
        }


        //// clear computing cells
        for (int i = 0; i < nextPointsB; i++)
        {
            int cpx = pointsB[i].X;
            int cpy = pointsB[i].Y;
            int cpz = pointsB[i].Z;

            int x = cpx;
            int y = cpy;
            int z = cpz;

            MapComputeCell * cell = &(this->operator ()(x, y, z));
            cell->IsProcessMarked = false;
            cell->IsWithinLightedCellsArray = false;
            cell->Intensity = 0;
        }

        //// save total number of points
        LightedTilesSumPerLayer[layer] = LightedTilesNext;



        ////
        gettimeofday(&end, NULL);
        long seconds, useconds;

        seconds  = end.tv_sec  - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;

        double mtime = ((seconds) * 1000 + useconds/1000.0);

        TimeTaken += mtime;
        NTimeTaken += 1;
    }



    //// 0 = left, 1 = right, 2 = top, 3 = bottom, 4 = up, 5 = down
    template<int dir>
    void TraceVisibility2(int stx, int sty, int stz, const MyIntensity & intensity)
    {
        ////
        struct timeval start, end;
        gettimeofday(&start, NULL);

        //// set up lighted tiles buffer




        //// clean existing visibility
        //std::vector<MyIntPoint> nextPoints;
        MyIntPoint * pointsA = &TracePointsA[0];
        int TracePointsNextA = 0;

        int nextPointsA = TracePointsNextA;

        //std::vector<MyIntPoint> currentPoints;
        MyIntPoint * pointsB = &TracePointsB[0];
        int TracePointsNextB = 0;

        //currentPoints.push_back(MyIntPoint(0, 0, 0));
        TracePointsB[0] = MyIntPoint(0, 0, 0);
        TracePointsNextB += 1;

        int nextPointsB = TracePointsNextB;


        //// A = future buffer
        //// B = current buffer

        ////
        MapComputeCell * initialCell = &(this->operator ()(0, 0, 0));

        initialCell->Intensity = intensity;
        initialCell->IsProcessMarked = false;
        initialCell->IsWithinLightedCellsArray = false;

        ////
        LightedTilesNext = 0;

        Tile * initialWorldTile = (*worldMap)(stx, sty, stz);
        if (initialWorldTile != nullptr)
        {
            if (intensity.HighEnough())
            {
                initialWorldTile->LightValueITraced = intensity;

                if (!initialWorldTile->IsWithinLightedTilesTraced)
                {
                    initialWorldTile->IsWithinLightedTilesTraced = true;

                    LightedTiles[LightedTilesNext] = initialWorldTile;
                    LightedTilesNext += 1;
                }
            }
        }

        ////
        //ComputingCellsLighted[ComputingCellsLightedNextIndex++] = initialCell;
        //allCellsProcessed.push_back(initialCell);

        ////
        LayersProcessed = 0;

        ////
        for (int layer = 0; layer < NLayers; layer++)
        {
            LayersProcessed += 1;

            ////
            bool shouldBreak = false;
            switch(dir)
            {
                case 0:
                    shouldBreak = (stx - layer - 1 < 0);
                    break;
                case 1:
                    shouldBreak = (stx + layer + 1 >= worldMap->x_size);
                    break;
                case 2:
                    shouldBreak = (sty - layer - 1 < 0);
                    break;
                case 3:
                    shouldBreak = (sty + layer + 1 >= worldMap->y_size);
                    break;
                case 4:
                    shouldBreak = (stz + layer + 1 >= worldMap->z_size);
                    break;
                case 5:
                    shouldBreak = (stz - layer - 1 < 0);
                    break;
                default:
                    shouldBreak = false;
                    break;
            }

            if (shouldBreak)
            {
                //// clear computing cells
                for (int i = 0; i < nextPointsB; i++)
                {
                    int cpx = pointsB[i].X;
                    int cpy = pointsB[i].Y;
                    int cpz = pointsB[i].Z;

                    int x = cpx;
                    int y = cpy;
                    int z = cpz;

                    MapComputeCell * cell = &(this->operator ()(x, y, z));
                    cell->IsProcessMarked = false;
                    cell->IsWithinLightedCellsArray = false;
                    cell->Intensity = 0;
                }

                //// save total number of points
                LightedTilesSumPerLayer[layer] = LightedTilesNext;

                ////
                break;
            }

            ////
            for (int i = 0; i < nextPointsB; i++)
            {
                int cpx = pointsB[i].X;
                int cpy = pointsB[i].Y;
                int cpz = pointsB[i].Z;

                int x = cpx;
                int y = cpy;
                int z = cpz;

                ////
                MapComputeCell * cell = &(this->operator ()(x, y, z));
                cell->IsProcessMarked = false;
                cell->IsWithinLightedCellsArray = false;
                MyIntensity cellAmountsI = cell->Intensity;
                cell->Intensity = 0;

                int minax = (cpx <= 0) ? 0 : 1;
                int maxax = (cpx >= 0) ? 2 : 1;
                int minay = (cpy <= 0) ? 0 : 1;
                int maxay = (cpy >= 0) ? 2 : 1;

                for (int ax = minax; ax <= maxax; ax++)
                {
                    int targetx = x + ax - 1;
                    //if (targetx < 0 || targetx > 2 * NLayers)
                    //    continue;

                    ////
                    for (int ay = minay; ay <= maxay; ay++)
                    {
                        int targety = y + ay - 1;
                    //    if (targety < 0 || targety > 2 * NLayers)
                    //        continue;

                        int targetz = layer + 1;

                        ////
                        int wcurrentx;
                        int wcurrenty;
                        int wcurrentz;

                        int wtargetx;
                        int wtargety;
                        int wtargetz;
                        switch(dir)
                        {
                            case 0:
                                //// left
                                wcurrentx = stx - layer;
                                wcurrenty = cpy + sty;
                                wcurrentz = stz + cpx;

                                wtargetx = wcurrentx - 1;
                                wtargety = wcurrenty + (ay - 1);
                                wtargetz = wcurrentz + (ax - 1);


                                if (wtargety < 0 || wtargety >= worldMap->y_size ||
                                    wtargetz < 0 || wtargetz >= worldMap->z_size)
                                {
                                    continue;
                                }

                                break;
                            case 1:
                                //// right
                                wcurrentx = stx + layer;
                                wcurrenty = cpy + sty;
                                wcurrentz = stz + cpx;

                                wtargetx = wcurrentx + 1;
                                wtargety = wcurrenty + (ay - 1);
                                wtargetz = wcurrentz + (ax - 1);

                                if (wtargety < 0 || wtargety >= worldMap->y_size ||
                                    wtargetz < 0 || wtargetz >= worldMap->z_size)
                                {
                                    continue;
                                }

                                break;
                            case 2:
                                //// bottom
                                wcurrentx = cpx + stx;
                                wcurrenty = sty - layer;
                                wcurrentz = cpy + stz;

                                wtargetx = wcurrentx + (ax - 1);
                                wtargety = wcurrenty - 1;
                                wtargetz = wcurrentz + (ay - 1);

                                if (wtargetx < 0 || wtargetx >= worldMap->x_size ||
                                    wtargetz < 0 || wtargetz >= worldMap->z_size)
                                {
                                    continue;
                                }

                                break;
                            case 3:
                                //// down
                                wcurrentx = cpx + stx;
                                wcurrenty = sty + layer;
                                wcurrentz = cpy + stz;

                                wtargetx = wcurrentx + (ax - 1);
                                wtargety = wcurrenty + 1;
                                wtargetz = wcurrentz + (ay - 1);

                                if (wtargetx < 0 || wtargetx >= worldMap->x_size ||
                                    wtargetz < 0 || wtargetz >= worldMap->z_size)
                                {
                                    continue;
                                }

                                break;
                            case 4:
                                //// up
                                wcurrentx = cpx + stx;
                                wcurrenty = cpy + sty;
                                wcurrentz = stz + layer;

                                wtargetx = wcurrentx + (ax - 1);
                                wtargety = wcurrenty + (ay - 1);
                                wtargetz = wcurrentz + 1;

                                if (wtargetx < 0 || wtargetx >= worldMap->x_size ||
                                    wtargety < 0 || wtargety >= worldMap->y_size)
                                {
                                    continue;
                                }

                                break;
                            case 5:
                                //// down
                                wcurrentx = cpx + stx;
                                wcurrenty = cpy + sty;
                                wcurrentz = stz - layer;

                                wtargetx = wcurrentx + (ax - 1);
                                wtargety = wcurrenty + (ay - 1);
                                wtargetz = wcurrentz - 1;

                                if (wtargetx < 0 || wtargetx >= worldMap->x_size ||
                                    wtargety < 0 || wtargety >= worldMap->y_size)
                                {
                                    continue;
                                }

                                break;
                        }

    //                    if (wtargetx < 0 || wtargetx >= worldMap->x_size ||
      //                      wtargety < 0 || wtargety >= worldMap->y_size ||
        //                    wtargetz < 0 || wtargetz >= worldMap->z_size)
          //              {
            //                continue;
              //          }

                        ////
                        Tile * worldTile = (*worldMap)(wtargetx, wtargety, wtargetz);
                        if (worldTile == nullptr)
                            continue;

                        MyIntensity amountToAddI = (cellAmountsI * cell->Directions[ax][ay]);

    //                    if (wtargetx == 70)
      //                      if (wtargety < 65)
        //                    amountToAddI *= 1.0 / (65 - wtargety);
          //              else
            //                    amountToAddI *= 1.0 / sqrt(abs(75-wtargety) + 1);

                        if (!amountToAddI.HighEnough())
                            continue;

                        if (wtargetz > wcurrentz)
                        {
                            //// go up
                            bool canGoUp = false;

                            //// can one go up?
                            Tile * currentwTile = (*worldMap)(wcurrentx, wcurrenty, wcurrentz);
                            if (currentwTile != nullptr)
                                //canGoUp = (currentwTile->IsPassable_HoleDown && worldTile->IsPassable_HoleUp);
                                canGoUp = (currentwTile->IsPassableByLight_HoleUp && worldTile->IsPassableByLight_HoleDown);

                                //canGoUp = (currentwTile->IsPassable_HoleUp &&
                                //           ((worldTile->IsPassable && worldTile->IsPassable_HoleDown) ||
                                //            (!worldTile->IsPassable)));

                            ////
                            if (!canGoUp)
                                continue;
                        }
                        else if (wtargetz < wcurrentz)
                        {
                            //// go down
                            bool canGoDown = false;

                            //// can one go down
                            Tile * currentwTile = (*worldMap)(wcurrentx, wcurrenty, wcurrentz);
                            if (currentwTile != nullptr)
                                //canGoDown = (currentwTile->IsPassable_HoleUp && worldTile->IsPassable_HoleDown);
                                canGoDown = (currentwTile->IsPassableByLight_HoleDown && worldTile->IsPassableByLight_HoleUp);

                                //canGoDown = (currentwTile->IsPassable_HoleDown &&
                                //             ((worldTile->IsPassable && worldTile->IsPassable_HoleUp) ||
                                //              (!worldTile->IsPassable)));

                            if (!canGoDown)
                                continue;
                        }
                        else
                        {
                            //// same level
                        }

                        ////
                        worldTile->LightValueITraced += amountToAddI;

                        if (!worldTile->IsWithinLightedTilesTraced)
                        {
                            worldTile->IsWithinLightedTilesTraced = true;

                            LightedTiles[LightedTilesNext] = worldTile;
                            LightedTilesNext += 1;
                        }

                        ////
                        if (!worldTile->IsPassableByLight)
                            continue;




                        ////
                        MapComputeCell * targetCell = &(this->operator ()(targetx, targety, targetz));
                        //targetCell->AmountsV += amountToAdd;
                        targetCell->Intensity += amountToAddI;

                        if (targetCell->IsProcessMarked)
                            continue;

                        targetCell->IsProcessMarked = true;

                        ////
                        pointsA[nextPointsA] = MyIntPoint(targetx, targety, targetz);
                        nextPointsA += 1;
                        //nextPoints.push_back(MyIntPoint(targetx - NLayers, targety - NLayers, targetz));

                    }
                }
            }

            //// save total number of points
            LightedTilesSumPerLayer[layer] = LightedTilesNext;

            //// no points?
            if (nextPointsA == 0)
                break;
            ////
            //currentPoints.clear();
            //currentPoints.assign(nextPoints.begin(), nextPoints.end());
            //nextPoints.clear();

            //// A = future buffer
            //// B = current buffer

            //// Swap A & B
            nextPointsB = nextPointsA;
            nextPointsA = 0;

            //// swap
            MyIntPoint * pswap = pointsA;
            pointsA = pointsB;
            pointsB = pswap;
        }


        ////
        gettimeofday(&end, NULL);
        long seconds, useconds;

        seconds  = end.tv_sec  - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;

        double mtime = ((seconds) * 1000 + useconds/1000.0);

        TimeTaken += mtime;
        NTimeTaken += 1;
    }


    //
    void ClearITraced()
    {
        for (int iCell = 0; iCell < LightedTilesNext; iCell++ )
        {
            LightedTiles[iCell]->LightValueITraced = 0;
            LightedTiles[iCell]->IsWithinLightedTilesTraced = false;
        }
    }

    //
    void SaveToStream(std::ostream * stream)
    {
        for (int k = 0; k <= NLayers; k++)
        {
            for (int i = -k; i <= k; i++)
            {
                for (int j = -k; j <= k; j++)
                {
                    (this->operator ()(i, j, k)).SaveToStream(stream);
                }
            }
        }
    }

    //
    void LoadFromStream(std::istream * stream)
    {
        for (int k = 0; k <= NLayers; k++)
        {
            for (int i = -k; i <= k; i++)
            {
                for (int j = -k; j <= k; j++)
                {
                    (this->operator ()(i, j, k)).LoadFromStream(stream);
                }
            }
        }
    }
};


MapComputeCells ComputingCells(80);







////


int NTraceVisibilities;









double TraceVisibilityCachedTime;
int NTraceVisibilityCachedTime;
double TraceVisibilityCachedAsMemoryTime;
int NTraceVisibilityCachedAsMemoryTime;
















LightWorldMap * lightWorldMap;

//
void CreateVisibilityCache(int i, int j, int k, int dir, int maxLayers)
{
    MyIntensity color(1);

    ////
    switch(dir)
    {
        case 0:
            ComputingCells.TraceVisibility3<0>(i, j, k, color, maxLayers);
            break;
        case 1:
            ComputingCells.TraceVisibility3<1>(i, j, k, color, maxLayers);
            break;
        case 2:
            ComputingCells.TraceVisibility3<2>(i, j, k, color, maxLayers);
            break;
        case 3:
            ComputingCells.TraceVisibility3<3>(i, j, k, color, maxLayers);
            break;
        case 4:
            ComputingCells.TraceVisibility3<4>(i, j, k, color, maxLayers);
            break;
        case 5:
            ComputingCells.TraceVisibility3<5>(i, j, k, color, maxLayers);
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
LightVisibilityCacheStruct * GetOrCreateVisibilityCachedLight(int stx, int sty, int stz, int dir, int maxLayersToCache)
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
std::unordered_set<LightVisibilityCacheStruct *> LightCachesToRemove;


//
void TraceVisibilityCached(LightVisibilityCache1 * light1)
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

void TraceVisibilityCached6(LightVisibilityCache6 * light6)
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
void PlaceLight1ToMap(LightVisibilityCache1 * light1)
{
    int stx = light1->WorldX;
    int sty = light1->WorldY;
    int stz = light1->WorldZ;
    int dir = light1->Dir;

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
void PlaceLight6ToMap(LightVisibilityCache6 * light6)
{
    int stx = light6->WorldX;
    int sty = light6->WorldY;
    int stz = light6->WorldZ;

    //// emit or create caches
    LightVisibilityCacheStruct * leftLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, 0, light6->LeftLight->MaxLayers);
    LightVisibilityCacheStruct * rightLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, 1, light6->RightLight->MaxLayers);
    LightVisibilityCacheStruct * topLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, 2, light6->TopLight->MaxLayers);
    LightVisibilityCacheStruct * bottomLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, 3, light6->BottomLight->MaxLayers);
    LightVisibilityCacheStruct * upLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, 4, light6->UpLight->MaxLayers);
    LightVisibilityCacheStruct * downLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, 5, light6->DownLight->MaxLayers);

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
void RemoveLight1FromMap(LightVisibilityCache1 * light1)
{
    int & x = light1->WorldX;
    int & y = light1->WorldY;
    int & z = light1->WorldZ;
    int & dir = light1->Dir;
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
void RemoveLight6FromMap(LightVisibilityCache6 * light6)
{
    RemoveLight1FromMap(light6->LeftLight);
    RemoveLight1FromMap(light6->RightLight);
    RemoveLight1FromMap(light6->TopLight);
    RemoveLight1FromMap(light6->BottomLight);
    RemoveLight1FromMap(light6->UpLight);
    RemoveLight1FromMap(light6->DownLight);
}


//
void ChangeLight1Color(LightVisibilityCache1 * light1, MyColor & newColor)
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
void ChangeLight6Color(LightVisibilityCache6 * light6, MyColor & newColor)
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
void TraceVisibilityCachedAsMemory(LightVisibilityCache1 * light1, bool addToLightedByMemoryLights)
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

void TraceVisibilityCached6AsMemory(LightVisibilityCache6 * light6, bool addToLightedByMemoryLights)
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
void PlaceMemoryLight1ToMap(LightVisibilityCache1 * light1)
{
    int stx = light1->WorldX;
    int sty = light1->WorldY;
    int stz = light1->WorldZ;
    int dir = light1->Dir;

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
void PlaceMemoryLight6ToMap(LightVisibilityCache6 * light6)
{
    int stx = light6->WorldX;
    int sty = light6->WorldY;
    int stz = light6->WorldZ;

    //// emit or create caches
    LightVisibilityCacheStruct * leftLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, 0, light6->LeftLight->MaxLayers);
    LightVisibilityCacheStruct * rightLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, 1, light6->RightLight->MaxLayers);
    LightVisibilityCacheStruct * topLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, 2, light6->TopLight->MaxLayers);
    LightVisibilityCacheStruct * bottomLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, 3, light6->BottomLight->MaxLayers);
    LightVisibilityCacheStruct * upLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, 4, light6->UpLight->MaxLayers);
    LightVisibilityCacheStruct * downLightCache = GetOrCreateVisibilityCachedLight(stx, sty, stz, 5, light6->DownLight->MaxLayers);

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
void RemoveMemoryLight1FromMap(LightVisibilityCache1 * light1)
{
    int & x = light1->WorldX;
    int & y = light1->WorldY;
    int & z = light1->WorldZ;
    int & dir = light1->Dir;
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
void RemoveMemoryLight6FromMap(LightVisibilityCache6 * light6)
{
    RemoveMemoryLight1FromMap(light6->LeftLight);
    RemoveMemoryLight1FromMap(light6->RightLight);
    RemoveMemoryLight1FromMap(light6->TopLight);
    RemoveMemoryLight1FromMap(light6->BottomLight);
    RemoveMemoryLight1FromMap(light6->UpLight);
    RemoveMemoryLight1FromMap(light6->DownLight);
}






/*
//
void ClearCache(int x, int y, int z, int dir)
{
    LightVisibilityCacheStruct * light = lightWorldMap->operator ()(x, y, z, dir);

    //// clear colorers
    for (int i = 0; i < light->Colorers->size(); i++)
    {
        ColorerCell * cell = &(light->Colorers->operator [](i));

        Tile * tile;
#ifdef fastColorerVersion
        tile = cell-> TargetWorldTile;
#else
        tile = (*worldMap)(cell->GlobalIndex);
#endif

        for (vector<LightVisibilityCacheStruct*>::iterator iter = tile->LightedBy.begin(); iter != tile->LightedBy.end(); iter++)
        //for (auto iter : tile->LightedBy)
        {
            if (*iter == light)
            {
                if (tile->LightedBy.size() == 1)
                {
                    tile->LightedBy.clear();
                }
                else
                {
                    vector<LightVisibilityCacheStruct*>::iterator iterLast = tile->LightedBy.end() - 1;
                    //auto iterLast = tile->LightedBy.end() - 1;
                    *iter = *iterLast;
                    tile->LightedBy.erase(iterLast);
                }
                //tile->LightedBy.erase(iter);
                break;
            }
        }

        //if (iterFind != light->Colorers->end())
        //    tile->LightedBy.erase(iterFind);
        //tile->LightedBy.erase(light);
    }

    light->Colorers->clear();
}
*/

/*

//
void ClearCache(int x, int y, int z)
{
    ////
    struct timeval start, end;

    gettimeofday(&start, NULL);



    Tile * tile = (*worldMap)(x, y, z);
    std::vector<LightVisibilityCacheStruct*> uset(tile->LightedBy);

    for (std::vector<LightVisibilityCacheStruct*>::iterator iter = uset.begin(); iter != uset.end(); iter++)
    {
        LightVisibilityCacheStruct * light = *iter;
        ClearCache(light->WorldX, light->WorldY, light->WorldZ, light->Dir);
        CreateVisibilityCache(light->WorldX, light->WorldY, light->WorldZ, light->Dir);
    }


    ////
    gettimeofday(&end, NULL);
    long seconds, useconds;

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    double mtime = ((seconds) * 1000 + useconds/1000.0);

    ////
    LastVisibilityProcessTime += mtime;
    ClearVisibilityProcessTime += mtime;

}
*/





//
std::vector<Tile*> Saplings;
std::vector<Tile*> Shrubs;


//
void TrackSaplingsNBushes()
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

/*
//
void RenderRecalcVisibilityCached(float multiplier)
{
    ////
    struct timeval start, end;

    gettimeofday(&start, NULL);


    ClearVisibility();

    RenderPointSourceCached(116, 93, 40, 100000 * multiplier);

    RenderPointSourceCached(124, 68, 40, 20000 * multiplier);
    RenderPointSourceCached(100, 96, 39, 120000 * multiplier);
    RenderPointSourceCached(117, 94, 40, 3000 * multiplier);
    RenderPointSourceCached(113, 85, 39, 120000 * multiplier);
    RenderPointSourceCached(109, 85, 42, 40000 * multiplier);
    RenderPointSourceCached(116, 88, 38, 10000 * multiplier);
    RenderPointSourceCached(126, 94, 39, 120000 * multiplier);
    RenderPointSourceCached(110, 96, 41, 10000 * multiplier);
    RenderPointSourceCached(100, 78, 42, 120000 * multiplier);
    RenderPointSourceCached(102, 106, 42, 40000 * multiplier);

    RenderPointSourceCached(108, 74, 42, 3000 * multiplier);
    RenderPointSourceCached(99, 109, 42, 3000 * multiplier);
    RenderPointSourceCached(104, 103, 42, 3000 * multiplier);
    RenderPointSourceCached(99, 89, 42, 13000 * multiplier);
    RenderPointSourceCached(106, 82, 42, 13000 * multiplier);

    RenderPointSourceCached(115, 81, 52, 140000 * multiplier);



    for (int i = 0; i < Saplings.size(); i++)
    {
#ifdef onecolor
        RenderPointSourceCached(Saplings[i]->X, Saplings[i]->Y, Saplings[i]->Z, 200 * multiplier);
#else
        MyColor color(200, 0, 0);
        RenderPointSourceCached(Saplings[i]->X, Saplings[i]->Y, Saplings[i]->Z, color);
#endif
    }


    for (int i = 0; i < Shrubs.size(); i++)
    {
#ifdef onecolor
        RenderPointSourceCached(Shrubs[i]->X, Shrubs[i]->Y, Shrubs[i]->Z, 300 * multiplier);
#else
        MyColor color(300, 0, 0);
        RenderPointSourceCached(Shrubs[i]->X, Shrubs[i]->Y, Shrubs[i]->Z, color);
#endif
    }


    ////
    gettimeofday(&end, NULL);
    long seconds, useconds;

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    double mtime = ((seconds) * 1000 + useconds/1000.0);

    ////
    LastVisibilityProcessTime += mtime;
    ClearVisibilityProcessTime += mtime;

    NVisibilityTimes += 1;
}
*/

/*
//
void ChangeTileType(int x, int y, int z, TileType newType, TileMaterialType newMaterial)
{
    NTraceVisibilities = 0;

    ////
    struct timeval start, end;

    gettimeofday(&start, NULL);



    Tile * tile = (*worldMap)(x, y, z);
    std::vector<LightVisibilityCacheStruct*> uset(tile->LightedBy);

    RenderRecalcVisibilityCached(-1.0f);


    ////
    for (std::vector<LightVisibilityCacheStruct*>::iterator iter = uset.begin(); iter != uset.end(); iter++)
    {
        LightVisibilityCacheStruct * light = *iter;
        ClearCache(light->WorldX, light->WorldY, light->WorldZ, light->Dir);
    }



    tile->Tile_Type = newType;
    tile->Tile_Material = newMaterial;
    tile->ModifyPassableFlags();

    //CreateVisibilityCache(116, 93, 40, 0);


    ////
    for (std::vector<LightVisibilityCacheStruct*>::iterator iter = uset.begin(); iter != uset.end(); iter++)
    {
        LightVisibilityCacheStruct * light = *iter;
        CreateVisibilityCache(light->WorldX, light->WorldY, light->WorldZ, light->Dir);
    }


    RenderRecalcVisibilityCached(1.0f);

    ////
    gettimeofday(&end, NULL);
    long seconds, useconds;

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    double mtime = ((seconds) * 1000 + useconds/1000.0);

    printf("Changing cell type took %f msec and %d TraceVisibility2\n", mtime, NTraceVisibilities);
    printf("Average %f msec per TraceVisibility2\n", mtime / NTraceVisibilities);

    ////
    LastVisibilityProcessTime = mtime;
    ClearVisibilityProcessTime = mtime;
    NVisibilityTimes = 1;

}
*/


//
//std::unordered_set<LightVisibilityCacheStruct*> LightCachesToRemove;


/*
//
void ChangeTileType2(int x, int y, int z, TileType newType, TileMaterialType newMaterial)
{
    NTraceVisibilities = 0;

    ////
    struct timeval start, end;

    gettimeofday(&start, NULL);



    Tile * tile = (*worldMap)(x, y, z);
    std::vector<LightVisibilityCacheStruct*> uset(tile->LightedBy);

    ////
    for (std::vector<LightVisibilityCacheStruct*>::iterator iter = uset.begin(); iter != uset.end(); iter++)
    {
        LightVisibilityCacheStruct * light = *iter;
        ClearCache(light->WorldX, light->WorldY, light->WorldZ, light->Dir);

        LightCachesToRemove.insert(light);
    }



    tile->Tile_Type = newType;
    tile->Tile_Material = newMaterial;
    tile->ModifyPassableFlags();


    ////
    gettimeofday(&end, NULL);
    long seconds, useconds;

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    double mtime = ((seconds) * 1000 + useconds/1000.0);

    printf("Changing cell type took %f msec and %d TraceVisibility2\n", mtime, NTraceVisibilities);
    printf("Average %f msec per TraceVisibility2\n", mtime / NTraceVisibilities);

    ////
    LastVisibilityProcessTime = mtime;
    ClearVisibilityProcessTime = mtime;
    NVisibilityTimes = 1;

}



//
void ChangeDoor(int x, int y, int z)
{
    NTraceVisibilities = 0;

    ////
    struct timeval start, end;

    gettimeofday(&start, NULL);



    Tile * tile = (*worldMap)(x, y, z);

    if (!tile->IsDoor)
        return;

    std::vector<LightVisibilityCacheStruct*> uset(tile->LightedBy);

    ////
    for (std::vector<LightVisibilityCacheStruct*>::iterator iter = uset.begin(); iter != uset.end(); iter++)
    {
        LightVisibilityCacheStruct * light = *iter;
        ClearCache(light->WorldX, light->WorldY, light->WorldZ, light->Dir);

        LightCachesToRemove.insert(light);
    }


    tile->IsDoorOpened = !tile->IsDoorOpened;
    tile->ModifyPassableFlags();



    ////
    gettimeofday(&end, NULL);
    long seconds, useconds;

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    double mtime = ((seconds) * 1000 + useconds/1000.0);

    printf("Changing door satet took %f msec and %d TraceVisibility2\n", mtime, NTraceVisibilities);
    printf("Average %f msec per TraceVisibility2\n", mtime / NTraceVisibilities);
}


//
void RemoveLight(int x, int y, int z)
{
    NTraceVisibilities = 0;

    ////
    struct timeval start, end;

    gettimeofday(&start, NULL);



    Tile * tile = (*worldMap)(x, y, z);

    std::vector<LightVisibilityCacheStruct*> uset(tile->LightedBy);

    ////
    for (std::vector<LightVisibilityCacheStruct*>::iterator iter = uset.begin(); iter != uset.end(); iter++)
    {
        LightVisibilityCacheStruct * light = *iter;
        ClearCache(light->WorldX, light->WorldY, light->WorldZ, light->Dir);

        LightCachesToRemove.insert(light);
    }


    ////
    gettimeofday(&end, NULL);
    long seconds, useconds;

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    double mtime = ((seconds) * 1000 + useconds/1000.0);

    printf("Changing door satet took %f msec and %d TraceVisibility2\n", mtime, NTraceVisibilities);
    printf("Average %f msec per TraceVisibility2\n", mtime / NTraceVisibilities);
}

*/




//
void ChangeDoor(int x, int y, int z)
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









/////////////////
/////// A* from Eric Lippert






//
std::vector<Tile *> PathfindableCells;

void TracePathfindableCells()
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


////
SDL_Surface * DFDwarvesPics;
void LoadDFDwarvesPng()
{
    //std::string filename("/home/nop/projects/lighting/dfimages/dwarves.png");
    std::string filename("dwarves.png");
    DFDwarvesPics = IMG_Load(filename.c_str());
}


//
std::vector<std::pair<int, int> > DFDwarvesAllowedCells;
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

            DFDwarvesAllowedCells.push_back(std::pair<int, int>(j, i));
        }
    }
}




//
std::vector<WalkingAgent *> WalkingAgents;

//int WalkingRandseed;
dflighting::Random walkingRng;

////
void CreateWalkingAgents()
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


void IterateAgent(WalkingAgent * wa)
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












LightVisibilityCache6 * MyLight;
LightVisibilityCache6 * MyLightV;




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
        case SDLK_q:
            //// change to floor. banned it somewhy? maybe was debugging smth
//            LightCachesToRemove.clear();
  //          ChangeTileType2(WorldOffsetX, WorldOffsetY, WorldOffsetZ, TileType::FLOOR, TileMaterialType::SOIL);
    //        for(auto light : LightCachesToRemove)
      //          lightWorldMap->DeleteLight(light);
            break;
        case SDLK_w:
        //// change to wall. banned it somewhy? maybe was debugging smth
//            LightCachesToRemove.clear();
//
  //          ChangeTileType2(WorldOffsetX, WorldOffsetY, WorldOffsetZ, TileType::WALL, TileMaterialType::LAVA_STONE);
//
  //          for(auto light : LightCachesToRemove)
    //            lightWorldMap->DeleteLight(light);
            break;
        case SDLK_c:
//            LightCachesToRemove.clear();
//
  //          ChangeDoor(WorldOffsetX, WorldOffsetY, WorldOffsetZ);
//
  //          for(auto light : LightCachesToRemove)
    //            lightWorldMap->DeleteLight(light);
            break;

        case SDLK_i:
            ////
            for (auto wa : WalkingAgents)
                IterateAgent(wa);
            break;
        case SDLK_SPACE:
            break;

    }



    ////
    if (recalculateLighting)
    {
        ////
        LastVisibilityProcessTime = 0;
        NVisibilityTimes = 0;


        ////
        //ClearVisibility();

  //      RenderPointSourceCached(WorldOffsetX, WorldOffsetY, WorldOffsetZ, -20000);

        RemoveMemoryLight6FromMap(MyLight);
        delete MyLight;
        RemoveLight6FromMap(MyLightV);
        delete MyLightV;

        WorldOffsetX += offsetX;
        WorldOffsetY += offsetY;
        WorldOffsetZ += offsetZ;

        MyLight = new LightVisibilityCache6(WorldOffsetX, WorldOffsetY, WorldOffsetZ, MyColor(50000, 50000, 50000));
        PlaceMemoryLight6ToMap(MyLight);

        MyLightV = new LightVisibilityCache6(WorldOffsetX, WorldOffsetY, WorldOffsetZ, MyColor(10000, 10000, 10000));
        PlaceLight6ToMap(MyLightV);

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

    int cpx = - centerx + WorldOffsetX + sdlEvent->motion.x / xCellSize;
    int cpy = - centery + WorldOffsetY + sdlEvent->motion.y / yCellSize;
    int cpz = WorldOffsetZ;

//    LightCachesToRemove.clear();

    ChangeDoor(cpx, cpy, cpz);

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
            int cpx = ix - centerx + WorldOffsetX;
            int cpy = iy - centery + WorldOffsetY;
            int cpz = WorldOffsetZ;

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

            if (cpx >= 0 && cpx < worldMap->x_size &&
                cpy >= 0 && cpy < worldMap->y_size &&
                cpz >= 0 && cpz < worldMap->z_size)
            {
                tile = (*worldMap)(cpx, cpy, cpz);
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
                lightCached0 = (lightWorldMap->operator ()(cpx, cpy, cpz, 0) != nullptr);
                lightCached1 = (lightWorldMap->operator ()(cpx, cpy, cpz, 1) != nullptr);
                lightCached2 = (lightWorldMap->operator ()(cpx, cpy, cpz, 2) != nullptr);
                lightCached3 = (lightWorldMap->operator ()(cpx, cpy, cpz, 3) != nullptr);
                lightCached4 = (lightWorldMap->operator ()(cpx, cpy, cpz, 4) != nullptr);
                lightCached5 = (lightWorldMap->operator ()(cpx, cpy, cpz, 5) != nullptr);
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
            if (cpx == WorldOffsetX && cpy == WorldOffsetY && cpz == WorldOffsetZ)
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
    int minx = 0 - centerx + WorldOffsetX;
    int miny = 0 - centery + WorldOffsetY;
    int maxx = nx - 1 - centerx + WorldOffsetX;
    int maxy = ny - 1 - centery + WorldOffsetY;
    int cz = WorldOffsetZ;

    for (auto wa : WalkingAgents)
    {
        ////
        if (wa->Z != cz) continue;
        if (wa->X < minx || wa->X > maxx ||
            wa->Y < miny || wa->Y > maxy)
            continue;

        MyColor lightlevelI;
        Tile * tile = (*worldMap)(wa->X, wa->Y, wa->Z);
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
    worldMap = new WorldMap(200, 200, 200);


    ///
    lightWorldMap = new LightWorldMap(200, 200, 200);

    // render light
    LightVisibilityCache1 lc(90, 90, 90, 0, MyColor(0, 0, 0));
    lc.MaxLayers = GENERATE_RENDER_DEPTH;
    PlaceLight1ToMap(&lc);

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
    LoadDFMap();
    worldMap->CreateNeighbours();

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

    WorldOffsetX = 109;
    WorldOffsetY = 79;
    WorldOffsetZ = 155;


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
    ComputingCells.LoadFromStream(&inputCCsFile);
    inputCCsFile.close();

    ///
    lightWorldMap = new LightWorldMap(worldMap->x_size, worldMap->y_size, worldMap->z_size);

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


    Tile * initialTile = worldMap->operator ()(110, 80, 155);
    Tile * finalTile = worldMap->operator ()(58, 70, 109);

    auto renderPath = AStarPathFinder::FindPath(initialTile, finalTile);

    ////
    gettimeofday(&endP, NULL);
    double secondsP, usecondsP;

    secondsP  = endP.tv_sec  - startP.tv_sec;
    usecondsP = endP.tv_usec - startP.tv_usec;

    double mtimeP = ((secondsP) * 1000 + usecondsP/1000.0);

    printf("Tracing pathfindable cells took %f msec\n", mtimeP);

    //// trace pathable cells
    TracePathfindableCells();

    if (renderPath != nullptr)
        delete renderPath;



    //WalkingRandseed = 13123121;
    walkingRng = dflighting::Random(13123121);
    CreateWalkingAgents();

    ////
    LastVisibilityProcessTime = 0;
    NVisibilityTimes = 0;

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

    TrackSaplingsNBushes();

    //RenderRecalcVisibility();
    ////
    NTraceVisibilities = 0;

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
    printf("Average %f msec per TraceVisibility2\n", mtime / NTraceVisibilities);

    LastVisibilityProcessTime = 0;
    ClearVisibilityProcessTime = 0;


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
    for (int i = 0; i < worldMap->x_size; i++)
        for (int j = 0; j < worldMap->y_size; j++)
            for (int k = 0; k < worldMap->z_size; k++)
            {
                Tile * tile = (*worldMap)(i, j, k);
                if (tile == nullptr)
                    continue;

                if (tile->IsPassableByLight)
                    nEmptyCells += 1;
            }

    cout << "Empty cells count = " << nEmptyCells << std::endl;


    //// place static lights
    LightVisibilityCache6 light6_1(110, 72, 155, MyColor(40000, 10000, 10000));
    PlaceLight6ToMap(&light6_1);

    LightVisibilityCache6 light6_2(110, 88, 155, MyColor(10000, 10000, 40000));
    PlaceLight6ToMap(&light6_2);

    LightVisibilityCache6 light6_3(90, 76, 155, MyColor(80000, 80000, 80000));
    PlaceLight6ToMap(&light6_3);

    //// more lights
    LightVisibilityCache6 light6_11(96, 58, 155, MyColor(1400000, 00000, 00000));
    PlaceLight6ToMap(&light6_11);

    LightVisibilityCache6 light6_12(96, 52, 155, MyColor(00000, 1400000, 00000));
    PlaceLight6ToMap(&light6_12);

    LightVisibilityCache6 light6_13(96, 46, 155, MyColor(00000, 00000, 1400000));
    PlaceLight6ToMap(&light6_13);

    //// more lights
    LightVisibilityCache6 light6_21(96, 42, 155, MyColor(5000, 5000, 5000));
    PlaceLight6ToMap(&light6_21);

    LightVisibilityCache6 light6_22(82, 40, 155, MyColor(15000, 15000, 15000));
    PlaceLight6ToMap(&light6_22);

    //// large one outside
    LightVisibilityCache6 light6_31(126, 82, 158, MyColor(1115000, 1115000, 1115000));
    PlaceLight6ToMap(&light6_31);

    ////
    LightVisibilityCache6 light6_d3(72, 62, 155, MyColor(150000, 150000, 150000));
    PlaceLight6ToMap(&light6_d3);


    //// my light
    MyLight = new LightVisibilityCache6(WorldOffsetX, WorldOffsetY, WorldOffsetZ, MyColor(50000, 50000, 50000));
    PlaceMemoryLight6ToMap(MyLight);
    MyLightV = new LightVisibilityCache6(WorldOffsetX, WorldOffsetY, WorldOffsetZ, MyColor(10000, 10000, 10000));
    PlaceLight6ToMap(MyLightV);

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
        Tile * currentTile = worldMap->operator ()(WorldOffsetX, WorldOffsetY, WorldOffsetZ);

        double visTimes = (NVisibilityTimes == 0) ? 0 : LastVisibilityProcessTime / NVisibilityTimes;
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
        sTitle << "DX = " << WorldOffsetX << " ";
        sTitle << "DY = " << WorldOffsetY << " ";
        sTitle << "DZ = " << WorldOffsetZ << " ";

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
        PlaceLight6ToMap(&light6_d1);

        LightVisibilityCache6 light6_d2(tx2, ty2, 155, MyColor(140000, 00000, 00000));
        PlaceLight6ToMap(&light6_d2);


        double tvf = cos(0.03f*ttime);
        tvf = tvf*tvf;
        int tvi = (int)(150000 * tvf);
        MyColor newFloatingColor(tvi, tvi, tvi);
        ChangeLight6Color(&light6_d3, newFloatingColor);

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
        LightCachesToRemove.clear();

        RemoveLight6FromMap(&light6_d1);
        RemoveLight6FromMap(&light6_d2);
        //RemoveLight6FromMap(&light6_d3);

        for(auto lightCache : LightCachesToRemove)
            lightWorldMap->DeleteLightCache(lightCache);

        ////
        struct timeval startmt, endmt;
        gettimeofday(&startmt, NULL);

        worldMap->ProcessMemoryTiles();

        ////
        gettimeofday(&endmt, NULL);
        double secondsmt, usecondsmt;

        secondsmt  = endmt.tv_sec  - startmt.tv_sec;
        usecondsmt = endmt.tv_usec - startmt.tv_usec;

        double mtimemt = ((secondsmt) * 1000 + usecondsmt/1000.0);

        //sTitle << "MT " << mtimemt << " ms. ";











        ////
        LightCachesToRemove.clear();


        //// remove lights
        for (WalkingAgent * wa : WalkingAgents)
        {
            if (wa->MemoryLight != nullptr)
            {
                RemoveMemoryLight6FromMap(wa->MemoryLight);
                delete wa->MemoryLight;
            }
        }


        for (WalkingAgent * wa : WalkingAgents)
        {
            if (wa->Light != nullptr)
            {
                RemoveLight6FromMap(wa->Light);
                delete wa->Light;
            }
        }

        struct timeval startr, endr;
        gettimeofday(&startr, NULL);

        //for(auto lightCache : LightCachesToRemove)
        //    lightWorldMap->DeleteLightCache(lightCache);
        lightWorldMap->ProcessLightCaches();

        ////
        gettimeofday(&endr, NULL);
        double secondsr, usecondsr;

        secondsr  = endr.tv_sec  - startr.tv_sec;
        usecondsr = endr.tv_usec - startr.tv_usec;

        double mtimer = ((secondsr) * 1000 + usecondsr/1000.0);


        //sTitle << "MTr " << mtimer << " ms. ";



        Tile * t = worldMap->operator ()(109, 77, 155);std::vector<MyColor> EGAColors;

        int EGAColorsMappingQuantity = 32;
        MyColor * EGAColorsMapping;

        //sTitle << "Q " << t->LightedByMemoryLights.size() << ". ";





        ////
        for (auto wa : WalkingAgents)
            IterateAgent(wa);


        for (WalkingAgent * wa : WalkingAgents)
        {
            wa->MemoryLight = new LightVisibilityCache6(wa->X, wa->Y, wa->Z, MyColor(5000, 5000, 5000));
            PlaceMemoryLight6ToMap(wa->MemoryLight);
        }


        for (WalkingAgent * wa : WalkingAgents)
        {
            wa->Light = new LightVisibilityCache6(wa->X, wa->Y, wa->Z, MyColor(3000, 3000, 3000));
            PlaceLight6ToMap(wa->Light);
        }


        ////
        TraceVisibilityCached6AsMemory(MyLight, false);



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



        sTitle << "MTs " << worldMap->MemoryTilesDL.Count() << " ms. ";
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



