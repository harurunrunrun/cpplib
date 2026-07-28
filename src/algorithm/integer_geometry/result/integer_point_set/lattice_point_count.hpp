#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_RESULT_INTEGER_POINT_SET_LATTICE_POINT_COUNT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_RESULT_INTEGER_POINT_SET_LATTICE_POINT_COUNT_HPP_INCLUDED

#include "../../integer/integer_point_set/lattice_polygon_metrics.hpp"

namespace integer_geometry{

inline LatticePointCount lattice_point_count(
    const std::vector<IntegerPoint>& polygon
){
    return {
        lattice_boundary_points(polygon),
        lattice_interior_points(polygon),
    };
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_RESULT_INTEGER_POINT_SET_LATTICE_POINT_COUNT_HPP_INCLUDED
