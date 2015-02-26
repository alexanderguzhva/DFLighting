#include "random.h"

namespace dflighting
{

//
random::random()
{
    randomSeed = 123;
}

//
random::random(const int seed) : randomSeed(seed)
{

}


//
int random::getRandomSeed() const
{
    return randomSeed;
}

//
void random::setRandomSeed(const int seed)
{
    randomSeed = seed;
}


//
double random::nextDouble()
{
    randomSeed = ((long long)0x08088405 * (long long)(randomSeed) + 1) & 0x7fffffff;
    return (((double)randomSeed) / 0x7fffffff);
}

//
int random::next(const int max)
{
    randomSeed = ((long long)0x08088405 * (long long)(randomSeed) + 1) & 0x7fffffff;

    return randomSeed % max;
}



}


