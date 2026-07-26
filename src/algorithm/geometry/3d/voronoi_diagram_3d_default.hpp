#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_VORONOI_DIAGRAM_3D_DEFAULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_VORONOI_DIAGRAM_3D_DEFAULT_HPP_INCLUDED

#include <utility>
#include <vector>

#include "delaunay_tetrahedralization_3d_default.hpp"
#include "voronoi_diagram_3d_detail.hpp"

inline VoronoiDiagram3 voronoi_diagram_3d(std::vector<Point3> points){
    return voronoi_diagram_3d_detail::from_delaunay(
        delaunay_tetrahedralization_3d(std::move(points))
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_VORONOI_DIAGRAM_3D_DEFAULT_HPP_INCLUDED
