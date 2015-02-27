#ifndef MYDOUBLEPOINT_H
#define MYDOUBLEPOINT_H

namespace dflighting
{
class MyDoublePoint
{
public:
    double X;
    double Y;
    double Z;

    MyDoublePoint() { }
    MyDoublePoint(double x, double y, double z) : X(x), Y(y), Z(z)
    {
    }
};

}

#endif // MYDOUBLEPOINT_H

