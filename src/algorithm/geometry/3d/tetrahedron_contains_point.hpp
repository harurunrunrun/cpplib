#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TETRAHEDRON_CONTAINS_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TETRAHEDRON_CONTAINS_POINT_HPP_INCLUDED

#include <algorithm>

#include "geometry3d_sign.hpp"
#include "point3.hpp"
#include "tetrahedron3.hpp"
#include "tetrahedron_point_barycentric_coordinates.hpp"

inline bool contains(const Tetrahedron3& tetrahedron, const Point3& point){
    const auto weights = barycentric_coordinates(tetrahedron, point);
    return std::all_of(weights.begin(), weights.end(), [](long double weight){
        return geometry3d_sign(weight) >= 0;
    });
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TETRAHEDRON_CONTAINS_POINT_HPP_INCLUDED
