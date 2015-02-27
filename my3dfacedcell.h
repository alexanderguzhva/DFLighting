#ifndef MY3DFACEDCELL_H
#define MY3DFACEDCELL_H

#include "my3dface.h"

namespace dflighting
{

//
class My3DFacedCell
{
public:
    int X;
    int Y;
    int Z;

    My3DFace * LeftFace;
    My3DFace * RightFace;
    My3DFace * TopFace;
    My3DFace * BottomFace;
    My3DFace * UpFace;
    My3DFace * DownFace;

    My3DFacedCell();
    My3DFacedCell(int x, int y, int z);
};


}

#endif // MY3DFACEDCELL_H
