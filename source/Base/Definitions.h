#pragma once

#include <array>
#include <map>
#include <set>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <optional>
#include <memory>

#include "Exceptions.h"

using std::int32_t;
using std::int64_t;
using std::uint32_t;
using std::uint64_t;

const double FLOATINGPOINT_HIGH_PRECISION = 1.0e-7;
const double FLOATINGPOINT_MEDIUM_PRECISION = 1.0e-4;
const double FLOATINGPOINT_LOW_PRECISION = 1.0e-1;

template <typename T>
inline float toFloat(T const& value)
{
    return static_cast<float>(value);
}

template<typename T>
inline int toInt(T const& value)
{
    return static_cast<int>(value);
}

#define THROW_NOT_IMPLEMENTED() throw std::runtime_error("not implemented")

#define CHECK(expression) \
    if (!(expression)) { \
        throw BugReportException("check failed"); \
    }

#define MEMBER_DECLARATION(className, type, name, initialValue) \
    type _##name = initialValue; \
    className& name(type const& name) \
    { \
        _##name = name; \
        return *this; \
    }

struct IntVector2D
{
    int x = 0;
    int y = 0;

    IntVector2D() = default;
    IntVector2D(std::initializer_list<int> l);
    bool operator==(IntVector2D const& vec) const;
    void operator-=(IntVector2D const& vec);
};

struct RealVector2D
{
    float x = 0.0f;
    float y = 0.0f;

    RealVector2D() = default;
    RealVector2D(float x_, float y_);
    RealVector2D(std::initializer_list<float> l);
    bool operator==(RealVector2D const& vec) const;
    bool operator!=(RealVector2D const& vec) const { return !operator==(vec); }
    void operator+=(RealVector2D const& vec);
    void operator-=(RealVector2D const& vec);
    template <typename T>
    void operator*=(T divisor)
    {
        x *= divisor;
        y *= divisor;
    }
    template <typename T>
    void operator/=(T divisor)
    {
        x /= divisor;
        y /= divisor;
    }
    template <typename T>
    RealVector2D operator*(T factor) const
    {
        return RealVector2D{x * factor, y * factor};
    }
    RealVector2D operator+(RealVector2D const& other) const;
    RealVector2D operator-(RealVector2D const& other) const;
    RealVector2D operator/(float divisor) const;
};

using RealMatrix2D = std::array<std::array<float, 2>, 2>;

struct RealRect
{
    RealVector2D topLeft;
    RealVector2D bottomRight;
};