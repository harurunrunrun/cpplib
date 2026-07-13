#pragma once

#include "intersect.hpp"
#include "parallel.hpp"

inline bool same_line(const Line& first, const Line& second){
    return parallel(first, second) && intersect(first, second.a);
}
