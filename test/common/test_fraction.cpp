#include "gmock/gmock.h"
#include "common/fraction.h"
TEST(Fraction, trim)
{
    EXPECT_EQ(fraction(1, 2), fraction(2, 4));
    EXPECT_EQ(fraction(1, 1), fraction(4, 4));
    EXPECT_NE(fraction(1, 0), fraction(0, 1));
    EXPECT_EQ(fraction(0, 2), fraction(0, 55));
}

TEST(Fraction, negatives)
{
    EXPECT_EQ(fraction(-1, 2), fraction(1, -2));
    EXPECT_NE(fraction(-1, 2), fraction(1, 2));
    EXPECT_NE(fraction(1, -2), fraction(1, 2));
    EXPECT_NE(fraction(1, -2), fraction(-1, -2));
    EXPECT_EQ(fraction(0, 2), fraction(0, -2));
}

TEST(Fraction, cast_to_double)
{
    EXPECT_DOUBLE_EQ(0.5, fraction(1, 2));
    EXPECT_DOUBLE_EQ(0.1, fraction(1, 10));
    EXPECT_DOUBLE_EQ(-0.5, fraction(-1, 2));
}

TEST(Fraction, cast_from_double)
{
    EXPECT_EQ(d2fr(0.25), fraction(1, 4));
    EXPECT_EQ(d2fr(0.3), fraction(3, 10));
    EXPECT_DOUBLE_EQ(d2fr(0.33333333333333333333333333333333333), fraction(1, 3));
}

TEST(Fraction, inf)
{
    EXPECT_DOUBLE_EQ(HUGE_VAL, fraction(1, 0));
}
TEST(Fraction, small)
{
    EXPECT_DOUBLE_EQ(1.0 / (LLONG_MAX), fraction(1, LLONG_MAX));
}
TEST(Fraction, smalltrim)
{
    EXPECT_DOUBLE_EQ(2.0 / (LLONG_MAX), fraction(2, LLONG_MAX));
}
TEST(Fraction, large)
{
    EXPECT_DOUBLE_EQ(1.0 * LLONG_MAX, fraction(LLONG_MAX, 1));
}
TEST(Fraction, largetrim)
{
    EXPECT_DOUBLE_EQ(0.5 * LLONG_MAX, fraction(LLONG_MAX, 2));
}


TEST(Fraction, addbasic)
{
    fraction f1(1, 5);
    fraction f2(2, 5);
    EXPECT_EQ(f1 + f2, fraction(3, 5));
}

TEST(Fraction, addsimplify)
{
    fraction f1(1, 4);
    fraction f2(3, 4);
    EXPECT_EQ(f1 + f2, fraction(1, 1));
}

TEST(Fraction, addreduction)
{
    fraction f1(1, 4);
    fraction f2(3, 8);
    EXPECT_EQ(f1 + f2, fraction(5, 8));
}

TEST(Fraction, addapprox)
{
    fraction f1(3, 8);
    fraction f2(3, 8);
    EXPECT_EQ(f1 + f2, fraction(3, 4));
}


TEST(Fraction, minusbasic)
{
    fraction f1(3, 5);
    fraction f2(1, 5);
    EXPECT_EQ(f1 - f2, fraction(2, 5));
}

TEST(Fraction, minussimplify)
{
    fraction f1(3, 4);
    fraction f2(1, 4);
    EXPECT_EQ(f1 - f2, fraction(1, 2));
}

TEST(Fraction, minusreduction)
{
    fraction f1(3, 4);
    fraction f2(3, 8);
    EXPECT_EQ(f1 - f2, fraction(3, 8));
}

TEST(Fraction, minusapprox)
{
    fraction f1(3, 8);
    fraction f2(1, 8);
    EXPECT_EQ(f1 - f2, fraction(1, 4));
}


TEST(Fraction, mulbasic1)
{
    fraction f1(1, 5);
    fraction f2(2, 5);
    EXPECT_EQ(f1 * f2, fraction(2, 25));
}

TEST(Fraction, mulbasic2)
{
    fraction f1(15, 4);
    fraction f2(1, 3);
    EXPECT_EQ(f1 * f2, fraction(5, 4));
}

TEST(Fraction, mulbasic3)
{
    fraction f1(15, 4);
    fraction f2(1, 3);
    EXPECT_EQ(f1 * f2, fraction(5, 4));
}
