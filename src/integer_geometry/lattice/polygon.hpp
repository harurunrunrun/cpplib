#ifndef CPPLIB_SRC_INTEGER_GEOMETRY_LATTICE_POLYGON_HPP_INCLUDED
#define CPPLIB_SRC_INTEGER_GEOMETRY_LATTICE_POLYGON_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "types.hpp"

namespace integer_geometry{

inline Integer lattice_polygon_twice_signed_area(
    const std::vector<IntegerPoint>& polygon
){
    Integer result = 0;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const IntegerPoint& first = polygon[index];
        const IntegerPoint& second = polygon[(index + 1) % polygon.size()];
        result += first.x * second.y - first.y * second.x;
    }
    return result;
}

inline Integer lattice_polygon_twice_area(
    const std::vector<IntegerPoint>& polygon
){
    return abs(lattice_polygon_twice_signed_area(polygon));
}

inline Integer lattice_boundary_points(
    const std::vector<IntegerPoint>& polygon
){
    Integer result = 0;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const IntegerPoint& first = polygon[index];
        const IntegerPoint& second = polygon[(index + 1) % polygon.size()];
        result += detail::gcd(
            abs(second.x - first.x),
            abs(second.y - first.y)
        );
    }
    return result;
}

inline Integer lattice_interior_points(
    const std::vector<IntegerPoint>& polygon
){
    if(polygon.size() < 3){
        throw std::invalid_argument("Pick's theorem requires a polygon");
    }
    const Integer twice_area = lattice_polygon_twice_area(polygon);
    const Integer boundary = lattice_boundary_points(polygon);
    const Integer doubled_interior = twice_area - boundary + 2;
    if(twice_area.is_zero() || doubled_interior < 0
        || doubled_interior.divmod(2).second != 0){
        throw std::invalid_argument("polygon is invalid for Pick's theorem");
    }
    return detail::divide_exact(doubled_interior, Integer{2});
}

struct LatticePointCount{
    Integer boundary;
    Integer interior;
};

inline LatticePointCount lattice_point_count(
    const std::vector<IntegerPoint>& polygon
){
    return {
        lattice_boundary_points(polygon),
        lattice_interior_points(polygon),
    };
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_INTEGER_GEOMETRY_LATTICE_POLYGON_HPP_INCLUDED
