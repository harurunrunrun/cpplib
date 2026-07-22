#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_MINIMUM_ENCLOSING_CIRCLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_MINIMUM_ENCLOSING_CIRCLE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "advanced/detail.hpp"
#include "circumcircle.hpp"
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
    static MinimumEnclosingCircleContainment compare_value(
        long double value,
        long double scale
    ){
        const int sign = advanced_geometry_detail::scaled_sign(value, scale);
        if(sign < 0) return MinimumEnclosingCircleContainment::INSIDE;
        if(sign == 0) return MinimumEnclosingCircleContainment::ON_BOUNDARY;
        return MinimumEnclosingCircleContainment::OUTSIDE;
    }

    static MinimumEnclosingCircleContainment compare_distance(
        long double point_distance,
        long double boundary_distance
    ){
        if(!std::isfinite(boundary_distance)){
            throw std::overflow_error(
                "minimum enclosing circle is not representable"
            );
        }
        if(!std::isfinite(point_distance)){
            return MinimumEnclosingCircleContainment::OUTSIDE;
        }
        return compare_value(
            point_distance - boundary_distance,
            std::max(point_distance, boundary_distance)
        );
    }

    MinimumEnclosingCircleContainment diameter_containment(
        const Point& point
    ) const{
        const Point doubled_offset =
            (point - support[0]) + (point - support[1]);
        const long double doubled_distance =
            advanced_geometry_detail::length(doubled_offset);
        const long double diameter = advanced_geometry_detail::length(
            support[1] - support[0]
        );
        return compare_distance(doubled_distance, diameter);
    }

    MinimumEnclosingCircleContainment circumcircle_containment(
        const Point& point
    ) const{
        const Point center = circumcenter(
            support[0], support[1], support[2]
        );
        const long double point_distance =
            advanced_geometry_detail::length(point - center);
        const long double radius =
            advanced_geometry_detail::length(support[0] - center);
        return compare_distance(point_distance, radius);
    }

public:
    MinimumEnclosingCircleContainment containment(const Point& point) const{
        if(support_size == 0){
            return MinimumEnclosingCircleContainment::OUTSIDE;
        }
        if(support_size == 1){
            const long double point_distance =
                advanced_geometry_detail::length(point - support[0]);
            return compare_distance(point_distance, 0.0L);
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
            const Point direction = support[1] - support[0];
            const long double radius = advanced_geometry_detail::length(
                direction
            ) / 2.0L;
            if(!std::isfinite(radius)){
                throw std::overflow_error(
                    "minimum enclosing circle is not representable"
                );
            }
            const Point center = support[0] + direction / 2.0L;
            return {center, radius};
        }
        const Circle result = circumcircle(
            support[0], support[1], support[2]
        );
        if(!std::isfinite(result.center.x) ||
            !std::isfinite(result.center.y) ||
            !std::isfinite(result.radius)){
            throw std::overflow_error(
                "minimum enclosing circle is not representable"
            );
        }
        return result;
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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_MINIMUM_ENCLOSING_CIRCLE_HPP_INCLUDED
