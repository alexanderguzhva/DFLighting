#ifndef ASTARPATHFINDER_H
#define ASTARPATHFINDER_H

#include <vector>

#include "tile.h"

namespace dflighting
{

// A*
class AStarPathFinder
{
public:
    static std::vector<Tile *> * FindPath(
            Tile * start,
            Tile * destination);
};


}


#endif // ASTARPATHFINDER_H
