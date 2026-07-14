#pragma once

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "../geometry.hpp"

inline std::vector<int> enumerate_points_on_polygon_boundary(
    const std::vector<Point>& polygon,
    const std::vector<Point>& points,
    bool unique = true
){
    int n = static_cast<int>(polygon.size());
    int m = static_cast<int>(points.size());
    std::vector<int> result;
    std::vector<char> used(static_cast<std::size_t>(m), 0);
    for(int i = 0; i < n; i++){
        Point a = polygon[static_cast<std::size_t>(i)];
        Point b = polygon[static_cast<std::size_t>((i + 1) % n)];
        Point base = b - a;
        long double base_norm = norm(base);
        std::vector<std::pair<long double, int>> current;
        for(int j = 0; j < m; j++){
            if(unique && used[static_cast<std::size_t>(j)]) continue;
            if(!on_segment({a, b}, points[static_cast<std::size_t>(j)])) continue;
            long double parameter = base_norm == 0.0L
                                        ? 0.0L
                                        : dot(points[static_cast<std::size_t>(j)] - a, base) / base_norm;
            current.push_back({parameter, j});
        }
        std::sort(current.begin(), current.end(), [](const auto& lhs, const auto& rhs){
            if(lhs.first != rhs.first) return lhs.first < rhs.first;
            return lhs.second < rhs.second;
        });
        for(auto [parameter, index]: current){
            (void)parameter;
            result.push_back(index);
            if(unique) used[static_cast<std::size_t>(index)] = 1;
        }
    }
    return result;
}
