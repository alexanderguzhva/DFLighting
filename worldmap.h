#ifndef WORLDMAP_H
#define WORLDMAP_H

#include <vector>


#include "tile.h"
#include "mydoublelinkedlist.h"

namespace dflighting
{


class WorldMap
{
public:
    int x_size;
    int y_size;
    int z_size;
    std::vector<Material *> inorganic_material;
    std::vector<Material *> organic_material;
    Tile ** Tiles;

    //std::vector<Tile*> MemoryTiles;
    MyDoubleLinkedList<Tile> MemoryTilesDL;

    //
    WorldMap(int xs, int ys, int zs);
    ~WorldMap();

    //
    int GetIndex(int x, int y, int z) const;

    //
    Tile *& operator() (int x, int y, int z)
    {
        return Tiles[GetIndex(x, y, z)];
    }

    Tile *& operator() (int index)
    {
        return Tiles[index];
    }


    //
    void ProcessMemoryTiles();

    //
    void TrackNeighbours(int x, int y, int z);

    //
    void CreateNeighbours();
};


}


#endif // WORLDMAP_H
