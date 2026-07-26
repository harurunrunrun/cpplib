#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_SUPPORT_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_SUPPORT_POINT_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <stdexcept>

#include "adaptive_orient3d.hpp"
#include "convex_polyhedron3.hpp"
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
    if(direction.x == 0.0L && direction.y == 0.0L
        && direction.z == 0.0L)[[unlikely]]{
        throw std::invalid_argument("zero support direction");
    }
    for(const Point3& point: polyhedron.vertices){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("non-finite convex polyhedron vertex");
        }
    }

    using namespace geometry3d_adaptive_detail;
    const Point3& anchor = polyhedron.vertices.front();
    const std::array<ExactDyadic, 3> exact_anchor{
        exact_dyadic(anchor.x), exact_dyadic(anchor.y), exact_dyadic(anchor.z),
    };
    const std::array<ExactDyadic, 3> exact_direction{
        exact_dyadic(direction.x), exact_dyadic(direction.y),
        exact_dyadic(direction.z),
    };
    const auto objective = [&](const Point3& point){
        const std::array<long double, 3> coordinate{
            point.x, point.y, point.z,
        };
        ExactDyadic result{};
        for(std::size_t index = 0; index < 3; ++index){
            result = add(result, multiply(
                subtract(exact_dyadic(coordinate[index]), exact_anchor[index]),
                exact_direction[index]
            ));
        }
        return result;
    };

    std::size_t answer = 0;
    ExactDyadic best = objective(anchor);
    for(std::size_t index = 1; index < polyhedron.vertices.size(); ++index){
        ExactDyadic value = objective(polyhedron.vertices[index]);
        if(sign(subtract(value, best)) > 0){
            best = value;
            answer = index;
        }
    }
    return polyhedron.vertices[answer];
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_SUPPORT_POINT_HPP_INCLUDED
