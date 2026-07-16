#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "base.hpp"
#include "circumcircle.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "projection.hpp"

inline Point3 closest_point(const Ray3& ray, const Point3& point){
    using namespace geometry3d_linear_closest_detail;
    return query(
        {ray.origin, ray.through, Kind::ray},
        {point, point, Kind::segment}
    ).first;
}

inline Point3 closest_point(const Segment3& segment, const Point3& point){
    using namespace geometry3d_linear_closest_detail;
    return query(
        {segment.a, segment.b, Kind::segment},
        {point, point, Kind::segment}
    ).first;
}

inline Point3 closest_point(const Triangle3& triangle, const Point3& point){
    using namespace geometry3d_circumcircle_detail;
    const auto ab_exact =
        geometry3d_plane_numeric_detail::exact_difference(
            triangle.b, triangle.a
        );
    const auto ac_exact =
        geometry3d_plane_numeric_detail::exact_difference(
            triangle.c, triangle.a
        );
    if(exact_zero(exact_cross(ab_exact, ac_exact)))[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    const Geometry3DNormalizedDifference ab_difference =
        geometry3d_normalized_difference(triangle.b, triangle.a);
    const Geometry3DNormalizedDifference ac_difference =
        geometry3d_normalized_difference(triangle.c, triangle.a);
    const Geometry3DNormalizedDifference bc_difference =
        geometry3d_normalized_difference(triangle.c, triangle.b);
    const long double scale = std::max({
        ab_difference.scale,
        ac_difference.scale,
        bc_difference.scale,
    });
    const Point3 ab = geometry3d_safe_normalized_difference(
        triangle.b, triangle.a, scale
    );
    const Point3 ac = geometry3d_safe_normalized_difference(
        triangle.c, triangle.a, scale
    );
    const Point3 ap = geometry3d_safe_normalized_difference(
        point, triangle.a, scale
    );
    if(!geometry3d_is_finite(ap))[[unlikely]]{
        throw std::overflow_error(
            "3D triangle query is not representable locally"
        );
    }
    const long double d1 = dot(ab, ap);
    const long double d2 = dot(ac, ap);
    Point3 local{};
    if(d1 <= 0 && d2 <= 0){
        local = {};
    }else{
        const Point3 bp = ap - ab;
        const long double d3 = dot(ab, bp);
        const long double d4 = dot(ac, bp);
        if(d3 >= 0 && d4 <= d3){
            local = ab;
        }else{
            const long double vc = d1 * d4 - d3 * d2;
            if(vc <= 0 && d1 >= 0 && d3 <= 0){
                local = ab * (d1 / (d1 - d3));
            }else{
                const Point3 cp = ap - ac;
                const long double d5 = dot(ab, cp);
                const long double d6 = dot(ac, cp);
                if(d6 >= 0 && d5 <= d6){
                    local = ac;
                }else{
                    const long double vb = d5 * d2 - d1 * d6;
                    if(vb <= 0 && d2 >= 0 && d6 <= 0){
                        local = ac * (d2 / (d2 - d6));
                    }else{
                        const long double va = d3 * d6 - d5 * d4;
                        if(va <= 0 && d4 - d3 >= 0
                            && d5 - d6 >= 0){
                            const Point3 edge = ac - ab;
                            local = ab + edge * (
                                (d4 - d3)
                                / ((d4 - d3) + (d5 - d6))
                            );
                        }else{
                            const long double denominator =
                                1 / (va + vb + vc);
                            local = ab * (vb * denominator)
                                + ac * (vc * denominator);
                        }
                    }
                }
            }
        }
    }
    return geometry3d_linear_closest_detail::restore(
        local, triangle.a, scale
    );
}
