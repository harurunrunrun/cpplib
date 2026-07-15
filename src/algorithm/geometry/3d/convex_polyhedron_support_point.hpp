#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "convex_polyhedron3.hpp"
#include "dot.hpp"
#include "is_finite.hpp"

inline Point3 convex_polyhedron_support_point(
    const ConvexPolyhedron3& polyhedron,
    const Point3& direction
){
    if(polyhedron.vertices.empty())[[unlikely]]{
        throw std::invalid_argument("support point of empty convex polyhedron");
    }
    if(!geometry3d_is_finite(direction))[[unlikely]]{
        throw std::invalid_argument("non-finite support direction");
    }
    const long double direction_scale = std::max({
        std::abs(direction.x), std::abs(direction.y), std::abs(direction.z)
    });
    if(direction_scale == 0.0L)[[unlikely]]{
        throw std::invalid_argument("zero support direction");
    }
    const Point3 unit_scale_direction = direction / direction_scale;
    long double coordinate_scale = 0.0L;
    for(const Point3& point: polyhedron.vertices){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("non-finite convex polyhedron vertex");
        }
        coordinate_scale = std::max({
            coordinate_scale, std::abs(point.x), std::abs(point.y),
            std::abs(point.z)
        });
    }
    if(coordinate_scale == 0.0L) return polyhedron.vertices.front();
    std::size_t answer = 0;
    long double best = dot(
        polyhedron.vertices.front() / coordinate_scale, unit_scale_direction
    );
    for(std::size_t index = 1; index < polyhedron.vertices.size(); ++index){
        const long double value = dot(
            polyhedron.vertices[index] / coordinate_scale, unit_scale_direction
        );
        if(value > best){
            best = value;
            answer = index;
        }
    }
    return polyhedron.vertices[answer];
}
