#pragma once

#include "cross_point.hpp"

inline Point line_intersection(const Line& first, const Line& second){
    return cross_point(first, second);
}
