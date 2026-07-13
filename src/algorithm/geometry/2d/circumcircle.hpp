#pragma once

#include "circumcenter.hpp"
#include "distance.hpp"

inline Circle circumcircle(
    const Point& first,
    const Point& second,
    const Point& third
){
    const Point center = circumcenter(first, second, third);
    return {center, distance(center, first)};
}
