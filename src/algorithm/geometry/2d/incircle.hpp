#pragma once

#include "distance.hpp"
#include "incenter.hpp"

inline Circle incircle(
    const Point& first,
    const Point& second,
    const Point& third
){
    const Point center = incenter(first, second, third);
    return {center, distance(Line{first, second}, center)};
}
