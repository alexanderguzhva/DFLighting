#include "walkingagent.h"



namespace dflighting
{

//
WalkingAgent::WalkingAgent()
{
    WalkingPath = nullptr;

    MemoryLight = nullptr;
    Light = nullptr;
}

//
WalkingAgent::~WalkingAgent()
{
    if (WalkingPath != nullptr)
    {
        delete WalkingPath;
        WalkingPath = nullptr;
    }

    if (MemoryLight != nullptr)
    {
        delete MemoryLight;
        MemoryLight = nullptr;
    }

    if (Light != nullptr)
    {
        delete Light;
        Light = nullptr;
    }
}

}
