#include "worldmap.h"

#include <unordered_set>

namespace dflighting
{


//
WorldMap::WorldMap(int xs, int ys, int zs)
{
    x_size = xs;
    y_size = ys;
    z_size = zs;

    Tiles = new Tile*[z_size * x_size * y_size];
    for (int i = 0; i < z_size * x_size * y_size; i++)
        Tiles[i] = nullptr;
}


//
int WorldMap::GetIndex(int x, int y, int z) const
{
    return x + x_size * (y + y_size * z);
}


//
WorldMap::~WorldMap()
{
    if (Tiles != nullptr)
    {
        delete[] Tiles;
        Tiles = nullptr;
    }

    ////
    MemoryTilesDL.Clear();

    ////
    for (auto material : inorganic_material)
        delete material;
    inorganic_material.clear();

    for (auto material : organic_material)
        delete material;
    organic_material.clear();
}



//
void WorldMap::ProcessMemoryTiles()
{
    if (MemoryTilesDL.Count() == 0)
        return;

    int element = MemoryTilesDL.First();

    while (element != -1)
    {
        MyDoubleLinkedListElement<Tile> * currentItem = MemoryTilesDL.operator ()(element);
        Tile * tile = currentItem->Data;

        element = currentItem->Next;

        int & memoryAmount = tile->MemoryAmount;
        memoryAmount -= 1;

        if (memoryAmount <= 0)
        {
            //// remove element
            MemoryTilesDL.DeleteItem(tile->DLIndex);
            tile->IsMemoryAffected = false;
        }
    }

    /*
    ////
    std::vector<Tile *> newMemoryTiles;
    newMemoryTiles.reserve(MemoryTiles.size());

    std::vector<Tile *> removeMemoryTiles;
    //removeMemoryTiles.reserve(MemoryTiles->size());

    for (int i = 0; i < MemoryTiles.size(); i++)
    {
        Tile * tile = MemoryTiles[i];

        int & memoryAmount = tile->MemoryAmount;
        //tile->MemoryAmount -= 1;
        memoryAmount -= 1;

        if (memoryAmount > 0)
        {
            newMemoryTiles.emplace_back(tile);
        }
        else
        {
            removeMemoryTiles.emplace_back(tile);
        }
    }

    for (auto tile : removeMemoryTiles)
    {
        tile->MemoryAmount = 0;
        tile->IsMemoryAffected = false;

        tile->DeleteCopyForMemory();
    }

    //MemoryTiles->clear();
    MemoryTiles.resize(newMemoryTiles.size());
    std::copy(newMemoryTiles.begin(), newMemoryTiles.end(), MemoryTiles.begin());
    */
}


//
void WorldMap::TrackNeighbours(int x, int y, int z)
{
    Tile * tile = (*this)(x, y, z);
    if (tile == nullptr)
        return;

    tile->Neighbours.clear();
    if (!tile->IsPassableByDwarf)
        return;

    ////
    std::unordered_set<Tile *> tiles(tile->Neighbours);

    for (auto t : tiles)
        t->Neighbours.erase(tile);

    ////
    tile->Neighbours.clear();

    for (int ax = -1; ax <= 1; ax++)
    {
        for (int ay = -1; ay <= 1; ay++)
        {
            for (int az = -1; az <= 1; az++)
            {
                if (ax == 0 && ay == 0 && az == 0)
                    continue;

                //// no diagonal towards z
                if (az != 0 && (ay != 0 || ax != 0))
                    continue;

                int targetX = ax + tile->X;
                int targetY = ay + tile->Y;
                int targetZ = az + tile->Z;

                if (targetX < 0 || targetX >= x_size ||
                    targetY < 0 || targetY >= y_size ||
                    targetZ < 0 || targetZ >= z_size)
                {
                    continue;
                }

                ////
                Tile * targetTile = (*this)(targetX, targetY, targetZ);
                if (targetTile == nullptr)
                    continue;


                ////
                bool canGoFromAToB;
                if (az == 1)
                {
                    // STAIR_UP = 6,
                    // STAIR_DOWN = 7,
                    // STAIR_UPDOWN = 8,
                    // RAMP = 9,
                    // RAMP_TOP = 10,

                    //// go up
                    canGoFromAToB =
                            (tile->Tile_Type == TileType::STAIR_UPDOWN || tile->Tile_Type == TileType::STAIR_UP || tile->Tile_Type == TileType::RAMP) &&
                            (targetTile->Tile_Type == TileType::STAIR_UPDOWN || targetTile->Tile_Type == TileType::STAIR_DOWN || targetTile->Tile_Type == TileType::RAMP_TOP);
                }
                else if (az == -1)
                {
                    //// go down
                    canGoFromAToB =
                            (targetTile->Tile_Type == TileType::STAIR_UPDOWN || targetTile->Tile_Type == TileType::STAIR_UP || targetTile->Tile_Type == TileType::RAMP) &&
                            (tile->Tile_Type == TileType::STAIR_UPDOWN || tile->Tile_Type == TileType::STAIR_DOWN || tile->Tile_Type == TileType::RAMP_TOP);
                }
                else
                {
                    //// same level
                    canGoFromAToB =
                            (tile->IsPassableByDwarf && tile->Flow == nullptr && tile->Tile_Type != TileType::EMPTY) &&
                            (targetTile->IsPassableByDwarf && targetTile->Flow == nullptr && targetTile->Tile_Type != TileType::EMPTY);

                }

                ////
                if (canGoFromAToB)
                {
                    ////
                    targetTile->Neighbours.insert(tile);
                    tile->Neighbours.insert(targetTile);
                }
            }
        }
    }
}


//
void WorldMap::CreateNeighbours()
{
    ////
    for (int i = 0; i < x_size; i++)
    {
        for (int j = 0; j < y_size; j++)
        {
            for (int k = 0; k < z_size; k++)
            {
                this->TrackNeighbours(i, j, k);
            }
        }
    }
}






}
