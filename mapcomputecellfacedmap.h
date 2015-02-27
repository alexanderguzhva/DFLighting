#ifndef MAPCOMPUTECELLFACEDMAP_H
#define MAPCOMPUTECELLFACEDMAP_H


#include "my3dfacedcell.h"

namespace dflighting
{

const int NLayersFaced = 20;

////
class MapComputeCellFacedMap
{
public:

    My3DFacedCell ** ComputingCellsFaced;

    int x_size;
    int y_size;
    int z_size;

    //
    MapComputeCellFacedMap();
    ~MapComputeCellFacedMap();

    //
    My3DFacedCell *& operator() (int x, int y, int z)
    {
        return ComputingCellsFaced[x + NLayersFaced + x_size * (y + NLayersFaced + y_size * z)];
    }
};

}


#endif // MAPCOMPUTECELLFACEDMAP_H
