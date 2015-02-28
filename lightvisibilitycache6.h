#ifndef LIGHTVISIBILITYCACHE6_H
#define LIGHTVISIBILITYCACHE6_H


#include "lightvisibilitycache1.h"
#include "mycolor.h"


namespace dflighting
{


//
class LightVisibilityCache6
{
public:
    int WorldX;
    int WorldY;
    int WorldZ;

    MyColor Color;

    LightVisibilityCache1 * LeftLight;
    LightVisibilityCache1 * RightLight;
    LightVisibilityCache1 * TopLight;
    LightVisibilityCache1 * BottomLight;
    LightVisibilityCache1 * UpLight;
    LightVisibilityCache1 * DownLight;

    //
    LightVisibilityCache6(int x, int y, int z, MyColor color);
    ~LightVisibilityCache6();
};


}


#endif // LIGHTVISIBILITYCACHE6_H
