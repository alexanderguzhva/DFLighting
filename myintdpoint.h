#ifndef MYINTDPOINT_H
#define MYINTDPOINT_H

#include <functional>
#include <cstddef>

#include "direction.h"

namespace dflighting
{

//
class MyIntDPoint
{
public:
    int X;
    int Y;
    int Z;
    Direction Dir;

    MyIntDPoint() { }
    MyIntDPoint(int x, int y, int z, Direction dir) : X(x), Y(y), Z(z), Dir(dir)
    {
    }

    bool operator ==(const MyIntDPoint &other) const
    {
        return (other.X == this->X &&
                other.Y == this->Y &&
                other.Z == this->Z &&
                other.Dir == this->Dir);
    }
};


// std::hash
struct MyIntDPointHasher
{
    size_t operator()(const MyIntDPoint & point) const
    {
        return point.X + 100 * (point.Y + 100 * (point.Z + 6 * static_cast<int>(point.Dir)));
    }
};


}




#endif // MYINTDPOINT_H

