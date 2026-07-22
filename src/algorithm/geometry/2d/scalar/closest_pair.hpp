#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_CLOSEST_PAIR_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_CLOSEST_PAIR_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <vector>

#include "abs.hpp"

inline long double closest_pair(std::vector<Point> points){
    const int size = static_cast<int>(points.size());
    if(size < 2) return std::numeric_limits<long double>::infinity();
    std::sort(
        points.begin(), points.end(),
        [](const Point& left, const Point& right){
            if(left.x != right.x) return left.x < right.x;
            return left.y < right.y;
        }
    );
    std::vector<Point> buffer(static_cast<std::size_t>(size));
    const auto solve = [&](auto&& self, int left, int right) -> long double {
        if(right - left <= 1){
            return std::numeric_limits<long double>::infinity();
        }
        const int middle = (left + right) / 2;
        const long double middle_x =
            points[static_cast<std::size_t>(middle)].x;
        long double result = std::min(
            self(self, left, middle),
            self(self, middle, right)
        );
        std::inplace_merge(
            points.begin() + left,
            points.begin() + middle,
            points.begin() + right,
            [](const Point& first, const Point& second){
                if(first.y != second.y) return first.y < second.y;
                return first.x < second.x;
            }
        );
        int count = 0;
        for(int index = left; index < right; ++index){
            const Point& point = points[static_cast<std::size_t>(index)];
            if(std::abs(point.x - middle_x) >= result) continue;
            for(int candidate = count - 1; candidate >= 0; --candidate){
                const Point difference = point -
                    buffer[static_cast<std::size_t>(candidate)];
                if(difference.y >= result) break;
                result = std::min(result, abs(difference));
            }
            buffer[static_cast<std::size_t>(count++)] = point;
        }
        return result;
    };
    return solve(solve, 0, size);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_CLOSEST_PAIR_HPP_INCLUDED
