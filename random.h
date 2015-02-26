#ifndef RANDOM_H
#define RANDOM_H


namespace dflighting
{


class Random
{
public:
    Random();
    Random(const int seed);

    int getRandomSeed() const;
    void setRandomSeed(const int seed);

    double nextDouble();
    int next(const int max);

private:
    int randomSeed;
};


}

#endif // RANDOM_H
