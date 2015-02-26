#include "random.h"

namespace dflighting
{

//
Random::Random()
{
    randomSeed = 123;
}

//
Random::Random(const int seed) : randomSeed(seed)
{

}


//
int Random::getRandomSeed() const
{
    return randomSeed;
}

//
void Random::setRandomSeed(const int seed)
{
    randomSeed = seed;
}


//
double Random::nextDouble()
{
    randomSeed = ((long long)0x08088405 * (long long)(randomSeed) + 1) & 0x7fffffff;
    return (((double)randomSeed) / 0x7fffffff);
}

//
int Random::next(const int max)
{
    randomSeed = ((long long)0x08088405 * (long long)(randomSeed) + 1) & 0x7fffffff;

    return randomSeed % max;
}



}


