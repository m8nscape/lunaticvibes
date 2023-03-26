#include "fraction.h"
#include <utility>
#include <numeric>
#pragma warning(disable: 4244)

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

    long long g = std::gcd(_numerator, _denominator);
    if (g == 1)
        return false;

    _numerator /= g;
    _denominator /= g;
    return true;
}

fraction::fraction(): _numerator(0), _denominator(1) {}
fraction::fraction(long long numerator, long long denominator, bool t) : _numerator(numerator), _denominator(denominator)
{
    if (denominator == 0)
    {
        numerator = 0;
        denominator = 1;
        return;
    }

    if (t)
        trim(_numerator, _denominator);
}

fraction::~fraction() {}

fraction fraction::operator+ (const fraction& rhs) const
{
    long long g = std::lcm(_denominator, rhs._denominator);
    return fraction(_numerator * (g / _denominator) + rhs._numerator * (g / rhs._denominator), g);
}

fraction& fraction::operator+= (const fraction& rhs)
{
    long long g = std::lcm(_denominator, rhs._denominator);
    _numerator = _numerator * (g / _denominator) + rhs._numerator * (g / rhs._denominator);
    _denominator = g;
    return *this;
}

fraction fraction::operator- (const fraction& rhs) const
{
    long long g = std::lcm(_denominator, rhs._denominator);
    return fraction(_numerator * (g / _denominator) - rhs._numerator * (g / rhs._denominator), g);
}

fraction& fraction::operator-= (const fraction& rhs)
{
    long long g = std::lcm(_denominator, rhs._denominator);
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

    long long dr = 1e15;
    trim(nr, dr);
    while (std::abs(nr) & 0xffffffff00000000ll ||
           std::abs(dr) & 0xffffffff00000000ll)
    {
        nr >>= 1;
        dr >>= 1;
    }
    return fraction(nr, dr);
}
