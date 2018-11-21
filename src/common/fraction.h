#pragma once

class fraction
{
public:
    long _numerator, _denominator;

public:
    fraction();
    fraction(long numerator, long denominator);
    ~fraction();

public:
    fraction  operator+  (const fraction& rhs) const;
    fraction& operator+= (const fraction& rhs);
    fraction  operator-  (const fraction& rhs) const;
    fraction& operator-= (const fraction& rhs);
    fraction  operator*  (const fraction& rhs) const;
    fraction& operator*= (const fraction& rhs);
    fraction& operator=  (const double rhs);
    operator double() const;
};

fraction d2fr(double d);