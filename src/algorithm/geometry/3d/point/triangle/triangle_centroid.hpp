#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TRIANGLE_TRIANGLE_CENTROID_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TRIANGLE_TRIANGLE_CENTROID_HPP_INCLUDED

#include "../../detail/point/centroid_detail.hpp"
#include "../../type/definition/point3.hpp"
#include "../../type/definition/triangle3.hpp"

inline Point3 centroid(const Triangle3& triangle){
    return geometry3d_centroid_detail::centroid<3>({
        triangle.a, triangle.b, triangle.c,
    });
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TRIANGLE_TRIANGLE_CENTROID_HPP_INCLUDED
