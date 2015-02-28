#include "astarpath.h"

#include <stack>


namespace dflighting
{

//
AStarPath::AStarPath(Tile * lastStep, AStarPath * previousSteps, double totalCost)
{
    LastStep = lastStep;
    PreviousSteps = previousSteps;
    TotalCost = totalCost;
}

//
AStarPath * AStarPath::AddStep(Tile * step, double stepCost)
{
    AStarPath * newPath = new AStarPath(step, this, TotalCost + stepCost);
    return newPath;
}

//
std::vector<Tile *> * AStarPath::GetNodes()
{
    auto outList = new std::vector<Tile *>;

    std::stack<Tile*> outStack;

    ////
    AStarPath * path = this;

    while (path != nullptr)
    {
        outStack.push(path->LastStep);
        path = path->PreviousSteps;
    }

    while (!outStack.empty())
    {
        Tile * tile = outStack.top();
        outList->push_back(tile);
        outStack.pop();
    }

    return outList;
}

}

