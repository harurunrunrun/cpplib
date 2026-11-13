#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_CLOSEST_PAIR_INDICES_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_CLOSEST_PAIR_INDICES_POINTS_HPP_INCLUDED

#include "../../../../math/integer/exact_integer.hpp"

#include <algorithm>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

#include "../../type/definition/closest_pair_result.hpp"

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

// 距離の二乗を固定長で正確に表す。座標が 64 bit 以下なら差の絶対値は
// 2^64 未満なので各二乗は __uint128_t に収まり、和は 1 bit の繰り上がり
// (high) を加えれば正確に表せる。
struct SquaredDistance{
    bool high = false;
    __uint128_t low = 0;

    friend auto operator<=>(
        const SquaredDistance&,
        const SquaredDistance&
    ) = default;

    ExactInteger to_exact() const{
        ExactInteger result(low);
        if(high) result += ExactInteger(1) << 128;
        return result;
    }
};

template<std::integral Coordinate>
SquaredDistance square_difference(
    Coordinate first,
    Coordinate second
){
    const __int128_t difference =
        static_cast<__int128_t>(first) - static_cast<__int128_t>(second);
    const __uint128_t magnitude = difference < 0
        ? static_cast<__uint128_t>(-difference)
        : static_cast<__uint128_t>(difference);
    return {false, magnitude * magnitude};
}

template<std::integral Coordinate>
SquaredDistance squared_distance(
    const IndexedPoint<Coordinate>& first,
    const IndexedPoint<Coordinate>& second
){
    const __uint128_t x = square_difference(first.x, second.x).low;
    const __uint128_t y = square_difference(first.y, second.y).low;
    const __uint128_t low = x + y;
    return {low < x, low};
}

struct Result{
    std::size_t first = std::numeric_limits<std::size_t>::max();
    std::size_t second = std::numeric_limits<std::size_t>::max();
    SquaredDistance squared_distance{};

    bool exists() const{
        return first != std::numeric_limits<std::size_t>::max();
    }
};

inline void update(
    Result& result,
    std::size_t first,
    std::size_t second,
    const SquaredDistance& distance
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

    using Result = closest_pair_indices_detail::Result;
    Result duplicate_result;
    for(std::size_t index = 1; index < sorted.size(); ++index){
        if(sorted[index - 1].x == sorted[index].x
            && sorted[index - 1].y == sorted[index].y){
            closest_pair_indices_detail::update(
                duplicate_result,
                sorted[index - 1].index,
                sorted[index].index,
                {}
            );
        }
    }
    if(duplicate_result.exists()){
        return {duplicate_result.first, duplicate_result.second, 0};
    }

    std::vector<IndexedPoint> buffer(sorted.size());
    std::vector<IndexedPoint> strip;
    strip.reserve(sorted.size());
    const auto solve = [&](auto&& self, std::size_t left, std::size_t right)
        -> Result {
        if(right - left <= 3){
            Result result;
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
        Result result = self(self, left, middle);
        const Result right_result = self(self, middle, right);
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
    const Result result = solve(solve, 0, sorted.size());
    return {
        result.first,
        result.second,
        result.squared_distance.to_exact(),
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_CLOSEST_PAIR_INDICES_POINTS_HPP_INCLUDED
