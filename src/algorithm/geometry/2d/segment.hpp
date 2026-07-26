#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SEGMENT_HPP_INCLUDED

#include "line.hpp"

struct Segment{
    Point a;
    Point b;
};

inline Line supporting_line(const Segment& segment){
    return {segment.a, segment.b};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SEGMENT_HPP_INCLUDED
