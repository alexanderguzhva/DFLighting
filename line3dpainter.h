#ifndef LINE3DPAINTER_H
#define LINE3DPAINTER_H

#include <vector>

#include "myintpoint.h"

namespace dflighting
{

class Line3DPainter
{
public:
    static void Draw(int x1, int y1, int z1, int x2, int y2, int z2,
                    int xmin, int xmax, int ymin, int ymax, int zmin, int zmax,
                    std::vector<MyIntPoint> & vec);
};


}


#endif // LINE3DPAINTER_H
