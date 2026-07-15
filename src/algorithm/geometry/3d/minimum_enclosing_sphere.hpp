#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <vector>

#include "base.hpp"
#include "cross.hpp"
#include "is_finite.hpp"
#include "norm.hpp"
#include "scalar_triple.hpp"

namespace minimum_enclosing_sphere_detail{

inline bool finite_point(const Point3& point){
    return geometry3d_is_finite(point);
}

inline long double stable_length(const Point3& point){
    return std::hypot(point.x, point.y, point.z);
}

inline bool is_zero(const Geometry3DNormalizedDifference& difference){
    return difference.value.x == 0.0L && difference.value.y == 0.0L
        && difference.value.z == 0.0L;
}

inline long double common_scale(
    std::initializer_list<Geometry3DNormalizedDifference> differences
){
    long double result = 0.0L;
    for(const Geometry3DNormalizedDifference& difference: differences){
        if(!is_zero(difference)){
            result = std::max(result, difference.scale);
        }
    }
    return result;
}

inline Point3 rescale(
    const Geometry3DNormalizedDifference& difference,
    long double scale
){
    return difference.value * (difference.scale / scale);
}

inline std::optional<long double> scaled_length(
    const Point3& normalized,
    long double scale,
    long double divisor = 1.0L
){
    const long double factor = stable_length(normalized) / divisor;
    if(!std::isfinite(factor) || factor < 0.0L
        || factor > std::numeric_limits<long double>::max() / scale){
        return std::nullopt;
    }
    const long double result = factor * scale;
    if(!std::isfinite(result) || (factor != 0.0L && result == 0.0L)){
        return std::nullopt;
    }
    return result;
}

inline std::optional<Point3> restore_offset(
    const Point3& anchor,
    const Point3& normalized_offset,
    long double scale
){
    const Point3 result{
        std::fma(normalized_offset.x, scale, anchor.x),
        std::fma(normalized_offset.y, scale, anchor.y),
        std::fma(normalized_offset.z, scale, anchor.z),
    };
    if(!finite_point(result)) return std::nullopt;
    return result;
}

inline bool contains(const Sphere3& sphere, const Point3& point){
    const Geometry3DNormalizedDifference difference =
        geometry3d_normalized_difference(point, sphere.center);
    if(is_zero(difference)) return true;
    const long double scale = std::max(sphere.radius, difference.scale);
    const long double distance = stable_length(difference.value)
        * (difference.scale / scale);
    const long double radius = sphere.radius / scale;
    const long double characteristic = std::max(distance, radius);
    const long double tolerance = (
        GEOMETRY3D_EPS
        + 32.0L * std::numeric_limits<long double>::epsilon()
    ) * characteristic;
    return distance <= radius + tolerance;
}

inline std::optional<Sphere3> sphere_through_two(
    const Point3& first,
    const Point3& second
){
    const Geometry3DNormalizedDifference difference =
        geometry3d_normalized_difference(second, first);
    const Point3 center{
        first.x / 2.0L + second.x / 2.0L,
        first.y / 2.0L + second.y / 2.0L,
        first.z / 2.0L + second.z / 2.0L,
    };
    const std::optional<long double> radius = scaled_length(
        difference.value, difference.scale, 2.0L
    );
    if(!finite_point(center) || !radius) return std::nullopt;
    return Sphere3{center, *radius};
}

inline std::optional<Sphere3> sphere_through_three(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    const Geometry3DNormalizedDifference first_difference =
        geometry3d_normalized_difference(second, first);
    const Geometry3DNormalizedDifference second_difference =
        geometry3d_normalized_difference(third, first);
    const long double scale = common_scale({
        first_difference, second_difference,
    });
    if(scale == 0.0L) return std::nullopt;
    const Point3 u = rescale(first_difference, scale);
    const Point3 v = rescale(second_difference, scale);
    const Point3 normal = cross(u, v);
    const long double denominator = 2 * norm(normal);
    if(denominator == 0 || !std::isfinite(denominator)) return std::nullopt;
    const Point3 scaled_offset = (
        norm(u) * cross(v, normal) + norm(v) * cross(normal, u)
    ) / denominator;
    const std::optional<Point3> center =
        restore_offset(first, scaled_offset, scale);
    const std::optional<long double> radius =
        scaled_length(scaled_offset, scale);
    if(!center || !radius) return std::nullopt;
    return Sphere3{*center, *radius};
}

inline std::optional<Sphere3> sphere_through_four(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    const Point3& fourth
){
    const Geometry3DNormalizedDifference first_difference =
        geometry3d_normalized_difference(second, first);
    const Geometry3DNormalizedDifference second_difference =
        geometry3d_normalized_difference(third, first);
    const Geometry3DNormalizedDifference third_difference =
        geometry3d_normalized_difference(fourth, first);
    const long double scale = common_scale({
        first_difference, second_difference, third_difference,
    });
    if(scale == 0.0L) return std::nullopt;
    const Point3 u = rescale(first_difference, scale);
    const Point3 v = rescale(second_difference, scale);
    const Point3 w = rescale(third_difference, scale);
    const long double denominator = 2 * scalar_triple(u, v, w);
    if(denominator == 0 || !std::isfinite(denominator)) return std::nullopt;
    const Point3 scaled_offset = (
        norm(u) * cross(v, w) +
        norm(v) * cross(w, u) +
        norm(w) * cross(u, v)
    ) / denominator;
    const std::optional<Point3> center =
        restore_offset(first, scaled_offset, scale);
    const std::optional<long double> radius =
        scaled_length(scaled_offset, scale);
    if(!center || !radius) return std::nullopt;
    return Sphere3{*center, *radius};
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
