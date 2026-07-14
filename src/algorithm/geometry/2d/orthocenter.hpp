#pragma once

#include "circumcenter.hpp"

inline Point orthocenter(
    const Point& first,
    const Point& second,
    const Point& third
){
    const Point center = circumcenter(first, second, third);
    return first + (second - center) + (third - center);
}
