#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_POLYHEDRON_DIAMETER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_POLYHEDRON_DIAMETER_HPP_INCLUDED

#include <optional>

#include "../../aggregate/all/point_set_diameter_3d.hpp"
#include "../../type/definition/polyhedron3.hpp"

inline std::optional<PointSetDiameter3DResult> polyhedron_diameter(
    const Polyhedron3& polyhedron
){
    return point_set_diameter_3d(polyhedron.vertices);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_POLYHEDRON_DIAMETER_HPP_INCLUDED
