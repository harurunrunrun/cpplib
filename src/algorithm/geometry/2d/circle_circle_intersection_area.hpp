#pragma once

#include <algorithm>
#include <cmath>

#include "validate_circle.hpp"

inline long double circle_circle_intersection_area(
    const Circle& first,
    const Circle& second
){
    validate_circle(first);
    validate_circle(second);

    const long double first_radius = first.radius;
    const long double second_radius = second.radius;
    const long double smaller_radius = std::min(first_radius, second_radius);
    if(smaller_radius == 0.0L) return 0.0L;

    const Point center_difference = first.center - second.center;
    const long double center_distance =
        std::hypot(center_difference.x, center_difference.y);
    const long double radius_sum = first_radius + second_radius;
    const long double radius_difference =
        std::abs(first_radius - second_radius);
    const long double pi = std::acos(-1.0L);
    if(center_distance >= radius_sum) return 0.0L;
    if(center_distance == 0.0L || center_distance <= radius_difference){
        return pi * smaller_radius * smaller_radius;
    }

    const auto segment_area = [](long double radius, long double cosine){
        cosine = std::clamp(cosine, -1.0L, 1.0L);
        const long double angle = std::acos(cosine);
        long double factor;
        if(angle < 1e-4L){
            const long double angle_squared = angle * angle;
            factor = angle * angle_squared * (
                2.0L / 3.0L + angle_squared * (
                    -2.0L / 15.0L + angle_squared * (4.0L / 315.0L)
                )
            );
        }else{
            const long double sine = std::sqrt(std::max(
                0.0L,
                (1.0L - cosine) * (1.0L + cosine)
            ));
            factor = angle - cosine * sine;
        }
        return radius * radius * factor;
    };

    const long double distance_squared = center_distance * center_distance;
    const long double first_cosine = (
        distance_squared +
        (first_radius - second_radius) * (first_radius + second_radius)
    ) / (2.0L * center_distance * first_radius);
    const long double second_cosine = (
        distance_squared +
        (second_radius - first_radius) * (second_radius + first_radius)
    ) / (2.0L * center_distance * second_radius);
    return segment_area(first_radius, first_cosine) +
        segment_area(second_radius, second_cosine);
}
