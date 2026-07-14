#pragma once

#include "circumcenter.hpp"

inline Point orthocenter(
    const Point& first,
    const Point& second,
    const Point& third
){
    return first + second + third -
        circumcenter(first, second, third) * 2.0L;
}
