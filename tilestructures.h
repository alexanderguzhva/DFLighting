#ifndef TILESTRUCTURES_H
#define TILESTRUCTURES_H

#include <string>

namespace dflighting
{

enum TileType
{
    EMPTY = 0,
    FLOOR = 1,
    BOULDER = 2,
    PEBBLES = 3,
    WALL = 4,
    FORTIFICATION = 5,
    STAIR_UP = 6,
    STAIR_DOWN = 7,
    STAIR_UPDOWN = 8,
    RAMP = 9,
    RAMP_TOP = 10,
    BROOK_BED = 11,
    BROOK_TOP = 12,
    TREE = 13,
    SAPLING = 14,
    SHRUB = 15,
    ENDLESS_PIT = 16
};

enum LiquidType
{
    WATER = 0,
    MAGMA = 1
};

enum TileMaterialType
{
    AIR = 0,
    SOIL = 1,
    STONE = 2,
    FEATURE = 3,
    LAVA_STONE = 4,
    MINERAL = 5,
    FROZEN_LIQUID = 6,
    CONSTRUCTION = 7,
    GRASS_LIGHT = 8,
    GRASS_DARK = 9,
    GRASS_DRY = 10,
    GRASS_DEAD = 11,
    HFS = 12,
    CAMPFIRE = 13,
    FIRE = 14,
    ASHES = 15,
    MAGMA_TYPE = 16,
    DRIFTWOOD = 17,
    POOL = 18,
    BROOK = 19,
    RIVER = 20
};

struct Plant
{
    uint32_t x;
    uint32_t y;
    bool is_shrub;
    uint32_t material;
};

struct Material
{
    uint32_t index;
    std::string name;
};

//
struct FlowData
{
    LiquidType Liquid_type;
    uint32_t Flow_size;
};


}

#endif // TILESTRUCTURES_H

