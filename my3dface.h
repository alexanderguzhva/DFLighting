#ifndef MY3DFACE_H
#define MY3DFACE_H

#include "facedirenum.h"
#include "direction.h"


namespace dflighting
{


//
class My3DFace
{
public:
    int X;
    int Y;
    int Z;

    Direction Face;
    FaceDirEnum Dir;

    long SourceHits;
    long DestinationHits;

    //
    My3DFace();
    My3DFace(int x, int y, int z, Direction face);
};

}

#endif // MY3DFACE_H
