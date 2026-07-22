#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_CIRCLE_INTERSECTIONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_CIRCLE_INTERSECTIONS_HPP_INCLUDED

#include <stdexcept>
#include <vector>

#include "base.hpp"
#include "geometry3d_sign.hpp"
#include "is_finite.hpp"
#include "line_circle_intersections.hpp"

inline std::vector<Point3> ray_circle_intersections(
    const Ray3& ray,
    const Circle3& circle
){
    geometry3d_validate(circle);
    if(!geometry3d_is_finite(ray))[[unlikely]]{
        throw std::invalid_argument("ray-circle intersection requires a finite ray");
    }
    const geometry3d_detail::LineSphereIntersectionParameters parameters =
        geometry3d_detail::line_circle_intersection_parameters(
            {ray.origin, ray.through}, circle
        );
    std::vector<Point3> result;
    result.reserve(parameters.signed_distances.size());
    for(const long double signed_distance: parameters.signed_distances){
        const int sign = geometry3d_sign(signed_distance);
        if(sign < 0) continue;
        if(signed_distance < 0.0L){
            result.push_back(ray.origin);
        }else{
            result.push_back(
                geometry3d_detail::restore_line_sphere_intersection(
                    ray.origin, parameters, signed_distance
                )
            );
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_CIRCLE_INTERSECTIONS_HPP_INCLUDED
