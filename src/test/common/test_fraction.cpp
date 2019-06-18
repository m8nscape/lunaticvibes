#include <gtest/gtest.h>

#include "common/fraction.h"
TEST(Common_Fraction, trim)
{
    EXPECT_EQ(fraction(1, 2), fraction(2, 4));
    EXPECT_EQ(fraction(1, 1), fraction(4, 4));
    EXPECT_NE(fraction(1, 0), fraction(0, 1));
    EXPECT_EQ(fraction(0, 2), fraction(0, 55));
}

TEST(Common_Fraction, negatives)
{
    EXPECT_EQ(fraction(-1, 2), fraction(1, -2));
    EXPECT_NE(fraction(0, 2), fraction(0, -2));
}

TEST(Common_Fraction, cast_to_double)
{
    EXPECT_DOUBLE_EQ(0.5, fraction(1, 2));
    EXPECT_DOUBLE_EQ(0.1, fraction(1, 10));
    EXPECT_DOUBLE_EQ(-0.5, fraction(-1, 2));
}

TEST(Common_Fraction, cast_from_double)
{
    EXPECT_EQ(d2fr(0.25), fraction(1, 4));
    EXPECT_EQ(d2fr(0.3), fraction(3, 10));
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
    EXPECT_EQ(f1 - f2, fraction(1, 4));
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
