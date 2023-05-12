#pragma once

namespace lunaticvibes
{

class NumberTransformBase
{
public:
    using ValueType = long long;

protected:
    decltype(std::declval<std::chrono::milliseconds>().count()) threshold_ms;
    ValueType value_real;
    ValueType value_prev;
    mutable ValueType value_display;
    std::chrono::system_clock::time_point modify_time_start;
    std::function<double(double, double)> progress_cb;

protected:
    using ProgressCallback = double(double time_passed, double threshold);
    static double ProgressLinear(double t, double d) { double v = t / d; return v; }

public:
    NumberTransformBase() = delete;
    NumberTransformBase(unsigned threshold_ms, ProgressCallback progress_cb = ProgressLinear) :
        threshold_ms(threshold_ms), value_real(0), value_display(0), progress_cb(progress_cb) {}
    NumberTransformBase(const NumberTransformBase& r) = delete;
    NumberTransformBase(NumberTransformBase&& r) = delete;
    virtual ~NumberTransformBase() {}

protected:

    void update() const { return update(std::chrono::system_clock::now()); }
    void update(time_t now) const { return update(std::chrono::system_clock::from_time_t(now)); }
    void update(std::chrono::system_clock::time_point now) const
    {
        auto time_passed = std::chrono::duration_cast<std::chrono::milliseconds>(now - modify_time_start);

        // Time is prior than modify time, do not update
        if (time_passed.count() < 0) return;

        // Time exceeds threshold, set display value to real number
        if (time_passed.count() >= threshold_ms)
        {
            value_display = value_real;
            return;
        }

        double progress = progress_cb(static_cast<double>(time_passed.count()), static_cast<double>(threshold_ms));
        value_display = value_prev + static_cast<ValueType>(std::round((value_real - value_prev) * progress));
    }

public:
    ValueType get() const { update(); return value_display; }
    operator ValueType() const { return get(); }

    void set(ValueType value)
    {
        value_prev = value_display;
        value_real = value;
        modify_time_start = std::chrono::system_clock::now();
    }
    NumberTransformBase& operator=(ValueType rhs) { set(rhs); return *this; }
};

//===========================================================================//
// Useful aliases

using NumberTransformLinear = NumberTransformBase;

class NumberTransformAccel : public NumberTransformBase
{
public:
    static double ProgressAccel(double t, double d) { double v = t / d; return v * v; }
    NumberTransformAccel(unsigned threshold_ms) : NumberTransformBase(threshold_ms, ProgressAccel) {}
    NumberTransformAccel& operator=(ValueType rhs) { set(rhs); return *this; }
};

class NumberTransformDecel : public NumberTransformBase
{
public:
    static double ProgressDecel(double t, double d) { double v = t / d; return std::sqrt(v); }
    NumberTransformDecel(unsigned threshold_ms) : NumberTransformBase(threshold_ms, ProgressDecel) {}
    NumberTransformDecel& operator=(ValueType rhs) { set(rhs); return *this; }
};

}
