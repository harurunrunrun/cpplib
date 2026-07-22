#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPES_HPP_INCLUDED

#include "geometry_sign.hpp"

struct Point{
    long double x;
    long double y;

    Point() : x(0), y(0){}
    Point(long double x_, long double y_) : x(x_), y(y_){}

    Point operator+() const{ return *this; }
    Point operator-() const{ return {-x, -y}; }
    Point operator+(const Point& other) const{
        return {x + other.x, y + other.y};
    }
    Point operator-(const Point& other) const{
        return {x - other.x, y - other.y};
    }
    Point operator*(long double scalar) const{
        return {x * scalar, y * scalar};
    }
    Point operator/(long double scalar) const{
        return {x / scalar, y / scalar};
    }
    Point& operator+=(const Point& other){
        x += other.x;
        y += other.y;
        return *this;
    }
    Point& operator-=(const Point& other){
        x -= other.x;
        y -= other.y;
        return *this;
    }
    Point& operator*=(long double scalar){
        x *= scalar;
        y *= scalar;
        return *this;
    }
    Point& operator/=(long double scalar){
        x /= scalar;
        y /= scalar;
        return *this;
    }
    bool operator<(const Point& other) const{
        if(x != other.x) return x < other.x;
        return y < other.y;
    }
    bool operator==(const Point& other) const{
        return geometry_sign(x - other.x) == 0 &&
            geometry_sign(y - other.y) == 0;
    }
};

inline Point operator*(long double scalar, const Point& point){
    return point * scalar;
}

struct Line{
    Point a;
    Point b;
};

using Segment = Line;

struct Circle{
    Point center;
    long double radius;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPES_HPP_INCLUDED
