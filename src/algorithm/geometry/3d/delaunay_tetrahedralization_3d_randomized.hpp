#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DELAUNAY_TETRAHEDRALIZATION_3D_RANDOMIZED_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DELAUNAY_TETRAHEDRALIZATION_3D_RANDOMIZED_HPP_INCLUDED

#include <cstdint>
#include <utility>
#include <vector>

#include "delaunay_tetrahedralization_3d_detail.hpp"

inline DelaunayTetrahedralization3
delaunay_tetrahedralization_3d_randomized(
    std::vector<Point3> input,
    std::uint64_t seed
){
    return delaunay_tetrahedralization_3d_detail::build_randomized(
        std::move(input), seed
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DELAUNAY_TETRAHEDRALIZATION_3D_RANDOMIZED_HPP_INCLUDED
