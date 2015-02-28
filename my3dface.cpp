#include "my3dface.h"

namespace dflighting
{

//
My3DFace::My3DFace() : X(0), Y(0), Z(0)
{
    SourceHits = 0;
    DestinationHits = 0;
}


My3DFace::My3DFace(int x, int y, int z, Direction face) : X(x), Y(y), Z(z), Face(face)
{
    switch(Face)
    {
        case Direction::Left:
        case Direction::Right:
            Dir = FaceDirEnum::XEq;
            break;
        case Direction::Top:
        case Direction::Bottom:
            Dir = FaceDirEnum::YEq;
            break;
        case Direction::Up:
        case Direction::Down:
            Dir = FaceDirEnum::ZEq;
            break;
    }

    ////
    SourceHits = 0;
    DestinationHits = 0;
}


}
