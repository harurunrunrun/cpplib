#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_POINT_PROJECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_POINT_PROJECTION_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

#include "constants.hpp"
#include "dot.hpp"
#include "line3.hpp"
#include "linear_closest_detail.hpp"
#include "point3.hpp"
#include "unit.hpp"

inline Point3 projection(const Line3& line, const Point3& point){
    using namespace geometry3d_linear_closest_detail;
    const Geometry3DNormalizedDifference direction =
        geometry3d_normalized_difference(line.b, line.a);
    if(!nonzero(direction))[[unlikely]]{
        throw std::invalid_argument("degenerate 3D linear primitive");
    }
    const Geometry3DNormalizedDifference difference =
        geometry3d_normalized_difference(point, line.a);
    const Point3 direction_unit = unit(direction.value);
    const long double parameter = dot(difference.value, direction_unit);
    const long double difference_length = std::hypot(
        difference.value.x,
        difference.value.y,
        difference.value.z
    );
    if(parameter == 0.0L
        || (std::abs(parameter) <= GEOMETRY3D_EPS * difference_length
            && exact_orthogonal_differences(
                line.b, line.a, point, line.a
            ))){
        return line.a;
    }
    return restore(
        direction_unit * parameter,
        line.a,
        difference.scale
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_POINT_PROJECTION_HPP_INCLUDED
