#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_COLLECTION_CIRCLE_SPHERE_INTERSECTIONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_COLLECTION_CIRCLE_SPHERE_INTERSECTIONS_HPP_INCLUDED

#include <stdexcept>
#include <vector>

#include "../core/geometry_primitives.hpp"
#include "../predicate/is_finite.hpp"
#include "circle_circle_intersections.hpp"
#include "../point/plane3_unit_normal.hpp"
#include "../shape/plane_sphere_intersection.hpp"

inline std::vector<Point3> circle_sphere_intersections(
    const Circle3& circle,
    const Sphere3& sphere
){
    geometry3d_validate(circle);
    geometry3d_validate(sphere);
    (void)plane3_unit_normal({circle.center, circle.normal});
    const auto section = plane_sphere_intersection(
        Plane3{circle.center, circle.normal}, sphere
    );
    if(!section) return {};
    return circle_circle_intersections(circle, *section);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_COLLECTION_CIRCLE_SPHERE_INTERSECTIONS_HPP_INCLUDED
