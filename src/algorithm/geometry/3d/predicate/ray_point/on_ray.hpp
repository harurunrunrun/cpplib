#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_RAY_POINT_ON_RAY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_RAY_POINT_ON_RAY_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../scalar/point_point/dot.hpp"
#include "../../integer/scalar/geometry3d_sign.hpp"
#include "../point_point/parallel.hpp"
#include "../../point/ray/ray3_direction.hpp"

inline bool on_ray(const Ray3& ray, const Point3& point){
    const Point3 direction = ray3_direction(ray);
    return parallel(direction, point - ray.origin) &&
        geometry3d_sign(dot(direction, point - ray.origin)) >= 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_RAY_POINT_ON_RAY_HPP_INCLUDED
