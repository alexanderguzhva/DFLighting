#include "mapcomputecells.h"

#include "random.h"

#include "line3dpainter.h"


namespace dflighting
{




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
        Line3DPainter::Draw(0, 0, 0, (int)dx, (int)dy, (int)dz, -NLayers, NLayers, -NLayers, NLayers, 0, NLayers, vec);

        for (size_t k = 1; k < vec.size(); k++)
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
