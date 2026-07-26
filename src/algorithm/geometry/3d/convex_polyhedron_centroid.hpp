#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_CENTROID_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_CENTROID_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "convex_polyhedron3.hpp"
#include "cross.hpp"
#include "is_finite.hpp"
#include "scalar_triple.hpp"

inline Point3 convex_polyhedron_centroid(const ConvexPolyhedron3& polyhedron){
    if(polyhedron.vertices.empty())[[unlikely]]{
        throw std::invalid_argument("centroid of empty convex polyhedron");
    }
    long double scale = 0.0L;
    for(const Point3& point: polyhedron.vertices){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("non-finite convex polyhedron vertex");
        }
        scale = std::max({
            scale, std::abs(point.x), std::abs(point.y), std::abs(point.z)
        });
    }
    if(scale == 0.0L) return polyhedron.vertices.front();
    std::vector<Point3> vertices;
    vertices.reserve(polyhedron.vertices.size());
    for(const Point3& point: polyhedron.vertices) vertices.push_back(point / scale);

    Point3 result{};
    if(polyhedron.affine_dimension <= 0){
        result = vertices.front();
    }else if(polyhedron.affine_dimension == 1){
        result = (vertices.front() + vertices.back()) / 2.0L;
    }else if(polyhedron.affine_dimension == 2){
        long double total = 0.0L;
        for(const auto& face: polyhedron.faces){
            for(std::size_t vertex: face){
                if(vertex >= vertices.size())[[unlikely]]{
                    throw std::out_of_range("convex polyhedron face index");
                }
            }
            const Point3 product = cross(
                vertices[face[1]] - vertices[face[0]],
                vertices[face[2]] - vertices[face[0]]
            );
            const long double weight = std::hypot(
                product.x, product.y, product.z
            );
            result += (
                vertices[face[0]] + vertices[face[1]] + vertices[face[2]]
            ) * (weight / 3.0L);
            total += weight;
        }
        if(total == 0.0L)[[unlikely]]{
            throw std::domain_error("zero-area convex polygon centroid");
        }
        result /= total;
    }else{
        const Point3 reference = vertices.front();
        long double total = 0.0L;
        for(const auto& face: polyhedron.faces){
            for(std::size_t vertex: face){
                if(vertex >= vertices.size())[[unlikely]]{
                    throw std::out_of_range("convex polyhedron face index");
                }
            }
            const long double weight = scalar_triple(
                vertices[face[0]] - reference,
                vertices[face[1]] - reference,
                vertices[face[2]] - reference
            );
            result += (
                reference + vertices[face[0]] + vertices[face[1]]
                    + vertices[face[2]]
            ) * (weight / 4.0L);
            total += weight;
        }
        if(total == 0.0L)[[unlikely]]{
            throw std::domain_error("zero-volume convex polyhedron centroid");
        }
        result /= total;
    }
    result *= scale;
    if(!geometry3d_is_finite(result))[[unlikely]]{
        throw std::overflow_error("convex polyhedron centroid overflow");
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_CENTROID_HPP_INCLUDED
