#ifndef ASTARPATH_H
#define ASTARPATH_H


#include <vector>

#include "tile.h"


/////////////////
/////// A* from Eric Lippert


namespace dflighting
{

class AStarPath
{
public:
    Tile * LastStep;
    AStarPath * PreviousSteps;
    double TotalCost;

    //
    AStarPath(Tile * lastStep, AStarPath * previousSteps, double totalCost);
    AStarPath(Tile * nodeStart) : AStarPath(nodeStart, nullptr, 0) {}

    //
    AStarPath * AddStep(Tile * step, double stepCost);

    //
    std::vector<Tile *> * GetNodes();
};

}

#endif // ASTARPATH_H
