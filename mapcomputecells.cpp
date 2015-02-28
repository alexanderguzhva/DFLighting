#include "mapcomputecells.h"

#include "random.h"


namespace dflighting
{


// find maximum of a and b
#define MAX(a,b) (((a)>(b))?(a):(b))

// absolute value of a
#define ABS(a) (((a)<0) ? -(a) : (a))

// take sign of a, either -1, 0, or 1
#define ZSGN(a) (((a)<0) ? -1 : (a)>0 ? 1 : 0)



void MapComputeCells::line3d(int x1, int y1, int z1, int x2, int y2, int z2,
                int xmin, int xmax, int ymin, int ymax, int zmin, int zmax,
                std::vector<MyIntPoint> * vec)
{
    int xd, yd, zd;
    int x, y, z;
    int ax, ay, az;
    int sx, sy, sz;
    int dx, dy, dz;

    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;

    ax = ABS(dx) << 1;
    ay = ABS(dy) << 1;
    az = ABS(dz) << 1;

    sx = ZSGN(dx);
    sy = ZSGN(dy);
    sz = ZSGN(dz);

    x = x1;
    y = y1;
    z = z1;

    if (ax >= MAX(ay, az))            /* x dominant */
    {
        yd = ay - (ax >> 1);
        zd = az - (ax >> 1);
        for (;;)
        {
            if (x < xmin || x > xmax || y < ymin || y > ymax || z < zmin || z > zmax)
                return;

            vec->push_back(MyIntPoint(x, y, z));
            if (x == x2)
            {
                return;
            }

            if (yd >= 0)
            {
                y += sy;
                yd -= ax;
            }

            if (zd >= 0)
            {
                z += sz;
                zd -= ax;
            }

            x += sx;
            yd += ay;
            zd += az;
        }
    }
    else if (ay >= MAX(ax, az))            /* y dominant */
    {
        xd = ax - (ay >> 1);
        zd = az - (ay >> 1);
        for (;;)
        {
            if (x < xmin || x > xmax || y < ymin || y > ymax || z < zmin || z > zmax)
                return;

            vec->push_back(MyIntPoint(x, y, z));
            if (y == y2)
            {
                return;
            }

            if (xd >= 0)
            {
                x += sx;
                xd -= ay;
            }

            if (zd >= 0)
            {
                z += sz;
                zd -= ay;
            }

            y += sy;
            xd += ax;
            zd += az;
        }
    }
    else if (az >= MAX(ax, ay))            /* z dominant */
    {
        xd = ax - (az >> 1);
        yd = ay - (az >> 1);
        for (;;)
        {
            if (x < xmin || x > xmax || y < ymin || y > ymax || z < zmin || z > zmax)
                return;

            vec->push_back(MyIntPoint(x, y, z));
            if (z == z2)
            {
                return;
            }

            if (xd >= 0)
            {
                x += sx;
                xd -= az;
            }

            if (yd >= 0)
            {
                y += sy;
                yd -= az;
            }

            z += sz;
            xd += ax;
            yd += ay;
        }
    }
}


//
MapComputeCells::MapComputeCells(int nLayers)
{
    NLayers = nLayers;

    x_size = (2 * NLayers + 1);
    y_size = (2 * NLayers + 1);
    z_size = NLayers + 1;

    int amount = z_size * x_size * y_size;

    ////
    ComputingCells = new MapComputeCell[amount];

    for (int i = 0; i < amount; i++)
        ComputingCells[i].Clear();

    ////
    LightedTiles = new Tile *[amount];

    for (int i = 0; i < amount; i++)
        LightedTiles[i] = nullptr;

    ////
    LightedTilesSumPerLayer = new int[z_size];
    for (int i = 0; i < z_size; i++)
        LightedTilesSumPerLayer[i] = 0;

    LayersProcessed = 0;

    ////
    //TracePointsNextA = 0;
    //TracePointsNextB = 0;

    TracePointsA = new MyIntPoint [amount];
    TracePointsB = new MyIntPoint [amount];

    ////
    TimeTaken = 0;
    NTimeTaken = 0;

}

//
int MapComputeCells::GetIndex(int x, int y, int z) const
{
    return x + x_size * (y + y_size * z);
}

//
MapComputeCells::~MapComputeCells()
{
    if (ComputingCells == nullptr)
    {
        delete [] ComputingCells;
        ComputingCells = nullptr;
    }

    ////
    if (LightedTiles != nullptr)
    {
        delete [] LightedTiles;
        LightedTiles = nullptr;
    }

    ////
    if (LightedTilesSumPerLayer != nullptr)
    {
        delete [] LightedTilesSumPerLayer;
        LightedTilesSumPerLayer = nullptr;
    }

    ////
    if (TracePointsA != nullptr)
    {
        delete [] TracePointsA;
        TracePointsA = nullptr;
    }

    if (TracePointsB != nullptr)
    {
        delete [] TracePointsB;
        TracePointsB = nullptr;
    }
}

//
void MapComputeCells::ClearComputingCells()
{
    //// number of points on the sphere
    for (int i = -NLayers; i <= NLayers; i++)
    {
        for (int j = -NLayers; j <= NLayers; j++)
        {
            for (int k = 0; k <= NLayers; k++)
            {
                MapComputeCell * cell = &(this->operator ()(i, j, k));
                cell->Clear();
            }
        }
    }
}

//
void MapComputeCells::GenerateLinesCombined(int nGenPointsUF)
{
    ClearComputingCells();

    double sqrt33 = 1.0 / sqrt(3);

    ////
    dflighting::Random rng(111);
    for (int i = 0; i < nGenPointsUF; i++)
    {
        //double z = myrandom(&initialRandom);
        double z = rng.nextDouble() * (1.0 - sqrt33) + sqrt33;

        double t = 2 * 3.14159265358 * rng.nextDouble();
        double r = sqrt(1 - z * z);
        double x = r * cos(t);
        double y = r * sin(t);

        ////
        if (!(z >=0 &&
              -z <= x && x <= z &&
              -z <= y && y <= z))
            continue;

        ////
        double dx = 200 * NLayers * x;
        double dy = 200 * NLayers * y;
        double dz = 200 * NLayers * z;

        ////
        std::vector<MyIntPoint> vec;
        line3d(0, 0, 0, (int)dx, (int)dy, (int)dz, -NLayers, NLayers, -NLayers, NLayers, 0, NLayers, &vec);

        for (int k = 1; k < vec.size(); k++)
        {
            int ax = vec[k].X;
            int ay = vec[k].Y;
            int az = vec[k].Z;

            int px = vec[k - 1].X;
            int py = vec[k - 1].Y;
            int pz = vec[k - 1].Z;

            this->operator ()(px, py, pz).Directions[ax - px + 1][ay - py + 1] += 1;
            //ComputingCells[pz][px + NLayers][py + NLayers].Directions[ax - px + 1][ay - py + 1] += 1;
        }
    }

    ////
    //double minLayerValue = 1;
    //double maxLayerValue = 1;

    //std::vector<double> minValues;
    //std::vector<double> maxValues;

    //// update amountsDir
    for (int k = 0; k <= NLayers; k++)
    {
        //minValues.push_back(minLayerValue);
        //maxValues.push_back(maxLayerValue);

        for (int i = -k; i <= k; i++)
        {
            for (int j = -k; j <= k; j++)
            {
                MapComputeCell * cell = &(this->operator ()(i, j, k)); // &(ComputingCells[k][i + NLayers][j + NLayers]);

                int sum = 0;
                for (int ax = 0; ax < 3; ax++)
                    for (int ay = 0; ay < 3; ay++)
                        sum += cell->Directions[ax][ay];

                if (sum == 0)
                    continue;

                double inv = 1.0 / (double)sum;
                for (int ax = 0; ax < 3; ax++)
                    for (int ay = 0; ay < 3; ay++)
                        cell->Directions[ax][ay] *= inv;
            }
        }

        //// least possible value corresponds to i = k && j = k
        //MapComputeCell & cellMin = this->operator ()(k, k, k);
        //double minTransferValue = cellMin.Directions[2][2];

        //minLayerValue *= minTransferValue;

        ////
        //MapComputeCell & cellMax = this->operator ()(0, 0, k);
        //double maxTransferValue = cellMax.Directions[1][1];

        //maxLayerValue *= maxTransferValue;

        ////
        //cout << "1";
    }
}



//
void MapComputeCells::ClearITraced()
{
    for (int iCell = 0; iCell < LightedTilesNext; iCell++ )
    {
        LightedTiles[iCell]->LightValueITraced = 0;
        LightedTiles[iCell]->IsWithinLightedTilesTraced = false;
    }
}

//
void MapComputeCells::SaveToStream(std::ostream * stream)
{
    for (int k = 0; k <= NLayers; k++)
    {
        for (int i = -k; i <= k; i++)
        {
            for (int j = -k; j <= k; j++)
            {
                (this->operator ()(i, j, k)).SaveToStream(stream);
            }
        }
    }
}

//
void MapComputeCells::LoadFromStream(std::istream * stream)
{
    for (int k = 0; k <= NLayers; k++)
    {
        for (int i = -k; i <= k; i++)
        {
            for (int j = -k; j <= k; j++)
            {
                (this->operator ()(i, j, k)).LoadFromStream(stream);
            }
        }
    }
}



}
