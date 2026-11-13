#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_FURTHEST_PAIR_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_FURTHEST_PAIR_POINTS_HPP_INCLUDED

#include "../../../../math/integer/exact_integer.hpp"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

#include "../../type/definition/furthest_pair_result.hpp"

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

// Wide は ExactInteger、または全座標の絶対値が 2^62 未満と確認できた場合の
// __int128_t（差の絶対値は 2^63 未満、外積・二乗の和は 2^127 未満に収まる）。
template<class Wide, std::integral Coordinate>
Wide cross(
    const IndexedPoint<Coordinate>& first,
    const IndexedPoint<Coordinate>& second,
    const IndexedPoint<Coordinate>& third
){
    const Wide first_x = Wide(second.x) - Wide(first.x);
    const Wide first_y = Wide(second.y) - Wide(first.y);
    const Wide second_x = Wide(third.x) - Wide(first.x);
    const Wide second_y = Wide(third.y) - Wide(first.y);
    return first_x * second_y - first_y * second_x;
}

template<class Wide, std::integral Coordinate>
Wide squared_distance(
    const IndexedPoint<Coordinate>& first,
    const IndexedPoint<Coordinate>& second
){
    const Wide dx = Wide(first.x) - Wide(second.x);
    const Wide dy = Wide(first.y) - Wide(second.y);
    return dx * dx + dy * dy;
}

template<std::integral Coordinate>
bool fits_int128(const std::vector<IndexedPoint<Coordinate>>& points){
    constexpr __int128_t limit = static_cast<__int128_t>(1) << 62;
    for(const IndexedPoint<Coordinate>& point: points){
        const __int128_t x = static_cast<__int128_t>(point.x);
        const __int128_t y = static_cast<__int128_t>(point.y);
        if(x <= -limit || limit <= x || y <= -limit || limit <= y) return false;
    }
    return true;
}

template<class Wide, std::integral Coordinate>
FurthestPairResult solve(
    const std::vector<IndexedPoint<Coordinate>>& unique_points
){
    using Point = IndexedPoint<Coordinate>;
    std::vector<Point> hull;
    hull.reserve(2 * unique_points.size());
    for(const Point& point: unique_points){
        while(hull.size() >= 2 && cross<Wide>(
            hull[hull.size() - 2], hull.back(), point
        ) <= 0){
            hull.pop_back();
        }
        hull.push_back(point);
    }
    const std::size_t lower_size = hull.size();
    for(std::size_t index = unique_points.size() - 1; index-- > 0;){
        const Point& point = unique_points[index];
        while(hull.size() > lower_size && cross<Wide>(
            hull[hull.size() - 2], hull.back(), point
        ) <= 0){
            hull.pop_back();
        }
        hull.push_back(point);
    }
    hull.pop_back();

    std::size_t best_first = std::numeric_limits<std::size_t>::max();
    std::size_t best_second = std::numeric_limits<std::size_t>::max();
    Wide best_distance = 0;
    const auto update = [&](const Point& first, const Point& second){
        if(first.index == second.index) return;
        const Wide distance = squared_distance<Wide>(first, second);
        const std::size_t first_index = std::min(first.index, second.index);
        const std::size_t second_index = std::max(first.index, second.index);
        if(best_first == std::numeric_limits<std::size_t>::max()
            || distance > best_distance
            || (distance == best_distance
                && std::pair{first_index, second_index}
                    < std::pair{best_first, best_second})){
            best_first = first_index;
            best_second = second_index;
            best_distance = distance;
        }
    };
    const auto finish = [&]() -> FurthestPairResult {
        if(best_first == std::numeric_limits<std::size_t>::max()) return {};
        return {best_first, best_second, ExactInteger(best_distance)};
    };

    if(hull.size() == 2){
        update(hull[0], hull[1]);
        return finish();
    }

    const std::size_t size = hull.size();
    std::size_t opposite = 1;
    for(std::size_t index = 0; index < size; ++index){
        const std::size_t next_index = (index + 1) % size;
        while(true){
            const std::size_t next_opposite = (opposite + 1) % size;
            const Wide current_area = cross<Wide>(
                hull[index], hull[next_index], hull[opposite]
            );
            const Wide next_area = cross<Wide>(
                hull[index], hull[next_index], hull[next_opposite]
            );
            if(next_area <= current_area) break;
            opposite = next_opposite;
        }
        update(hull[index], hull[opposite]);
        update(hull[next_index], hull[opposite]);

        const std::size_t next_opposite = (opposite + 1) % size;
        const Wide current_area = cross<Wide>(
            hull[index], hull[next_index], hull[opposite]
        );
        const Wide next_area = cross<Wide>(
            hull[index], hull[next_index], hull[next_opposite]
        );
        if(next_area == current_area){
            update(hull[index], hull[next_opposite]);
            update(hull[next_index], hull[next_opposite]);
        }
    }
    return finish();
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

    if(furthest_pair_detail::fits_int128(unique_points)){
        return furthest_pair_detail::solve<__int128_t>(unique_points);
    }
    return furthest_pair_detail::solve<ExactInteger>(unique_points);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_FURTHEST_PAIR_POINTS_HPP_INCLUDED
