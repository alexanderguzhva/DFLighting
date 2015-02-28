#ifndef MAPCOMPUTECELLS_H
#define MAPCOMPUTECELLS_H


#include <vector>
#include <algorithm>

#include "time.h"
#include <sys/time.h>

#include "mapcomputecell.h"
#include "tile.h"
#include "myintpoint.h"

#include "worldmap.h"

namespace dflighting
{


class MapComputeCells
{
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
    MapComputeCells(int nLayers);
    ~MapComputeCells();

    //
    int GetIndex(int x, int y, int z) const;

    //
    MapComputeCell & operator() (int x, int y, int z)
    {
        return ComputingCells[GetIndex(x + NLayers, y + NLayers, z)];
    }

    //
    void ClearComputingCells();

    //
    void GenerateLinesCombined(int nGenPointsUF);


    //// 0 = left, 1 = right, 2 = top, 3 = bottom, 4 = up, 5 = down
    template<Direction dir>
    void TraceVisibility3(int stx, int sty, int stz, const MyIntensity & intensity, int maxLayers, WorldMap * worldMap)
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
        int layersToProcess = std::min(maxLayers, NLayers);

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
                case Direction::Left:
                    shouldBreak = (stx - layer - 1 < 0);
                    break;
                case Direction::Right:
                    shouldBreak = (stx + layer + 1 >= worldMap->x_size);
                    break;
                case Direction::Top:
                    shouldBreak = (sty - layer - 1 < 0);
                    break;
                case Direction::Bottom:
                    shouldBreak = (sty + layer + 1 >= worldMap->y_size);
                    break;
                case Direction::Up:
                    shouldBreak = (stz + layer + 1 >= worldMap->z_size);
                    break;
                case Direction::Down:
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
                            case Direction::Left:
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
                            case Direction::Right:
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
                            case Direction::Top:
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
                            case Direction::Bottom:
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
                            case Direction::Up:
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
                            case Direction::Down:
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


    //
    void ClearITraced();

    //
    void SaveToStream(std::ostream * stream);

    //
    void LoadFromStream(std::istream * stream);
};




}

#endif // MAPCOMPUTECELLS_H
