#pragma once

#include "base.hpp"
#include "line3_direction.hpp"
#include "parallel.hpp"
#include "ray3_direction.hpp"
#include "segment3_direction.hpp"

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
