#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "line_sphere_intersections.hpp"
#include "on_ray.hpp"

inline std::vector<Point3> ray_sphere_intersections(
    const Ray3& ray,
    const Sphere3& sphere
){
    std::vector<Point3> result;
    for(const Point3& point: line_sphere_intersections(
        {ray.origin, ray.through}, sphere
    )){
        if(on_ray(ray, point)) result.push_back(point);
    }
    return result;
}
