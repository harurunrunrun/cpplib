#pragma once

#include "advanced/detail.hpp"
#include "circumcenter.hpp"

inline Circle circumcircle(
    const Point& first,
    const Point& second,
    const Point& third
){
    const Point center = circumcenter(first, second, third);
    return {center, advanced_geometry_detail::length(center - first)};
}
