#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ON_RAY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ON_RAY_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../core/geometry_primitives.hpp"
#include "../scalar/dot.hpp"
#include "geometry3d_sign.hpp"
#include "parallel.hpp"
#include "../point/ray3_direction.hpp"

inline bool on_ray(const Ray3& ray, const Point3& point){
    const Point3 direction = ray3_direction(ray);
    return parallel(direction, point - ray.origin) &&
        geometry3d_sign(dot(direction, point - ray.origin)) >= 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ON_RAY_HPP_INCLUDED
