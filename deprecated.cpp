









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











/*
class MapComputeCells
{

    //// 0 = left, 1 = right, 2 = top, 3 = bottom, 4 = up, 5 = down
    template<int dir>
    void TraceVisibility2(int stx, int sty, int stz, const MyIntensity & intensity, WorldMap * worldMap)
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

};
*/
