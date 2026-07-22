#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_QUERY_CLOSEST_PAIR_INDICES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_QUERY_CLOSEST_PAIR_INDICES_HPP_INCLUDED

#include "../../../math/integer/exact_integer.hpp"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

struct ClosestPairResult{
    std::size_t first = std::numeric_limits<std::size_t>::max();
    std::size_t second = std::numeric_limits<std::size_t>::max();
    ExactInteger squared_distance = 0;

    bool exists() const{
        return first != std::numeric_limits<std::size_t>::max();
    }
};

namespace closest_pair_indices_detail{

template<std::integral Coordinate>
struct IndexedPoint{
    static_assert(
        sizeof(Coordinate) <= sizeof(std::uint64_t),
        "closest_pair_indices supports integer coordinates up to 64 bits"
    );
    Coordinate x;
    Coordinate y;
    std::size_t index;
};

template<std::integral Coordinate>
ExactInteger square_difference(
    Coordinate first,
    Coordinate second
){
    const ExactInteger difference = ExactInteger(first) - ExactInteger(second);
    return difference * difference;
}

template<std::integral Coordinate>
ExactInteger squared_distance(
    const IndexedPoint<Coordinate>& first,
    const IndexedPoint<Coordinate>& second
){
    return square_difference(first.x, second.x)
        + square_difference(first.y, second.y);
}

inline void update(
    ClosestPairResult& result,
    std::size_t first,
    std::size_t second,
    const ExactInteger& distance
){
    if(first > second) std::swap(first, second);
    if(!result.exists() || distance < result.squared_distance
        || (distance == result.squared_distance
            && std::pair{first, second} < std::pair{result.first, result.second})){
        result = {first, second, distance};
    }
}

}  // namespace closest_pair_indices_detail

template<std::integral Coordinate>
ClosestPairResult closest_pair_indices(
    const std::vector<std::pair<Coordinate, Coordinate>>& points
){
    using IndexedPoint = closest_pair_indices_detail::IndexedPoint<Coordinate>;
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

    ClosestPairResult duplicate_result;
    for(std::size_t index = 1; index < sorted.size(); ++index){
        if(sorted[index - 1].x == sorted[index].x
            && sorted[index - 1].y == sorted[index].y){
            closest_pair_indices_detail::update(
                duplicate_result,
                sorted[index - 1].index,
                sorted[index].index,
                0
            );
        }
    }
    if(duplicate_result.exists()) return duplicate_result;

    std::vector<IndexedPoint> buffer(sorted.size());
    std::vector<IndexedPoint> strip;
    strip.reserve(sorted.size());
    const auto solve = [&](auto&& self, std::size_t left, std::size_t right)
        -> ClosestPairResult {
        if(right - left <= 3){
            ClosestPairResult result;
            for(std::size_t first = left; first < right; ++first){
                for(std::size_t second = first + 1; second < right; ++second){
                    closest_pair_indices_detail::update(
                        result,
                        sorted[first].index,
                        sorted[second].index,
                        closest_pair_indices_detail::squared_distance(
                            sorted[first], sorted[second]
                        )
                    );
                }
            }
            std::sort(
                sorted.begin() + static_cast<std::ptrdiff_t>(left),
                sorted.begin() + static_cast<std::ptrdiff_t>(right),
                [](const auto& first, const auto& second){
                    if(first.y != second.y) return first.y < second.y;
                    if(first.x != second.x) return first.x < second.x;
                    return first.index < second.index;
                }
            );
            return result;
        }

        const std::size_t middle = (left + right) / 2;
        const Coordinate middle_x = sorted[middle].x;
        ClosestPairResult result = self(self, left, middle);
        const ClosestPairResult right_result = self(self, middle, right);
        if(right_result.exists()){
            closest_pair_indices_detail::update(
                result,
                right_result.first,
                right_result.second,
                right_result.squared_distance
            );
        }

        std::merge(
            sorted.begin() + static_cast<std::ptrdiff_t>(left),
            sorted.begin() + static_cast<std::ptrdiff_t>(middle),
            sorted.begin() + static_cast<std::ptrdiff_t>(middle),
            sorted.begin() + static_cast<std::ptrdiff_t>(right),
            buffer.begin() + static_cast<std::ptrdiff_t>(left),
            [](const auto& first, const auto& second){
                if(first.y != second.y) return first.y < second.y;
                if(first.x != second.x) return first.x < second.x;
                return first.index < second.index;
            }
        );
        std::copy(
            buffer.begin() + static_cast<std::ptrdiff_t>(left),
            buffer.begin() + static_cast<std::ptrdiff_t>(right),
            sorted.begin() + static_cast<std::ptrdiff_t>(left)
        );

        strip.clear();
        for(std::size_t index = left; index < right; ++index){
            if(closest_pair_indices_detail::square_difference(
                sorted[index].x, middle_x
            ) > result.squared_distance) continue;
            for(std::size_t candidate = strip.size(); candidate-- > 0;){
                if(closest_pair_indices_detail::square_difference(
                    sorted[index].y, strip[candidate].y
                ) > result.squared_distance) break;
                closest_pair_indices_detail::update(
                    result,
                    sorted[index].index,
                    strip[candidate].index,
                    closest_pair_indices_detail::squared_distance(
                        sorted[index], strip[candidate]
                    )
                );
            }
            strip.push_back(sorted[index]);
        }
        return result;
    };
    return solve(solve, 0, sorted.size());
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_QUERY_CLOSEST_PAIR_INDICES_HPP_INCLUDED
