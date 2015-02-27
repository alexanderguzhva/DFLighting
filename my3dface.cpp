#include "my3dface.h"

namespace dflighting
{

//
My3DFace::My3DFace() : X(0), Y(0), Z(0)
{
    SourceHits = 0;
    DestinationHits = 0;
}


My3DFace::My3DFace(int x, int y, int z, FaceFaceEnum face) : X(x), Y(y), Z(z), Face(face)
{
    switch(Face)
    {
        case FaceFaceEnum::Left:
        case FaceFaceEnum::Right:
            Dir = FaceDirEnum::XEq;
            break;
        case FaceFaceEnum::Top:
        case FaceFaceEnum::Bottom:
            Dir = FaceDirEnum::YEq;
            break;
        case FaceFaceEnum::Up:
        case FaceFaceEnum::Down:
            Dir = FaceDirEnum::ZEq;
            break;
    }

    ////
    SourceHits = 0;
    DestinationHits = 0;
}


}
