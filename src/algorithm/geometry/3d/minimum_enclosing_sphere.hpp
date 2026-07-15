#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <vector>

#include "base.hpp"
#include "cross.hpp"
#include "norm.hpp"
#include "scalar_triple.hpp"

namespace minimum_enclosing_sphere_detail{

inline bool finite_point(const Point3& point){
    return std::isfinite(point.x) && std::isfinite(point.y) &&
        std::isfinite(point.z);
}

inline long double stable_length(const Point3& point){
    return std::hypot(point.x, point.y, point.z);
}

inline bool contains(const Sphere3& sphere, const Point3& point){
    const long double point_distance = stable_length(point - sphere.center);
    const long double scale = std::max({
        1.0L, sphere.radius, point_distance,
    });
    const long double tolerance = GEOMETRY3D_EPS +
        32 * std::numeric_limits<long double>::epsilon() * scale;
    return std::isfinite(point_distance) &&
        point_distance <= sphere.radius + tolerance;
}

inline std::optional<Sphere3> sphere_through_two(
    const Point3& first,
    const Point3& second
){
    const Point3 difference = second - first;
    const Point3 center = first + difference / 2;
    const long double radius = stable_length(difference) / 2;
    if(!finite_point(center) || !std::isfinite(radius)) return std::nullopt;
    return Sphere3{center, radius};
}

inline long double coordinate_scale(
    const Point3& first,
    const Point3& second,
    const Point3& third = {}
){
    return std::max({
        std::abs(first.x), std::abs(first.y), std::abs(first.z),
        std::abs(second.x), std::abs(second.y), std::abs(second.z),
        std::abs(third.x), std::abs(third.y), std::abs(third.z),
    });
}

inline std::optional<Sphere3> sphere_through_three(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    const Point3 first_vector = second - first;
    const Point3 second_vector = third - first;
    const long double scale = coordinate_scale(first_vector, second_vector);
    if(scale == 0 || !std::isfinite(scale)) return std::nullopt;
    const Point3 u = first_vector / scale;
    const Point3 v = second_vector / scale;
    const Point3 normal = cross(u, v);
    const long double denominator = 2 * norm(normal);
    if(denominator == 0 || !std::isfinite(denominator)) return std::nullopt;
    const Point3 scaled_offset = (
        norm(u) * cross(v, normal) + norm(v) * cross(normal, u)
    ) / denominator;
    const Point3 offset = scaled_offset * scale;
    const Point3 center = first + offset;
    const long double radius = stable_length(offset);
    if(!finite_point(center) || !std::isfinite(radius)) return std::nullopt;
    return Sphere3{center, radius};
}

inline std::optional<Sphere3> sphere_through_four(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    const Point3& fourth
){
    const Point3 first_vector = second - first;
    const Point3 second_vector = third - first;
    const Point3 third_vector = fourth - first;
    const long double scale = coordinate_scale(
        first_vector, second_vector, third_vector
    );
    if(scale == 0 || !std::isfinite(scale)) return std::nullopt;
    const Point3 u = first_vector / scale;
    const Point3 v = second_vector / scale;
    const Point3 w = third_vector / scale;
    const long double denominator = 2 * scalar_triple(u, v, w);
    if(denominator == 0 || !std::isfinite(denominator)) return std::nullopt;
    const Point3 scaled_offset = (
        norm(u) * cross(v, w) +
        norm(v) * cross(w, u) +
        norm(w) * cross(u, v)
    ) / denominator;
    const Point3 offset = scaled_offset * scale;
    const Point3 center = first + offset;
    const long double radius = stable_length(offset);
    if(!finite_point(center) || !std::isfinite(radius)) return std::nullopt;
    return Sphere3{center, radius};
}

inline Sphere3 support_sphere(const Point3* points, std::size_t size){
    Sphere3 best{{}, std::numeric_limits<long double>::infinity()};
    bool found = false;
    const auto consider = [&](const std::optional<Sphere3>& candidate){
        if(!candidate) return;
        for(std::size_t index = 0; index < size; ++index){
            if(!minimum_enclosing_sphere_detail::contains(*candidate, points[index])) return;
        }
        if(!found || candidate->radius < best.radius){
            best = *candidate;
            found = true;
        }
    };

    for(std::size_t first = 0; first < size; ++first){
        consider(Sphere3{points[first], 0});
    }
    for(std::size_t first = 0; first < size; ++first){
        for(std::size_t second = first + 1; second < size; ++second){
            consider(sphere_through_two(points[first], points[second]));
        }
    }
    for(std::size_t first = 0; first < size; ++first){
        for(std::size_t second = first + 1; second < size; ++second){
            for(std::size_t third = second + 1; third < size; ++third){
                consider(sphere_through_three(
                    points[first], points[second], points[third]
                ));
            }
        }
    }
    if(size == 4){
        consider(sphere_through_four(points[0], points[1], points[2], points[3]));
    }
    if(!found)[[unlikely]]{
        throw std::overflow_error("could not construct a finite support sphere");
    }
    return best;
}

inline void validate_points(const Point3* points, std::size_t size){
    for(std::size_t index = 0; index < size; ++index){
        if(!finite_point(points[index]))[[unlikely]]{
            throw std::invalid_argument("minimum enclosing sphere requires finite points");
        }
    }
}

} // namespace minimum_enclosing_sphere_detail

inline Sphere3 minimum_enclosing_sphere(const Point3& point){
    minimum_enclosing_sphere_detail::validate_points(&point, 1);
    return {point, 0};
}

inline Sphere3 minimum_enclosing_sphere(
    const Point3& first,
    const Point3& second
){
    const std::array<Point3, 2> points{{first, second}};
    minimum_enclosing_sphere_detail::validate_points(points.data(), points.size());
    return minimum_enclosing_sphere_detail::support_sphere(
        points.data(), points.size()
    );
}

inline Sphere3 minimum_enclosing_sphere(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    const std::array<Point3, 3> points{{first, second, third}};
    minimum_enclosing_sphere_detail::validate_points(points.data(), points.size());
    return minimum_enclosing_sphere_detail::support_sphere(
        points.data(), points.size()
    );
}

inline Sphere3 minimum_enclosing_sphere(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    const Point3& fourth
){
    const std::array<Point3, 4> points{{first, second, third, fourth}};
    minimum_enclosing_sphere_detail::validate_points(points.data(), points.size());
    return minimum_enclosing_sphere_detail::support_sphere(
        points.data(), points.size()
    );
}

inline Sphere3 minimum_enclosing_sphere(
    std::vector<Point3> points,
    std::uint64_t seed = 0x9E3779B97F4A7C15ULL
){
    if(points.empty())[[unlikely]]{
        throw std::invalid_argument("minimum enclosing sphere requires a point");
    }
    minimum_enclosing_sphere_detail::validate_points(points.data(), points.size());
    std::mt19937_64 random(seed);
    std::shuffle(points.begin(), points.end(), random);

    Sphere3 sphere = minimum_enclosing_sphere(points[0]);
    for(std::size_t first = 1; first < points.size(); ++first){
        if(minimum_enclosing_sphere_detail::contains(sphere, points[first])) continue;
        sphere = minimum_enclosing_sphere(points[first]);
        for(std::size_t second = 0; second < first; ++second){
            if(minimum_enclosing_sphere_detail::contains(sphere, points[second])){
                continue;
            }
            sphere = minimum_enclosing_sphere(points[first], points[second]);
            for(std::size_t third = 0; third < second; ++third){
                if(minimum_enclosing_sphere_detail::contains(sphere, points[third])){
                    continue;
                }
                sphere = minimum_enclosing_sphere(
                    points[first], points[second], points[third]
                );
                for(std::size_t fourth = 0; fourth < third; ++fourth){
                    if(minimum_enclosing_sphere_detail::contains(
                        sphere, points[fourth]
                    )) continue;
                    sphere = minimum_enclosing_sphere(
                        points[first], points[second],
                        points[third], points[fourth]
                    );
                }
            }
        }
    }
    return sphere;
}
