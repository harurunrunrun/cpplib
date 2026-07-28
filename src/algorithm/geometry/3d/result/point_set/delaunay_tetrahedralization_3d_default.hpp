#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_SET_DELAUNAY_TETRAHEDRALIZATION_3D_DEFAULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_SET_DELAUNAY_TETRAHEDRALIZATION_3D_DEFAULT_HPP_INCLUDED

#include <utility>
#include <vector>

#include "../../detail/point_set/delaunay_tetrahedralization_3d_detail.hpp"

inline DelaunayTetrahedralization3 delaunay_tetrahedralization_3d(
    std::vector<Point3> input
){
    return delaunay_tetrahedralization_3d_detail::build_randomized(
        std::move(input), 0x243f6a8885a308d3ULL
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_SET_DELAUNAY_TETRAHEDRALIZATION_3D_DEFAULT_HPP_INCLUDED
