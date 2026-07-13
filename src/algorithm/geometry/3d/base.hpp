#pragma once

#include <cmath>

inline constexpr long double GEOMETRY3D_EPS = 1e-10L;
inline constexpr long double GEOMETRY3D_PI =
    3.141592653589793238462643383279502884L;

struct Point3{
    long double x = 0;
    long double y = 0;
    long double z = 0;

    Point3() = default;
    Point3(long double x_, long double y_, long double z_)
        : x(x_), y(y_), z(z_){}

    Point3 operator+() const{ return *this; }
    Point3 operator-() const{ return {-x, -y, -z}; }
    Point3 operator+(const Point3& other) const{
        return {x + other.x, y + other.y, z + other.z};
    }
    Point3 operator-(const Point3& other) const{
        return {x - other.x, y - other.y, z - other.z};
    }
    Point3 operator*(long double scalar) const{
        return {x * scalar, y * scalar, z * scalar};
    }
    Point3 operator/(long double scalar) const{
        return {x / scalar, y / scalar, z / scalar};
    }
    Point3& operator+=(const Point3& other){
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    Point3& operator-=(const Point3& other){
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    Point3& operator*=(long double scalar){
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    Point3& operator/=(long double scalar){
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    bool operator==(const Point3& other) const{
        return std::abs(x - other.x) <= GEOMETRY3D_EPS &&
            std::abs(y - other.y) <= GEOMETRY3D_EPS &&
            std::abs(z - other.z) <= GEOMETRY3D_EPS;
    }
    bool operator<(const Point3& other) const{
        if(x != other.x) return x < other.x;
        if(y != other.y) return y < other.y;
        return z < other.z;
    }
};

inline Point3 operator*(long double scalar, const Point3& point){
    return point * scalar;
}

struct Line3{
    Point3 a;
    Point3 b;
};

struct Ray3{
    Point3 origin;
    Point3 through;
};

struct Segment3{
    Point3 a;
    Point3 b;
};

struct Plane3{
    Point3 point;
    Point3 normal;
};

struct Circle3{
    Point3 center;
    Point3 normal;
    long double radius;
};

struct Sphere3{
    Point3 center;
    long double radius;
};

struct Triangle3{
    Point3 a;
    Point3 b;
    Point3 c;
};

struct Tetrahedron3{
    Point3 a;
    Point3 b;
    Point3 c;
    Point3 d;
};
