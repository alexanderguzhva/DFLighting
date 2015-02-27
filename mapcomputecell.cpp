#include "mapcomputecell.h"

namespace dflighting
{


void MapComputeCell::Clear()
{
    for (int i = 0; i < 3; i++)
        for (int j = 0;j < 3; j++)
            Directions[i][j] = 0;

    Intensity = 0;
    IsProcessMarked = false;
    IsWithinLightedCellsArray = false;
}

//
void MapComputeCell::SaveToStream(std::ostream * stream)
{
    for (int i = 0; i < 3; i++)
        for (int j = 0;j < 3; j++)
            stream->write(reinterpret_cast<char*>(&(Directions[i][j])), sizeof(float));
}

//
void MapComputeCell::LoadFromStream(std::istream * stream)
{
    for (int i = 0; i < 3; i++)
        for (int j = 0;j < 3; j++)
            stream->read(reinterpret_cast<char*>(&(Directions[i][j])), sizeof(float));
}


}
