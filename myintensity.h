#ifndef MYINTENSITY
#define MYINTENSITY

#include <iostream>

#include "visibilityeps.h"

namespace dflighting
{

class MyIntensity
{
public:
    float I;

    MyIntensity()
    {
        //I = 0;
    }

    MyIntensity(float i) : I(i)
    {
    }

    MyIntensity & operator=(const float &rhs)
    {
        this->I = rhs;
        return *this;
    }

    MyIntensity & operator=(const MyIntensity &rhs)
    {
//        if (this != &rhs)
  //      {
            this->I = rhs.I;
    //    }

        return *this;
    }


    MyIntensity & operator +=(const MyIntensity &rhs)
    {
        this->I += rhs.I;

        return *this;
    }

    MyIntensity & operator -=(const MyIntensity &rhs)
    {
        this->I -= rhs.I;

        return *this;
    }

    MyIntensity & operator *=(const float &data)
    {
        this->I *= data;

        return *this;
    }

    const MyIntensity operator +(const MyIntensity &other) const
    {
        return MyIntensity(*this) += other;
    }

    const MyIntensity operator +(const float &other) const
    {
        return MyIntensity(*this) *= other;
    }

    const MyIntensity operator -(const MyIntensity &other) const
    {
        return MyIntensity(*this) -= other;
    }

    const MyIntensity operator *(const float &other) const
    {
        return MyIntensity(*this) *= other;
    }


    bool operator ==(const MyIntensity &other) const
    {
        return (other.I == this->I);
    }

    bool operator !=(const MyIntensity &other) const
    {
        return (!(*this == other));
    }

    bool HighEnough() const
    {
        return (this->I >= visibilityEps);
    }

    //
    void SaveToStream(std::ostream * stream)
    {
        stream->write(reinterpret_cast<char*>(&I), sizeof(float));
    }
};


}

#endif // MYINTENSITY

