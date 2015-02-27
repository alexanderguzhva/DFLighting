#ifndef EGACOLORSMAPPER_H
#define EGACOLORSMAPPER_H

#include <vector>

#include "mycolor.h"

namespace dflighting
{

class EGAColorsMapper
{
public:
    std::vector<MyColor> EGAColors;

    const int EGAColorsMappingQuantity = 32;
    MyColor * EGAColorsMapping;


    //
    EGAColorsMapper();
    ~EGAColorsMapper();


    //
    void CreateMapping();

};

}

#endif // EGACOLORSMAPPER_H
