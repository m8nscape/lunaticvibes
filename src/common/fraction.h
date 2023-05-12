#pragma once

namespace lunaticvibes
{

class fraction
{
public:
    long long _numerator, _denominator;

public:
    fraction();
    fraction(long long numerator, long long denominator, bool trim = true);
    virtual ~fraction();

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

}
