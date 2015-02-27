#ifndef MYDOUBLEEQPOINT_H
#define MYDOUBLEEQPOINT_H

#include "mydoublepoint.h"


namespace dflighting
{

//
class MyDoubleEqPoint : public MyDoublePoint
{
public:
    bool XEq;
    bool YEq;
    bool ZEq;

    unsigned int NEqs;

    //
    MyDoubleEqPoint();
    MyDoubleEqPoint(double x, double y, double z, bool xeq, bool yeq, bool zeq);
};


}


#endif // MYDOUBLEEQPOINT_H
