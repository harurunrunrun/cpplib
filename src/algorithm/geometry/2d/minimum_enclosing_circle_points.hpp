#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_MINIMUM_ENCLOSING_CIRCLE_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_MINIMUM_ENCLOSING_CIRCLE_POINTS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "advanced_convex_geometry_detail.hpp"
#include "minimum_enclosing_circle_result.hpp"

namespace minimum_enclosing_circle_detail{

inline std::uint64_t next_random(std::uint64_t& state){
    state ^= state << 7;
    state ^= state >> 9;
    return state;
}

inline void deterministic_shuffle(
    std::vector<Point>& points,
    std::uint64_t seed
){
    for(std::size_t size = points.size(); size > 1; --size){
        const std::size_t other = static_cast<std::size_t>(
            next_random(seed) % size
        );
        std::swap(points[size - 1], points[other]);
    }
}

inline void reduce_collinear_support(MinimumEnclosingCircleResult& result){
    if(result.support_size != 3) return;
    if(advanced_geometry_detail::cross_sign(
        result.support[1] - result.support[0],
        result.support[2] - result.support[0]
    ) != 0){
        return;
    }
    const long double first_second = advanced_geometry_detail::length(
        result.support[0] - result.support[1]
    );
    const long double second_third = advanced_geometry_detail::length(
        result.support[1] - result.support[2]
    );
    const long double first_third = advanced_geometry_detail::length(
        result.support[0] - result.support[2]
    );
    result.support_size = 2;
    if(second_third >= first_second && second_third >= first_third){
        result.support[0] = result.support[1];
        result.support[1] = result.support[2];
    }else if(first_third >= first_second){
        result.support[1] = result.support[2];
    }
}

}  // namespace minimum_enclosing_circle_detail

inline MinimumEnclosingCircleResult minimum_enclosing_circle(
    std::vector<Point> points,
    std::uint64_t seed = 0x9e3779b97f4a7c15ULL
){
    minimum_enclosing_circle_detail::deterministic_shuffle(points, seed);
    MinimumEnclosingCircleResult result;
    for(std::size_t first = 0; first < points.size(); ++first){
        if(result.contains(points[first])) continue;
        result.support_size = 1;
        result.support[0] = points[first];
        for(std::size_t second = 0; second < first; ++second){
            if(result.contains(points[second])) continue;
            result.support_size = 2;
            result.support[0] = points[first];
            result.support[1] = points[second];
            for(std::size_t third = 0; third < second; ++third){
                if(result.contains(points[third])) continue;
                result.support_size = 3;
                result.support[0] = points[first];
                result.support[1] = points[second];
                result.support[2] = points[third];
                minimum_enclosing_circle_detail::reduce_collinear_support(result);
            }
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_MINIMUM_ENCLOSING_CIRCLE_POINTS_HPP_INCLUDED
