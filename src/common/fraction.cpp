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
    int g = std::lcm(_denominator, rhs._denominator);
    return fraction(_numerator * (g / _denominator) + rhs._numerator * (g / rhs._denominator), g);
}

fraction& fraction::operator+= (const fraction& rhs)
{
    int g = std::lcm(_denominator, rhs._denominator);
    _numerator = _numerator * (g / _denominator) + rhs._numerator * (g / rhs._denominator);
    _denominator = g;
    return *this;
}

fraction fraction::operator- (const fraction& rhs) const
{
    int g = std::lcm(_denominator, rhs._denominator);
    return fraction(_numerator * (g / _denominator) - rhs._numerator * (g / rhs._denominator), g);
}

fraction& fraction::operator-= (const fraction& rhs)
{
    int g = std::lcm(_denominator, rhs._denominator);
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
    while (abs(nr) & 0xffffffff00000000 || abs(dr) & 0xffffffff00000000)
    {
        nr >>= 1;
        dr >>= 1;
    }
    return fraction(nr, dr);
}

#include <gtest/gtest.h>
TEST(Common_Fraction, trim1)
{
    EXPECT_EQ(fraction(1, 2), fraction(2, 4));
}
TEST(Common_Fraction, trim2)
{
    EXPECT_EQ(fraction(1, 1), fraction(4, 4));
}
TEST(Common_Fraction, trimzero)
{
    EXPECT_NE(fraction(1, 0), fraction(0, 1));
    EXPECT_EQ(fraction(0, 2), fraction(0, 55));
}
TEST(Common_Fraction, propnegative)
{
    EXPECT_EQ(fraction(-1, 2), fraction(1, -2));
}

TEST(Common_Fraction, fr2d1)
{
    EXPECT_DOUBLE_EQ(0.5, fraction(1, 2));
}
TEST(Common_Fraction, fr2d2)
{
    EXPECT_DOUBLE_EQ(0.1, fraction(1, 10));
}
TEST(Common_Fraction, fr2d3)
{
    EXPECT_DOUBLE_EQ(-0.5, fraction(-1, 2));
}

TEST(Common_Fraction, d2fr1)
{
    EXPECT_EQ(d2fr(0.25), fraction(1, 4));
}
TEST(Common_Fraction, d2fr2)
{
    EXPECT_EQ(d2fr(0.3), fraction(3, 10));
}
TEST(Common_Fraction, d2fr3)
{
    EXPECT_DOUBLE_EQ(d2fr(0.33333333333333333333333333333333333), fraction(1, 3));
}

TEST(Common_Fraction, inf)
{
    EXPECT_DOUBLE_EQ(HUGE_VAL, fraction(1, 0));
}
TEST(Common_Fraction, small)
{
    EXPECT_DOUBLE_EQ(1.0 / (LLONG_MAX), fraction(1, LLONG_MAX));
}
TEST(Common_Fraction, smalltrim)
{
    EXPECT_DOUBLE_EQ(2.0 / (LLONG_MAX), fraction(2, LLONG_MAX));
}
TEST(Common_Fraction, large)
{
    EXPECT_DOUBLE_EQ(1.0 * LLONG_MAX, fraction(LLONG_MAX, 1));
}
TEST(Common_Fraction, largetrim)
{
    EXPECT_DOUBLE_EQ(0.5 * LLONG_MAX, fraction(LLONG_MAX, 2));
}


TEST(Common_Fraction, addbasic)
{
    fraction f1(1, 5);
    fraction f2(2, 5);
    EXPECT_EQ(f1 + f2, fraction(3, 5));
}

TEST(Common_Fraction, addsimplify)
{
    fraction f1(1, 4);
    fraction f2(3, 4);
    EXPECT_EQ(f1 + f2, fraction(1, 1));
}

TEST(Common_Fraction, addreduction)
{
    fraction f1(1, 4);
    fraction f2(3, 8);
    EXPECT_EQ(f1 + f2, fraction(5, 8));
}

TEST(Common_Fraction, addapprox)
{
    fraction f1(3, 8);
    fraction f2(3, 8);
    EXPECT_EQ(f1 + f2, fraction(3, 4));
}


TEST(Common_Fraction, minusbasic)
{
    fraction f1(3, 5);
    fraction f2(1, 5);
    EXPECT_EQ(f1 - f2, fraction(2, 5));
}

TEST(Common_Fraction, minussimplify)
{
    fraction f1(3, 4);
    fraction f2(1, 4);
    EXPECT_EQ(f1 - f2, fraction(1, 2));
}

TEST(Common_Fraction, minusreduction)
{
    fraction f1(3, 4);
    fraction f2(3, 8);
    EXPECT_EQ(f1 - f2, fraction(3, 8));
}

TEST(Common_Fraction, minusapprox)
{
    fraction f1(3, 8);
    fraction f2(1, 8);
    EXPECT_EQ(f1 - f2, fraction(3, 4));
}


TEST(Common_Fraction, mulbasic1)
{
    fraction f1(1, 5);
    fraction f2(2, 5);
    EXPECT_EQ(f1 * f2, fraction(2, 25));
}

TEST(Common_Fraction, mulbasic2)
{
    fraction f1(15, 4);
    fraction f2(1, 3);
    EXPECT_EQ(f1 * f2, fraction(5, 4));
}

TEST(Common_Fraction, mulbasic3)
{
    fraction f1(15, 4);
    fraction f2(1, 3);
    EXPECT_EQ(f1 * f2, fraction(5, 4));
}
