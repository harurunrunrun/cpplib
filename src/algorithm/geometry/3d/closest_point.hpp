#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "abs.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "norm.hpp"
#include "ray3_direction.hpp"

inline Point3 closest_point(const Ray3& ray, const Point3& point){
    const Point3 direction = ray3_direction(ray);
    const long double parameter = dot(point - ray.origin, direction) / norm(direction);
    return parameter <= 0 ? ray.origin : ray.origin + direction * parameter;
}

inline Point3 closest_point(const Segment3& segment, const Point3& point){
    const Point3 direction = segment.b - segment.a;
    if(geometry3d_sign(abs(direction)) == 0) return segment.a;
    const long double parameter = std::clamp(
        dot(point - segment.a, direction) / norm(direction), 0.0L, 1.0L
    );
    return segment.a + direction * parameter;
}

inline Point3 closest_point(const Triangle3& triangle, const Point3& point){
    const Point3 ab = triangle.b - triangle.a;
    const Point3 ac = triangle.c - triangle.a;
    if(geometry3d_sign(abs(cross(ab, ac))) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    const Point3 ap = point - triangle.a;
    const long double d1 = dot(ab, ap);
    const long double d2 = dot(ac, ap);
    if(d1 <= 0 && d2 <= 0) return triangle.a;

    const Point3 bp = point - triangle.b;
    const long double d3 = dot(ab, bp);
    const long double d4 = dot(ac, bp);
    if(d3 >= 0 && d4 <= d3) return triangle.b;

    const long double vc = d1 * d4 - d3 * d2;
    if(vc <= 0 && d1 >= 0 && d3 <= 0){
        return triangle.a + ab * (d1 / (d1 - d3));
    }

    const Point3 cp = point - triangle.c;
    const long double d5 = dot(ab, cp);
    const long double d6 = dot(ac, cp);
    if(d6 >= 0 && d5 <= d6) return triangle.c;

    const long double vb = d5 * d2 - d1 * d6;
    if(vb <= 0 && d2 >= 0 && d6 <= 0){
        return triangle.a + ac * (d2 / (d2 - d6));
    }

    const long double va = d3 * d6 - d5 * d4;
    if(va <= 0 && d4 - d3 >= 0 && d5 - d6 >= 0){
        const Point3 edge = triangle.c - triangle.b;
        return triangle.b + edge * ((d4 - d3) / ((d4 - d3) + (d5 - d6)));
    }

    const long double denominator = 1 / (va + vb + vc);
    return triangle.a + ab * (vb * denominator) + ac * (vc * denominator);
}
