#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "circumcircle.hpp"
#include "cross.hpp"
#include "norm.hpp"
#include "types.hpp"

enum class MinimumEnclosingCircleContainment : int{
    OUTSIDE = -1,
    ON_BOUNDARY = 0,
    INSIDE = 1,
};

struct MinimumEnclosingCircleResult{
    std::array<Point, 3> support{};
    std::size_t support_size = 0;

private:
    static MinimumEnclosingCircleContainment compare_value(long double value){
        const int sign = geometry_sign(value);
        if(sign < 0) return MinimumEnclosingCircleContainment::INSIDE;
        if(sign == 0) return MinimumEnclosingCircleContainment::ON_BOUNDARY;
        return MinimumEnclosingCircleContainment::OUTSIDE;
    }

    MinimumEnclosingCircleContainment diameter_containment(
        const Point& point
    ) const{
        const Point doubled = point * 2.0L - support[0] - support[1];
        return compare_value(norm(doubled) - norm(support[0] - support[1]));
    }

    MinimumEnclosingCircleContainment circumcircle_containment(
        const Point& point
    ) const{
        const Point first = support[0] - point;
        const Point second = support[1] - point;
        const Point third = support[2] - point;
        const long double determinant =
            first.x * (second.y * norm(third) - third.y * norm(second))
            - first.y * (second.x * norm(third) - third.x * norm(second))
            + norm(first) * cross(second, third);
        const int orientation = geometry_sign(cross(
            support[1] - support[0], support[2] - support[0]
        ));
        return compare_value(-static_cast<long double>(orientation) * determinant);
    }

public:
    MinimumEnclosingCircleContainment containment(const Point& point) const{
        if(support_size == 0){
            return MinimumEnclosingCircleContainment::OUTSIDE;
        }
        if(support_size == 1){
            return geometry_sign(norm(point - support[0])) == 0
                ? MinimumEnclosingCircleContainment::ON_BOUNDARY
                : MinimumEnclosingCircleContainment::OUTSIDE;
        }
        if(support_size == 2) return diameter_containment(point);
        return circumcircle_containment(point);
    }

    bool contains(const Point& point) const{
        return containment(point) != MinimumEnclosingCircleContainment::OUTSIDE;
    }

    bool on_boundary(const Point& point) const{
        return containment(point)
            == MinimumEnclosingCircleContainment::ON_BOUNDARY;
    }

    Circle circle() const{
        if(support_size == 0) return {{0, 0}, 0};
        if(support_size == 1) return {support[0], 0};
        if(support_size == 2){
            const Point center = (support[0] + support[1]) / 2.0L;
            return {center, std::sqrt(norm(support[0] - support[1])) / 2.0L};
        }
        return circumcircle(support[0], support[1], support[2]);
    }
};

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
    if(geometry_sign(cross(
        result.support[1] - result.support[0],
        result.support[2] - result.support[0]
    )) != 0){
        return;
    }
    const long double first_second = norm(
        result.support[0] - result.support[1]
    );
    const long double second_third = norm(
        result.support[1] - result.support[2]
    );
    const long double first_third = norm(
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
