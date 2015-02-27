#include "mydoubleeqpoint.h"

namespace dflighting
{

//
MyDoubleEqPoint::MyDoubleEqPoint() : MyDoublePoint()
{
}

//
MyDoubleEqPoint::MyDoubleEqPoint(double x, double y, double z, bool xeq, bool yeq, bool zeq) : MyDoublePoint(x, y, z)
{
    XEq = xeq;
    YEq = yeq;
    ZEq = zeq;

    NEqs = 0;
    if (XEq)
        NEqs += 1;
    if (YEq)
        NEqs += 1;
    if (ZEq)
        NEqs += 1;
}


}
