#ifndef MYCOLOR_H
#define MYCOLOR_H

#include "visibilityeps.h"

namespace dflighting
{

//
class MyColor
{
public:
    float R;
    float G;
    float B;

    MyColor()
    {
//        R = 0;
  //      G = 0;
    //    B = 0;
    }

    MyColor(float r, float g, float b) : R(r), G(g), B(b)
    {
    }

    MyColor & operator=(const MyColor &rhs)
    {
//        if (this != &rhs)
        {
            this->R = rhs.R;
            this->G = rhs.G;
            this->B = rhs.B;
        }

        return *this;
    }

    MyColor & operator=(const float &rhs)
    {
        this->R = rhs;
        this->G = rhs;
        this->B = rhs;

        return *this;
    }

    MyColor & operator +=(const MyColor &rhs)
    {
        this->R += rhs.R;
        this->G += rhs.G;
        this->B += rhs.B;

        return *this;
    }

    MyColor & operator -=(const MyColor &rhs)
    {
        this->R -= rhs.R;
        this->G -= rhs.G;
        this->B -= rhs.B;

        return *this;
    }

    MyColor & operator *=(const float &data)
    {
        this->R *= data;
        this->G *= data;
        this->B *= data;

        return *this;
    }


    const MyColor operator +(const MyColor &other) const
    {
        return MyColor(*this) += other;
    }

    const MyColor operator -(const MyColor &other) const
    {
        return MyColor(*this) -= other;
    }

    const MyColor operator *(const float &other) const
    {
        return MyColor(*this) *= other;
    }

    bool operator ==(const MyColor &other) const
    {
        return (other.R == this->R &&
                other.G == this->G &&
                other.B == this->B);
    }

    bool operator !=(const MyColor &other) const
    {
        return !(*this == other);
    }

    bool HighEnough() const
    {
        return (this->R >= visibilityEps || this->G >= visibilityEps || this->B >= visibilityEps);
    }

};

}

#endif // MYCOLOR_H
