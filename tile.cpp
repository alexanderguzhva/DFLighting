#include "tile.h"


namespace dflighting
{


Tile::Tile()
{
    Flow = nullptr;
    Mat = nullptr;
    Material_Index = -1;
    Material_Type = -1;
    Tile_Material = TileMaterialType::AIR;
    Tile_Type = TileType::EMPTY;
    ColorValueI = 0;
    LightValueITraced = 0;

    IsWithinLightedTilesTraced = false;

    IsPassableByLight = false;
    IsPassableByLight_HoleDown = false;
    IsPassableByLight_HoleUp = false;

    IsPassableByDwarf = false;
    IsPassableByDwarf_HoleDown = false;
    IsPassableByDwarf_HoleUp = false;

    IsDoor = false;
    IsDoorOpened = false;

    IsPathfinded = false;

    MemoryAmount = 0;
    IsMemoryAffected = false;

    CopyForMemory = nullptr;
}

//
void Tile::ModifyPassableFlags()
{
    ////
    IsPassableByLight = false;
    IsPassableByLight_HoleDown = false;
    IsPassableByLight_HoleUp = false;

    ////
    IsPassableByDwarf = false;
    IsPassableByDwarf_HoleDown = false;
    IsPassableByDwarf_HoleUp = false;

    ////
    if (Tile_Type == TileType::FLOOR ||
        Tile_Type == TileType::EMPTY ||
        Tile_Type == TileType::STAIR_UP ||
        Tile_Type == TileType::STAIR_DOWN ||
        Tile_Type == TileType::STAIR_UPDOWN ||
        Tile_Type == TileType::SHRUB ||
        Tile_Type == TileType::SAPLING ||
        Tile_Type == TileType::RAMP ||
        Tile_Type == TileType::RAMP_TOP ||
        Tile_Type == TileType::PEBBLES ||
        Tile_Type == TileType::BOULDER)
    {
        IsPassableByLight = true;

        IsPassableByDwarf = true;
    }

    /*
    ////
    if (Tile_Type == TileType::EMPTY ||
        Tile_Type == TileType::STAIR_DOWN ||
        Tile_Type == TileType::STAIR_UPDOWN ||
        Tile_Type == TileType::RAMP)
    {
        IsPassable_HoleDown = true;
    }

    ////
    if (Tile_Type == TileType::EMPTY ||
        Tile_Type == TileType::STAIR_UP ||
        Tile_Type == TileType::STAIR_UPDOWN ||
        Tile_Type == TileType::RAMP_TOP ||
        Tile_Type == TileType::FLOOR)
    {
        IsPassable_HoleUp = true;
    }
    */

    ////
    if (Tile_Type == TileType::EMPTY ||
        Tile_Type == TileType::STAIR_UP ||
        Tile_Type == TileType::STAIR_UPDOWN ||
        Tile_Type == TileType::RAMP_TOP)
    {
        IsPassableByLight_HoleDown = true;

        IsPassableByDwarf_HoleDown = true;
    }

    ////
    if (Tile_Type == TileType::EMPTY ||
        Tile_Type == TileType::STAIR_DOWN ||
        Tile_Type == TileType::STAIR_UPDOWN ||
        Tile_Type == TileType::RAMP ||
        Tile_Type == TileType::FLOOR)
    {
        IsPassableByLight_HoleUp = true;

        IsPassableByDwarf_HoleUp = true;
    }

    //
    if (IsDoor)
    {
        if (IsPassableByLight && !IsDoorOpened)
            IsPassableByLight = false;
        if (IsPassableByLight_HoleDown && !IsDoorOpened)
            IsPassableByLight_HoleDown = false;
        if (IsPassableByLight_HoleUp && !IsDoorOpened)
            IsPassableByLight_HoleUp = false;

        IsPassableByDwarf = true;
    }

}

//
void Tile::InitializeCopyForMemory()
{
    if (this->CopyForMemory == nullptr)
        this->CopyForMemory = new Tile;
    Tile * tile = this->CopyForMemory;

    tile->Flow = this->Flow;
    tile->Material_Index = this->Material_Index;
    tile->Material_Type = this->Material_Type;
    tile->Tile_Material = this->Tile_Material;
    tile->Tile_Type = this->Tile_Type;

    tile->IsPathfinded = this->IsPathfinded;

    tile->IsDoor = this->IsDoor;
}

//
void Tile::ModifyCopyForMemory()
{
    Tile * tile = this->CopyForMemory;
    tile->ColorValueI = this->ColorValueI;
    tile->IsDoorOpened = this->IsDoorOpened;
}

//
void Tile::DeleteCopyForMemory()
{
    if (this->CopyForMemory != nullptr)
    {
        delete this->CopyForMemory;
        this->CopyForMemory = nullptr;
    }
}

//
Tile::~Tile()
{
    DeleteCopyForMemory();
}




}
