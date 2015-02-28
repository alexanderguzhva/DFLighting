#include "lightvisibilitycache1.h"

#include <algorithm>


namespace dflighting
{




float MaxLayersSupplemental[] = {
    1.0f,
    0.112436f,
    0.0517176f,
    0.0294527f,
    0.0189217f,
    0.0131949f,
    0.00972241f,
    0.00744677f,
    0.00590197f,
    0.00478206f,
    0.0039569f,
    0.00332408f,
    0.0028316f,
    0.00243792f,
    0.00212429f,
    0.0018727f,
    0.00165953f,
    0.00148131f,
    0.00133203f,
    0.00120218f,
    0.00109126f,
    0.000996209f,
    0.000911559f,
    0.000835555f,
    0.000771584f,
    0.000714264f,
    0.000662083f,
    0.000616493f,
    0.000575952f,
    0.000533264f,
    0.000498379f,
    0.000469099f,
    0.000438034f,
    0.000413938f,
    0.00039078f,
    0.00037119f,
    0.000351478f,
    0.000332432f,
    0.000316621f,
    0.000299328f,
    0.000287598f,
    0.000273026f,
    0.000262626f,
    0.000248931f,
    0.000238622f,
    0.000228403f,
    0.000221178f,
    0.000210748f,
    0.000202615f,
    0.000193727f,
    0.000185988f,
    0.00017843f,
    0.000172323f,
    0.000167939f,
    0.000160925f,
    0.000154214f,
    0.000149951f,
    0.00014327f,
    0.000139007f,
    0.000135047f,
    0.000131026f,
    0.000127126f,
    0.000122077f,
    0.000118419f,
    0.000116454f,
    0.000113189f,
    0.000109198f,
    0.000105933f,
    0.000102003f,
    0.000100491f,
    9.68939e-05f,
    9.53823e-05f,
    9.21474e-05f,
    9.03637e-05f,
    8.6887e-05f,
    8.54359e-05f,
    8.25336e-05f,
    8.10825e-05f,
    7.92988e-05f,
    7.59128e-05f,
    0};

float MinIntensityToSee = 32;





//
LightVisibilityCache1::LightVisibilityCache1(int x, int y, int z, Direction dir, MyColor color)
{
    WorldX = x;
    WorldY = y;
    WorldZ = z;
    Dir = dir;

    //LightCache = nullptr;

    Color = color;
    MaxLayers = Color2MaxLayers(color);
}

//
int LightVisibilityCache1::Color2MaxLayers(const MyColor &color)
{
   // return 40;

    float maxv = std::max(color.R, std::max(color.G, color.B));

    ////
    for (int i = 0; i < 80; i++)
    {
        if (maxv < MinIntensityToSee / MaxLayersSupplemental[i])
            return i + 1;
    }

    return 80;
}

//
void LightVisibilityCache1::ChangeColor(const MyColor & newColor)
{
    Color = newColor;
    MaxLayers = Color2MaxLayers(newColor);
}

//
MyIntDPoint LightVisibilityCache1::GetIntDPoint() const
{
    return MyIntDPoint(WorldX, WorldY, WorldZ, Dir);
}


}
