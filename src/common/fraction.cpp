#include "fraction.h"
#include <utility>

int gcd(long long a, long long b, size_t max_count = 5)
{
    if (max_count == 0)
        return 1;
    
    if (a < b)
        std::swap(a, b);
    if (b == 0)
        return 1;
    else
    {
        if (a % b == 0)
            return b;
        else
            return gcd(b, a % b, --max_count);
    }
}

int lcm(long long a, long long b)
{
    return (a * b < 0 ? -a * b : a * b) / gcd(a, b);
}

bool trim(long &_numerator, long &_denominator)
{
    if (_numerator == 0)
    {
        _denominator = 1;
        return true;
    }

    if ((_numerator < 0 && _denominator < 0) || (_numerator >= 0 && _denominator < 0))
    {
        _numerator = -_numerator;
        _denominator = -_denominator;
    }

    int g = gcd(_numerator, _denominator);
    if (g == 1)
        return false;

    _numerator /= g;
    _denominator /= g;
    return true;
}

bool trim(long long &_numerator, long long &_denominator)
{
    if (_numerator == 0)
    {
        _denominator = 1;
        return true;
    }

    if ((_numerator < 0 && _denominator < 0) || (_numerator >= 0 && _denominator < 0))
    {
        _numerator = -_numerator;
        _denominator = -_denominator;
    }

    int g = gcd(_numerator, _denominator);
    if (g == 1)
        return false;

    _numerator /= g;
    _denominator /= g;
    return true;
}

fraction::fraction(): _numerator(0), _denominator(1) {}
fraction::fraction(long numerator, long denominator, bool t) : _numerator(numerator), _denominator(denominator)
{
    if (t)
        trim(_numerator, _denominator);
}

fraction::~fraction() {}

fraction fraction::operator+ (const fraction& rhs) const
{
    int g = lcm(_denominator, rhs._denominator);
    return fraction(_numerator * (g / _denominator) + rhs._numerator * (g / rhs._denominator), g);
}

fraction& fraction::operator+= (const fraction& rhs)
{
    int g = lcm(_denominator, rhs._denominator);
    _numerator = _numerator * (g / _denominator) + rhs._numerator * (g / rhs._denominator);
    _denominator = g;
    return *this;
}

fraction fraction::operator- (const fraction& rhs) const
{
    int g = lcm(_denominator, rhs._denominator);
    return fraction(_numerator * (g / _denominator) - rhs._numerator * (g / rhs._denominator), g);
}

fraction& fraction::operator-= (const fraction& rhs)
{
    int g = lcm(_denominator, rhs._denominator);
    _numerator = _numerator * (g / _denominator) - rhs._numerator * (g / rhs._denominator);
    _denominator = g;
    return *this;
}

fraction fraction::operator* (const fraction& rhs) const
{
    return fraction(_numerator * rhs._numerator, _denominator * rhs._denominator);
}

fraction& fraction::operator*= (const fraction& rhs)
{
    _numerator *= rhs._numerator;
    _denominator *= rhs._denominator;
    return *this;
}

fraction& fraction::operator=(const double d)
{
    *this = d2fr(d);
    return *this;
}

fraction::operator double() const { return static_cast<double>(_numerator) / _denominator; }

fraction d2fr(double d)
{
    long long nr = d * 1e15;
    if (nr == 0)
    {
        return fraction(0, 1);
    }

    long long dr = 1e16;
    trim(nr, dr);
    while (abs(nr) & 0xffffffff00000000 || abs(dr) & 0xffffffff00000000)
    {
        nr >>= 1;
        dr >>= 1;
    }
    return fraction(nr, dr);
}