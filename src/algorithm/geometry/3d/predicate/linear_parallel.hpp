#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINEAR_PARALLEL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINEAR_PARALLEL_HPP_INCLUDED

#include "../core/geometry_primitives.hpp"
#include "../point/line3_direction.hpp"
#include "parallel.hpp"
#include "../point/ray3_direction.hpp"
#include "../point/segment3_direction.hpp"

inline bool parallel(const Line3& first, const Line3& second){
    return parallel(line3_direction(first), line3_direction(second));
}

inline bool parallel(const Line3& first, const Ray3& second){
    return parallel(line3_direction(first), ray3_direction(second));
}

inline bool parallel(const Ray3& first, const Line3& second){
    return parallel(second, first);
}

inline bool parallel(const Line3& first, const Segment3& second){
    return parallel(line3_direction(first), segment3_direction(second));
}

inline bool parallel(const Segment3& first, const Line3& second){
    return parallel(second, first);
}

inline bool parallel(const Ray3& first, const Ray3& second){
    return parallel(ray3_direction(first), ray3_direction(second));
}

inline bool parallel(const Ray3& first, const Segment3& second){
    return parallel(ray3_direction(first), segment3_direction(second));
}

inline bool parallel(const Segment3& first, const Ray3& second){
    return parallel(second, first);
}

inline bool parallel(const Segment3& first, const Segment3& second){
    return parallel(segment3_direction(first), segment3_direction(second));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINEAR_PARALLEL_HPP_INCLUDED
