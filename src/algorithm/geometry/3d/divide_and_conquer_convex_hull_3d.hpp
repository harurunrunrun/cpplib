#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "convex_hull_3d.hpp"
#include "is_finite.hpp"

namespace divide_and_conquer_convex_hull_3d_detail{

inline ConvexPolyhedron3 solve(
    const std::vector<Point3>& points,
    std::size_t begin,
    std::size_t end
){
    if(end - begin <= 32){
        return convex_hull_3d(std::vector<Point3>(
            points.begin() + static_cast<std::ptrdiff_t>(begin),
            points.begin() + static_cast<std::ptrdiff_t>(end)
        ));
    }
    const std::size_t middle = begin + (end - begin) / 2;
    ConvexPolyhedron3 left = solve(points, begin, middle);
    ConvexPolyhedron3 right = solve(points, middle, end);
    left.vertices.insert(
        left.vertices.end(), right.vertices.begin(), right.vertices.end()
    );
    return convex_hull_3d(std::move(left.vertices));
}

}  // namespace divide_and_conquer_convex_hull_3d_detail

inline ConvexPolyhedron3 divide_and_conquer_convex_hull_3d(
    std::vector<Point3> points
){
    for(const Point3& point: points){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument(
                "divide_and_conquer_convex_hull_3d requires finite points"
            );
        }
    }
    std::sort(points.begin(), points.end());
    points.erase(std::unique(points.begin(), points.end(), [](const Point3& a,
                                                              const Point3& b){
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }), points.end());
    return divide_and_conquer_convex_hull_3d_detail::solve(
        points, 0, points.size()
    );
}
