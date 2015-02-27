#ifndef MY3DFACE_H
#define MY3DFACE_H

#include "facedirenum.h"
#include "facefaceenum.h"


namespace dflighting
{


//
class My3DFace
{
public:
    int X;
    int Y;
    int Z;

    FaceFaceEnum Face;
    FaceDirEnum Dir;

    long SourceHits;
    long DestinationHits;

    //
    My3DFace();
    My3DFace(int x, int y, int z, FaceFaceEnum face);
};

}

#endif // MY3DFACE_H
