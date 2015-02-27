#ifndef MYINTPOINT_H
#define MYINTPOINT_H

namespace dflighting
{

// a point in 3D space
class MyIntPoint
{
public:
    int X;
    int Y;
    int Z;

    MyIntPoint() { }
    MyIntPoint(int x, int y, int z) : X(x), Y(y), Z(z)
    {
        X = x;
        Y = y;
        Z = z;
    }
};

}

#endif // MYINTPOINT_H

