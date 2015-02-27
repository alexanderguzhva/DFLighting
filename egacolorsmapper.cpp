#include "egacolorsmapper.h"


namespace dflighting
{


//
EGAColorsMapper::EGAColorsMapper()
{
    EGAColorsMapping = nullptr;
}


//
EGAColorsMapper::~EGAColorsMapper()
{
    if (EGAColorsMapping != nullptr)
    {
        delete [] EGAColorsMapping;
        EGAColorsMapping = nullptr;
    }
}


//
void EGAColorsMapper::CreateMapping()
{
    //// std ega palette
    EGAColors.push_back(MyColor(0x00, 0x00, 0x00));
    EGAColors.push_back(MyColor(0x00, 0x00, 0xAA));
    EGAColors.push_back(MyColor(0x00, 0xAA, 0x00));
    EGAColors.push_back(MyColor(0x00, 0xAA, 0xAA));

    EGAColors.push_back(MyColor(0xAA, 0x00, 0x00));
    EGAColors.push_back(MyColor(0xAA, 0x00, 0xAA));
    EGAColors.push_back(MyColor(0xAA, 0x55, 0x00));
    EGAColors.push_back(MyColor(0xAA, 0xAA, 0xAA));

    EGAColors.push_back(MyColor(0x55, 0x55, 0x55));
    EGAColors.push_back(MyColor(0x55, 0x55, 0xFF));
    EGAColors.push_back(MyColor(0x55, 0xFF, 0x55));
    EGAColors.push_back(MyColor(0x55, 0xFF, 0xFF));

    EGAColors.push_back(MyColor(0xFF, 0x55, 0x55));
    EGAColors.push_back(MyColor(0xFF, 0x55, 0xFF));
    EGAColors.push_back(MyColor(0xFF, 0xFF, 0x55));
    EGAColors.push_back(MyColor(0xFF, 0xFF, 0xFF));


    ////
    EGAColorsMapping = new MyColor[EGAColorsMappingQuantity * EGAColorsMappingQuantity * EGAColorsMappingQuantity];

    int amount = 256 / EGAColorsMappingQuantity;
    for (int r = 0; r < EGAColorsMappingQuantity; r++)
    {
        int rc = r * amount;
        for (int g = 0; g < EGAColorsMappingQuantity; g++)
        {
            int gc = g * amount;
            for (int b = 0; b < EGAColorsMappingQuantity; b++)
            {
                int bc = b * amount;

                int minIndex = 0;
                int index = 0;
                double mindistance = 9999999999999999;
                for (auto color : EGAColors)
                {
                    double distance =
                            (rc - color.R) * (rc - color.R) +
                            (gc - color.G) * (gc - color.G) +
                            (bc - color.B) * (bc - color.B);

                    if (mindistance > distance)
                    {
                        mindistance = distance;
                        minIndex = index;
                    }

                    index += 1;
                }

                EGAColorsMapping[(r * EGAColorsMappingQuantity + g) * EGAColorsMappingQuantity + b] = EGAColors[minIndex];
            }
        }
    }

}



}
