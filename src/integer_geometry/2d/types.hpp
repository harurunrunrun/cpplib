#ifndef CPPLIB_SRC_INTEGER_GEOMETRY_2D_TYPES_HPP_INCLUDED
#define CPPLIB_SRC_INTEGER_GEOMETRY_2D_TYPES_HPP_INCLUDED

#include <compare>
#include <utility>

#include "../number/rational.hpp"

namespace integer_geometry{

struct Vector{
    Rational x;
    Rational y;

    Vector() = default;
    Vector(Rational x_value, Rational y_value)
        : x(std::move(x_value)), y(std::move(y_value)){}

    friend bool operator==(const Vector&, const Vector&) = default;

    friend Vector operator+(Vector left, const Vector& right){
        left.x += right.x;
        left.y += right.y;
        return left;
    }

    friend Vector operator-(Vector left, const Vector& right){
        left.x -= right.x;
        left.y -= right.y;
        return left;
    }

    friend Vector operator-(const Vector& value){
        return {-value.x, -value.y};
    }

    friend Vector operator*(Vector value, const Rational& scalar){
        value.x *= scalar;
        value.y *= scalar;
        return value;
    }

    friend Vector operator*(const Rational& scalar, Vector value){
        return value * scalar;
    }

    friend Vector operator/(Vector value, const Rational& scalar){
        value.x /= scalar;
        value.y /= scalar;
        return value;
    }
};

struct Point{
    Rational x;
    Rational y;

    Point() = default;
    Point(Rational x_value, Rational y_value)
        : x(std::move(x_value)), y(std::move(y_value)){}

    friend bool operator==(const Point&, const Point&) = default;

    friend std::strong_ordering operator<=> (
        const Point& left,
        const Point& right
    ){
        const std::strong_ordering x_order = left.x <=> right.x;
        return x_order != 0 ? x_order : left.y <=> right.y;
    }

    friend Point operator+(Point point, const Vector& vector){
        point.x += vector.x;
        point.y += vector.y;
        return point;
    }

    friend Point operator-(Point point, const Vector& vector){
        point.x -= vector.x;
        point.y -= vector.y;
        return point;
    }

    friend Vector operator-(const Point& left, const Point& right){
        return {left.x - right.x, left.y - right.y};
    }
};

struct Line{
    Point a;
    Point b;
};

struct Segment{
    Point a;
    Point b;
};

inline Vector direction(const Line& line){
    return line.b - line.a;
}

inline Vector direction(const Segment& segment){
    return segment.b - segment.a;
}

inline Vector vector_from(const Point& from, const Point& to){
    return to - from;
}

enum class SegmentIntersectionKind{
    none,
    touch,
    proper,
    overlap,
};

enum class PointLocation{
    outside,
    boundary,
    inside,
};

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_INTEGER_GEOMETRY_2D_TYPES_HPP_INCLUDED
