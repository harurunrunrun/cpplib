#ifndef CPPLIB_SRC_APPROXIMATE_GEOMETRY_RANSAC_LINE_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GEOMETRY_RANSAC_LINE_HPP_INCLUDED

#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

namespace approximate::geometry {

struct RansacLineResult {
    bool found = false;
    std::array<long double, 2> point{};
    std::array<long double, 2> direction{};
    std::vector<std::size_t> inliers;
    long double squared_error = 0.0L;
};

namespace detail {

template<class T>
void validate_ransac_point(const std::array<T, 2>& point) {
    if(!std::isfinite(static_cast<long double>(point[0]))
       || !std::isfinite(static_cast<long double>(point[1]))){
        throw std::invalid_argument("point contains a non-finite coordinate");
    }
}

inline long double line_squared_distance(
    const std::array<long double, 2>& point,
    const std::array<long double, 2>& origin,
    const std::array<long double, 2>& unit_direction
) {
    const long double dx = point[0] - origin[0];
    const long double dy = point[1] - origin[1];
    const long double cross = dx * unit_direction[1] - dy * unit_direction[0];
    const long double result = cross * cross;
    if(!std::isfinite(result)) throw std::overflow_error("line distance overflow");
    return result;
}

}  // namespace detail

template<class T, class RandomEngine>
RansacLineResult ransac_line(
    const std::vector<std::array<T, 2>>& points,
    long double inlier_distance,
    std::size_t iterations,
    RandomEngine& random,
    std::size_t minimum_inliers = 2
) {
    if(!std::isfinite(inlier_distance) || inlier_distance < 0.0L){
        throw std::invalid_argument("inlier_distance must be finite and non-negative");
    }
    if(iterations == 0) throw std::invalid_argument("iterations must be positive");
    if(minimum_inliers < 2) throw std::invalid_argument("minimum_inliers must be at least two");
    for(const auto& point: points) detail::validate_ransac_point(point);
    if(points.size() < 2 || minimum_inliers > points.size()) return {};
    const long double squared_threshold = inlier_distance * inlier_distance;
    if(!std::isfinite(squared_threshold)){
        throw std::overflow_error("squared inlier distance overflow");
    }

    RansacLineResult best;
    best.squared_error = std::numeric_limits<long double>::infinity();
    std::uniform_int_distribution<std::size_t> distribution(0, points.size() - 1);
    std::uniform_int_distribution<std::size_t> other_distribution(0, points.size() - 2);
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        const std::size_t first = distribution(random);
        std::size_t second = other_distribution(random);
        if(second >= first) ++second;
        const std::array<long double, 2> origin{
            static_cast<long double>(points[first][0]),
            static_cast<long double>(points[first][1])
        };
        long double dx = static_cast<long double>(points[second][0]) - origin[0];
        long double dy = static_cast<long double>(points[second][1]) - origin[1];
        const long double norm = std::hypotl(dx, dy);
        if(!std::isfinite(norm)) throw std::overflow_error("sampled line direction overflow");
        if(norm == 0.0L) continue;
        dx /= norm;
        dy /= norm;
        const std::array<long double, 2> direction{dx, dy};
        std::vector<std::size_t> inliers;
        inliers.reserve(points.size());
        long double error = 0.0L;
        for(std::size_t index = 0; index < points.size(); ++index){
            const std::array<long double, 2> point{
                static_cast<long double>(points[index][0]),
                static_cast<long double>(points[index][1])
            };
            const long double squared_distance = detail::line_squared_distance(
                point, origin, direction
            );
            if(squared_distance <= squared_threshold){
                inliers.push_back(index);
                error += squared_distance;
                if(!std::isfinite(error)) throw std::overflow_error("RANSAC error overflow");
            }
        }
        if(inliers.size() < minimum_inliers) continue;
        if(!best.found || inliers.size() > best.inliers.size()
           || (inliers.size() == best.inliers.size() && error < best.squared_error)){
            best.found = true;
            best.point = origin;
            best.direction = direction;
            best.inliers = std::move(inliers);
            best.squared_error = error;
        }
    }
    if(!best.found) best.squared_error = 0.0L;
    return best;
}

}  // namespace approximate::geometry

#endif  // CPPLIB_SRC_APPROXIMATE_GEOMETRY_RANSAC_LINE_HPP_INCLUDED
