#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "abs.hpp"
#include "closest_point.hpp"
#include "closest_points.hpp"
#include "projection.hpp"
#include "signed_distance.hpp"

inline long double distance(const Point3& left, const Point3& right){
    return abs(left - right);
}

inline long double distance(const Line3& line, const Point3& point){
    return abs(point - projection(line, point));
}

inline long double distance(const Ray3& ray, const Point3& point){
    return abs(point - closest_point(ray, point));
}

inline long double distance(const Segment3& segment, const Point3& point){
    return abs(point - closest_point(segment, point));
}

inline long double distance(const Plane3& plane, const Point3& point){
    return std::abs(signed_distance(plane, point));
}

inline long double distance(const Line3& first, const Line3& second){
    const auto [left, right] = closest_points(first, second);
    return abs(left - right);
}

inline long double distance(const Segment3& first, const Segment3& second){
    const auto [left, right] = closest_points(first, second);
    return abs(left - right);
}

inline long double distance(const Triangle3& triangle, const Point3& point){
    return abs(point - closest_point(triangle, point));
}
