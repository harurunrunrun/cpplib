#pragma once

#include <vector>

#include "base.hpp"
#include "line_circle_intersections.hpp"
#include "on_ray.hpp"

inline std::vector<Point3> ray_circle_intersections(
    const Ray3& ray,
    const Circle3& circle
){
    std::vector<Point3> result;
    for(const Point3& point: line_circle_intersections(
        Line3{ray.origin, ray.through}, circle
    )){
        if(on_ray(ray, point)) result.push_back(point);
    }
    return result;
}
