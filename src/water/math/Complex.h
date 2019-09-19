#pragma once

#include <math.h>

// exp(ix) = cos(x) + isin(x)
// Если z=x+iy, то z* = x-iy
// (a1 + a2*i) * (b1 + b2*i) = (a1*b1 - a2*b2) + (a1*b2 + a2*b1)*i

struct Complex
{
    double Re;
    double Im;

    Complex() :
        Re(0.0),
        Im(0.0)
    {}

    Complex(double re, double im) :
        Re(re),
        Im(im)
    {}

    inline void operator += (const Complex& rhv)
    {
        Re += rhv.Re;
        Im += rhv.Im;
    }

    inline void operator /= (double s)
    {
        Re /= s;
        Im /= s;
    }

    inline Complex operator * (const Complex& rhv) const
    {
        return Complex(Re*rhv.Re - Im*rhv.Im, Re*rhv.Im + Im*rhv.Re);
    }

    inline Complex operator * (double s) const
    {
        return Complex(Re*s, Im*s);
    }

    inline Complex operator + (const Complex& rhv) const
    {
        return Complex(Re + rhv.Re, Im + rhv.Im);
    }

    inline Complex operator - (const Complex& rhv) const
    {
        return Complex(Re - rhv.Re, Im - rhv.Im);
    }

    inline Complex operator / (double s) const
    {
        return Complex(Re/s, Im/s);
    }

    inline static Complex exp(double x)
    {
        return Complex(cos(x), sin(x));
    }
};
