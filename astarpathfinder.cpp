#include "astarpathfinder.h"

#include <unordered_set>
#include <cmath>

#include "astarpath.h"
#include "astarpriorityqueue.h"


namespace dflighting
{


// A*
std::vector<Tile *> * AStarPathFinder::FindPath(
            Tile * start,
            Tile * destination)
{
    std::vector<AStarPath *> pathsAllocated;

    //// algo
    std::unordered_set<Tile *> closed;
    auto priorityQueue = new AStarPriorityQueue<double, AStarPath>;

    ////
    auto initialPath = new AStarPath(start);
    priorityQueue->Enqueue(0, initialPath);

    pathsAllocated.push_back(initialPath);

    AStarPath * stPath = nullptr;


    while (!priorityQueue->IsEmpty())
    {
        ////
        auto path = priorityQueue->Dequeue();

        //// internal tracking
        path->LastStep->IsPathfinded = true;

        //// is item inside? continue
        if (closed.find(path->LastStep) != closed.end())
            continue;

        ////
        if (path->LastStep == destination)
        {
            //// found it
            stPath = path;
            break;
        }

        closed.insert(path->LastStep);

        for (auto n : path->LastStep->Neighbours)
        {
            Tile * node = n;

            //double d = 1; //// distance
            double d = std::sqrt((node->X - path->LastStep->X) * (node->X - path->LastStep->X) +
                            (node->Y - path->LastStep->Y) * (node->Y - path->LastStep->X) +
                            (node->Z - path->LastStep->Z) * (node->Z - path->LastStep->X));

            double estimate = std::sqrt((node->X - destination->X) * (node->X - destination->X) +
                                   (node->Y - destination->Y) * (node->Y - destination->X) +
                                   (node->Z - destination->Z) * (node->Z - destination->X));


            auto newPath = path->AddStep(node, d);
            pathsAllocated.push_back(newPath);

            priorityQueue->Enqueue(newPath->TotalCost + estimate, newPath);
        }
    }

    //// generate output
    std::vector<Tile *> * outputVector = nullptr;
    if (stPath != nullptr)
        outputVector = stPath->GetNodes();

    //// delete all paths that were allocated
    for (auto path : pathsAllocated)
        delete path;

    ////
    delete priorityQueue;

    ////
    return outputVector;
}


}

