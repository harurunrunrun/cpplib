#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINEAR_OVERLAP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINEAR_OVERLAP_HPP_INCLUDED

#include <variant>

#include "base.hpp"
#include "line_line_intersection.hpp"
#include "line_ray_intersection.hpp"
#include "line_segment_intersection.hpp"
#include "ray_ray_intersection.hpp"
#include "ray_segment_intersection.hpp"
#include "segment_segment_intersection.hpp"

inline bool overlap(const Line3& first, const Line3& second){
    return std::holds_alternative<Line3>(line_line_intersection(first, second));
}

inline bool overlap(const Line3& first, const Ray3& second){
    return std::holds_alternative<Ray3>(line_ray_intersection(first, second));
}

inline bool overlap(const Ray3& first, const Line3& second){
    return overlap(second, first);
}

inline bool overlap(const Line3& first, const Segment3& second){
    return std::holds_alternative<Segment3>(
        line_segment_intersection(first, second)
    );
}

inline bool overlap(const Segment3& first, const Line3& second){
    return overlap(second, first);
}

inline bool overlap(const Ray3& first, const Ray3& second){
    const LinearIntersection3 intersection = ray_ray_intersection(first, second);
    return std::holds_alternative<Ray3>(intersection) ||
        std::holds_alternative<Segment3>(intersection);
}

inline bool overlap(const Ray3& first, const Segment3& second){
    return std::holds_alternative<Segment3>(
        ray_segment_intersection(first, second)
    );
}

inline bool overlap(const Segment3& first, const Ray3& second){
    return overlap(second, first);
}

inline bool overlap(const Segment3& first, const Segment3& second){
    return std::holds_alternative<Segment3>(
        segment_segment_intersection(first, second)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINEAR_OVERLAP_HPP_INCLUDED
