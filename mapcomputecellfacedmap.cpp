#include "mapcomputecellfacedmap.h"

namespace dflighting
{

//
MapComputeCellFacedMap::MapComputeCellFacedMap()
{
    x_size = 2 * NLayersFaced + 1;
    y_size = 2 * NLayersFaced + 1;
    z_size = NLayersFaced + 1;

    int amount = x_size * y_size * z_size;
    ComputingCellsFaced = new My3DFacedCell * [amount];

    for (int x = -NLayersFaced; x <= NLayersFaced; x++)
        for (int y = -NLayersFaced; y <= NLayersFaced; y++)
            for (int z = 0; z <= NLayersFaced; z++)
                this->operator ()(x, y, z) = new My3DFacedCell(x, y, z);
}

//
MapComputeCellFacedMap::~MapComputeCellFacedMap()
{
    if (ComputingCellsFaced != nullptr)
    {
        int amount = x_size * y_size * z_size;
        for (int i = 0; i < amount; i++)
        {
            if (ComputingCellsFaced[i] != nullptr)
            {
                delete ComputingCellsFaced[i];
                ComputingCellsFaced[i] = nullptr;
            }
        }

        delete [] ComputingCellsFaced;
        ComputingCellsFaced = nullptr;
    }
}


}
