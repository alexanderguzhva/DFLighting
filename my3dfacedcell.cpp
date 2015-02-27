#include "my3dfacedcell.h"


namespace dflighting
{

//
My3DFacedCell::My3DFacedCell(int x, int y, int z) : X(x), Y(y), Z(z)
{
    LeftFace = new My3DFace(X, Y, Z, FaceFaceEnum::Left);
    RightFace = new My3DFace(X, Y, Z, FaceFaceEnum::Right);
    TopFace = new My3DFace(X, Y, Z, FaceFaceEnum::Top);
    BottomFace = new My3DFace(X, Y, Z, FaceFaceEnum::Bottom);
    UpFace = new My3DFace(X, Y, Z, FaceFaceEnum::Up);
    DownFace = new My3DFace(X, Y, Z, FaceFaceEnum::Down);
}

//
My3DFacedCell::My3DFacedCell() : X(0), Y(0), Z(0)
{
    LeftFace = nullptr;
    RightFace = nullptr;
    TopFace = nullptr;
    BottomFace = nullptr;
    UpFace = nullptr;
    DownFace = nullptr;
}


}
