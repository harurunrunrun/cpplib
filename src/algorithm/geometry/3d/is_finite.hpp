#pragma once

#include <cmath>

#include "base.hpp"

inline bool geometry3d_is_finite(long double value) noexcept{
    return std::isfinite(value);
}

inline bool geometry3d_is_finite(const Point3& point) noexcept{
    return geometry3d_is_finite(point.x) && geometry3d_is_finite(point.y)
        && geometry3d_is_finite(point.z);
}

inline bool geometry3d_is_finite(const Line3& line) noexcept{
    return geometry3d_is_finite(line.a) && geometry3d_is_finite(line.b);
}

inline bool geometry3d_is_finite(const Ray3& ray) noexcept{
    return geometry3d_is_finite(ray.origin) && geometry3d_is_finite(ray.through);
}

inline bool geometry3d_is_finite(const Segment3& segment) noexcept{
    return geometry3d_is_finite(segment.a) && geometry3d_is_finite(segment.b);
}

inline bool geometry3d_is_finite(const Plane3& plane) noexcept{
    return geometry3d_is_finite(plane.point) && geometry3d_is_finite(plane.normal);
}

inline bool geometry3d_is_finite(const Circle3& circle) noexcept{
    return geometry3d_is_finite(circle.center) && geometry3d_is_finite(circle.normal)
        && geometry3d_is_finite(circle.radius);
}

inline bool geometry3d_is_finite(const Sphere3& sphere) noexcept{
    return geometry3d_is_finite(sphere.center) && geometry3d_is_finite(sphere.radius);
}

inline bool geometry3d_is_finite(const Triangle3& triangle) noexcept{
    return geometry3d_is_finite(triangle.a) && geometry3d_is_finite(triangle.b)
        && geometry3d_is_finite(triangle.c);
}

inline bool geometry3d_is_finite(const Tetrahedron3& tetrahedron) noexcept{
    return geometry3d_is_finite(tetrahedron.a)
        && geometry3d_is_finite(tetrahedron.b)
        && geometry3d_is_finite(tetrahedron.c)
        && geometry3d_is_finite(tetrahedron.d);
}
