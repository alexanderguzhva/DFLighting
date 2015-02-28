#include "lightvisibilitycache6.h"

namespace dflighting
{

//
LightVisibilityCache6::LightVisibilityCache6(int x, int y, int z, MyColor color)
    : WorldX(x), WorldY(y), WorldZ(z), Color(color)
{
    LeftLight = new LightVisibilityCache1(x, y, z, 0, color);
    RightLight = new LightVisibilityCache1(x, y, z, 1, color);
    TopLight = new LightVisibilityCache1(x, y, z, 2, color);
    BottomLight = new LightVisibilityCache1(x, y, z, 3, color);
    UpLight = new LightVisibilityCache1(x, y, z, 4, color);
    DownLight = new LightVisibilityCache1(x, y, z, 5, color);
}

//
LightVisibilityCache6::~LightVisibilityCache6()
{
    if (UpLight != nullptr) { delete UpLight; UpLight = nullptr; }
    if (DownLight != nullptr) { delete DownLight; DownLight = nullptr; }
    if (LeftLight != nullptr) { delete LeftLight; LeftLight = nullptr; }
    if (RightLight != nullptr) { delete RightLight; RightLight = nullptr; }
    if (TopLight != nullptr) { delete TopLight; TopLight = nullptr; }
    if (BottomLight != nullptr) { delete BottomLight; BottomLight = nullptr; }
}


}
