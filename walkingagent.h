#ifndef WALKINGAGENT_H
#define WALKINGAGENT_H

#include <vector>

#include "tile.h"
#include "lightvisibilitycache6.h"


namespace dflighting
{

////
class WalkingAgent
{
public:
    int X;
    int Y;
    int Z;

    int XPicture;
    int YPicture;

    std::vector<Tile *> * WalkingPath;
    int CurrentTile;

    int LightPower;

    LightVisibilityCache6 * MemoryLight;
    LightVisibilityCache6 * Light;

    //
    WalkingAgent();
    ~WalkingAgent();
};

}



#endif // WALKINGAGENT_H
