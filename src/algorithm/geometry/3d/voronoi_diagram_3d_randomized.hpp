#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_VORONOI_DIAGRAM_3D_RANDOMIZED_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_VORONOI_DIAGRAM_3D_RANDOMIZED_HPP_INCLUDED

#include <cstdint>
#include <utility>
#include <vector>

#include "delaunay_tetrahedralization_3d_randomized.hpp"
#include "voronoi_diagram_3d_detail.hpp"

inline VoronoiDiagram3 voronoi_diagram_3d_randomized(
    std::vector<Point3> points,
    std::uint64_t seed
){
    return voronoi_diagram_3d_detail::from_delaunay(
        delaunay_tetrahedralization_3d_randomized(
            std::move(points), seed
        )
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_VORONOI_DIAGRAM_3D_RANDOMIZED_HPP_INCLUDED
