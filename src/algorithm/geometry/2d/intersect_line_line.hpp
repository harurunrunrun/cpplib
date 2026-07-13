#pragma once

#include "same_line.hpp"

inline bool intersect_line_line(const Line& first, const Line& second){
    return !parallel(first, second) || same_line(first, second);
}
