#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#include "advanced/detail.hpp"

namespace convex_hull_detail{

inline std::vector<Point> sorted_unique_points(std::vector<Point> points){
    std::sort(points.begin(), points.end());
    points.erase(std::unique(
        points.begin(), points.end(),
        [](const Point& left, const Point& right){
            return left.x == right.x && left.y == right.y;
        }
    ), points.end());
    return points;
}

inline std::vector<Point> build_chain_from_sorted(
    const std::vector<Point>& points,
    bool upper,
    bool keep_collinear
){
    std::vector<Point> chain;
    chain.reserve(points.size());
    for(const Point& point: points){
        while(chain.size() >= 2){
            const int turn = advanced_geometry_detail::cross_sign(
                chain.back() - chain[chain.size() - 2],
                point - chain.back()
            );
            const bool invalid = upper
                ? (keep_collinear ? turn > 0 : turn >= 0)
                : (keep_collinear ? turn < 0 : turn <= 0);
            if(!invalid) break;
            chain.pop_back();
        }
        chain.push_back(point);
    }
    return chain;
}

inline std::vector<Point> build_convex_hull(
    std::vector<Point> points,
    bool keep_collinear
){
    points = sorted_unique_points(std::move(points));
    if(points.size() <= 1) return points;

    if(keep_collinear){
        bool all_collinear = true;
        for(std::size_t index = 2; index < points.size(); ++index){
            if(advanced_geometry_detail::cross_sign(
                points[1] - points[0], points[index] - points[0]
            ) != 0){
                all_collinear = false;
                break;
            }
        }
        if(all_collinear) return points;
    }

    std::vector<Point> lower = build_chain_from_sorted(
        points, false, keep_collinear
    );
    std::vector<Point> upper = build_chain_from_sorted(
        points, true, keep_collinear
    );
    lower.reserve(lower.size() + upper.size() - 2);
    for(std::size_t index = upper.size() - 1; index-- > 1;){
        lower.push_back(upper[index]);
    }
    return lower;
}

}  // namespace convex_hull_detail
