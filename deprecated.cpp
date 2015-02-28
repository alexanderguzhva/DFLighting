









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

