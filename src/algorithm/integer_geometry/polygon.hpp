#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POLYGON_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POLYGON_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <vector>

#include "predicates.hpp"

namespace integer_geometry{

inline Wide polygon_twice_signed_area(const std::vector<Point>& polygon){
    Wide result = 0;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Point& first = polygon[index];
        const Point& second = polygon[(index + 1) % polygon.size()];
        const Wide term = detail::checked_subtract(
            detail::checked_multiply(first.x, second.y),
            detail::checked_multiply(first.y, second.x)
        );
        result = detail::checked_add(result, term);
    }
    return result;
}

inline UnsignedWide polygon_twice_area(const std::vector<Point>& polygon){
    return detail::unsigned_magnitude(polygon_twice_signed_area(polygon));
}

inline PointLocation point_in_polygon(
    const std::vector<Point>& polygon,
    const Point& point
){
    if(polygon.empty()) return PointLocation::outside;
    int winding_number = 0;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Point& first = polygon[index];
        const Point& second = polygon[(index + 1) % polygon.size()];
        if(on_segment({first, second}, point)) return PointLocation::boundary;
        if(first.y <= point.y && point.y < second.y
            && orientation(first, second, point) > 0){
            ++winding_number;
        }else if(second.y <= point.y && point.y < first.y
            && orientation(first, second, point) < 0){
            --winding_number;
        }
    }
    return winding_number == 0 ? PointLocation::outside : PointLocation::inside;
}

namespace detail{

inline std::uint64_t coordinate_distance(Coordinate first, Coordinate second){
    if(first >= second){
        return static_cast<std::uint64_t>(first)
            - static_cast<std::uint64_t>(second);
    }
    return static_cast<std::uint64_t>(second)
        - static_cast<std::uint64_t>(first);
}
}  // namespace detail


inline UnsignedWide lattice_boundary_points(const std::vector<Point>& polygon){
    UnsignedWide result = 0;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Point& first = polygon[index];
        const Point& second = polygon[(index + 1) % polygon.size()];
        result = detail::checked_add_unsigned(
            result,
            std::gcd(
                detail::coordinate_distance(first.x, second.x),
                detail::coordinate_distance(first.y, second.y)
            )
        );
    }
    return result;
}

inline UnsignedWide lattice_interior_points(const std::vector<Point>& polygon){
    if(polygon.size() < 3){
        throw std::invalid_argument("Pick's theorem requires a polygon");
    }
    const UnsignedWide twice_area = polygon_twice_area(polygon);
    const UnsignedWide boundary = lattice_boundary_points(polygon);
    if(twice_area == 0 || ((twice_area ^ boundary) & 1) != 0){
        throw std::invalid_argument("polygon is invalid for Pick's theorem");
    }
    if(boundary > twice_area){
        if(boundary - twice_area != 2){
            throw std::invalid_argument(
                "polygon is invalid for Pick's theorem"
            );
        }
        return 0;
    }
    return detail::checked_add_unsigned((twice_area - boundary) / 2, 1);
}

struct LatticePointCount{
    UnsignedWide boundary;
    UnsignedWide interior;
};

inline LatticePointCount lattice_point_count(
    const std::vector<Point>& polygon
){
    return {
        lattice_boundary_points(polygon),
        lattice_interior_points(polygon),
    };
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POLYGON_HPP_INCLUDED
