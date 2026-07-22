#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_FURTHEST_PAIR_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_FURTHEST_PAIR_HPP_INCLUDED

#include "../../math/integer/exact_integer.hpp"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

struct FurthestPairResult{
    std::size_t first = std::numeric_limits<std::size_t>::max();
    std::size_t second = std::numeric_limits<std::size_t>::max();
    ExactInteger squared_distance = 0;

    bool exists() const{
        return first != std::numeric_limits<std::size_t>::max();
    }
};

namespace furthest_pair_detail{

template<std::integral Coordinate>
struct IndexedPoint{
    static_assert(
        sizeof(Coordinate) <= sizeof(std::uint64_t),
        "furthest_pair supports integer coordinates up to 64 bits"
    );
    Coordinate x;
    Coordinate y;
    std::size_t index;
};

template<std::integral Coordinate>
ExactInteger cross(
    const IndexedPoint<Coordinate>& first,
    const IndexedPoint<Coordinate>& second,
    const IndexedPoint<Coordinate>& third
){
    const ExactInteger first_x = ExactInteger(second.x) - ExactInteger(first.x);
    const ExactInteger first_y = ExactInteger(second.y) - ExactInteger(first.y);
    const ExactInteger second_x = ExactInteger(third.x) - ExactInteger(first.x);
    const ExactInteger second_y = ExactInteger(third.y) - ExactInteger(first.y);
    return first_x * second_y - first_y * second_x;
}

template<std::integral Coordinate>
ExactInteger squared_distance(
    const IndexedPoint<Coordinate>& first,
    const IndexedPoint<Coordinate>& second
){
    const ExactInteger dx = ExactInteger(first.x) - ExactInteger(second.x);
    const ExactInteger dy = ExactInteger(first.y) - ExactInteger(second.y);
    return dx * dx + dy * dy;
}

}  // namespace furthest_pair_detail

template<std::integral Coordinate>
FurthestPairResult furthest_pair(
    const std::vector<std::pair<Coordinate, Coordinate>>& points
){
    using IndexedPoint = furthest_pair_detail::IndexedPoint<Coordinate>;
    if(points.size() < 2) return {};

    std::vector<IndexedPoint> sorted;
    sorted.reserve(points.size());
    for(std::size_t index = 0; index < points.size(); ++index){
        sorted.push_back({points[index].first, points[index].second, index});
    }
    std::sort(sorted.begin(), sorted.end(), [](const auto& left, const auto& right){
        if(left.x != right.x) return left.x < right.x;
        if(left.y != right.y) return left.y < right.y;
        return left.index < right.index;
    });

    std::vector<IndexedPoint> unique_points;
    unique_points.reserve(sorted.size());
    for(const IndexedPoint& point: sorted){
        if(unique_points.empty() || unique_points.back().x != point.x
            || unique_points.back().y != point.y){
            unique_points.push_back(point);
        }
    }
    if(unique_points.size() == 1){
        return {0, 1, 0};
    }

    std::vector<IndexedPoint> hull;
    hull.reserve(2 * unique_points.size());
    for(const IndexedPoint& point: unique_points){
        while(hull.size() >= 2 && furthest_pair_detail::cross(
            hull[hull.size() - 2], hull.back(), point
        ) <= 0){
            hull.pop_back();
        }
        hull.push_back(point);
    }
    const std::size_t lower_size = hull.size();
    for(std::size_t index = unique_points.size() - 1; index-- > 0;){
        const IndexedPoint& point = unique_points[index];
        while(hull.size() > lower_size && furthest_pair_detail::cross(
            hull[hull.size() - 2], hull.back(), point
        ) <= 0){
            hull.pop_back();
        }
        hull.push_back(point);
    }
    hull.pop_back();

    FurthestPairResult result;
    const auto update = [&result](const IndexedPoint& first, const IndexedPoint& second){
        if(first.index == second.index) return;
        const ExactInteger distance =
            furthest_pair_detail::squared_distance(first, second);
        const std::size_t first_index = std::min(first.index, second.index);
        const std::size_t second_index = std::max(first.index, second.index);
        if(!result.exists() || distance > result.squared_distance ||
           (distance == result.squared_distance &&
            std::pair{first_index, second_index}
                < std::pair{result.first, result.second})){
            result = {first_index, second_index, distance};
        }
    };

    if(hull.size() == 2){
        update(hull[0], hull[1]);
        return result;
    }

    const std::size_t size = hull.size();
    std::size_t opposite = 1;
    for(std::size_t index = 0; index < size; ++index){
        const std::size_t next_index = (index + 1) % size;
        while(true){
            const std::size_t next_opposite = (opposite + 1) % size;
            const ExactInteger current_area = furthest_pair_detail::cross(
                hull[index], hull[next_index], hull[opposite]
            );
            const ExactInteger next_area = furthest_pair_detail::cross(
                hull[index], hull[next_index], hull[next_opposite]
            );
            if(next_area <= current_area) break;
            opposite = next_opposite;
        }
        update(hull[index], hull[opposite]);
        update(hull[next_index], hull[opposite]);

        const std::size_t next_opposite = (opposite + 1) % size;
        const ExactInteger current_area = furthest_pair_detail::cross(
            hull[index], hull[next_index], hull[opposite]
        );
        const ExactInteger next_area = furthest_pair_detail::cross(
            hull[index], hull[next_index], hull[next_opposite]
        );
        if(next_area == current_area){
            update(hull[index], hull[next_opposite]);
            update(hull[next_index], hull[next_opposite]);
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_FURTHEST_PAIR_HPP_INCLUDED
