#ifndef CPPLIB_SRC_INTEGER_GEOMETRY_LATTICE_TYPES_HPP_INCLUDED
#define CPPLIB_SRC_INTEGER_GEOMETRY_LATTICE_TYPES_HPP_INCLUDED

#include <compare>
#include <utility>

#include "../2d/types.hpp"

namespace integer_geometry{

struct IntegerPoint{
    Integer x = 0;
    Integer y = 0;

    IntegerPoint() = default;
    IntegerPoint(Integer x_value, Integer y_value)
        : x(std::move(x_value)), y(std::move(y_value)){}

    friend bool operator==(const IntegerPoint&, const IntegerPoint&) = default;

    friend std::strong_ordering operator<=> (
        const IntegerPoint& left,
        const IntegerPoint& right
    ){
        const std::strong_ordering x_order = left.x <=> right.x;
        return x_order != 0 ? x_order : left.y <=> right.y;
    }
};

inline Point rational_point(const IntegerPoint& point){
    return {Rational(point.x), Rational(point.y)};
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_INTEGER_GEOMETRY_LATTICE_TYPES_HPP_INCLUDED
