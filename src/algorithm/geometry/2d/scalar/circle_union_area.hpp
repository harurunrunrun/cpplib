#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_CIRCLE_UNION_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_CIRCLE_UNION_AREA_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

#include "../predicate/validate_circle.hpp"

inline long double circle_union_area(const std::vector<Circle>& circles){
    for(const Circle& circle: circles) validate_circle(circle);
    const long double pi = std::acos(-1.0L);
    const long double two_pi = 2.0L * pi;
    long double result = 0.0L;

    for(std::size_t index = 0; index < circles.size(); ++index){
        const Circle& current_circle = circles[index];
        const long double radius = current_circle.radius;
        if(radius == 0.0L) continue;

        bool fully_covered = false;
        std::vector<std::pair<long double, long double>> covered_intervals;
        covered_intervals.reserve(circles.size() + 1);
        for(std::size_t other_index = 0;
            other_index < circles.size();
            ++other_index
        ){
            if(index == other_index) continue;
            const Circle& other = circles[other_index];
            if(other.radius == 0.0L) continue;
            const long double difference_x =
                other.center.x - current_circle.center.x;
            const long double difference_y =
                other.center.y - current_circle.center.y;
            const long double distance = std::hypot(difference_x, difference_y);

            const bool same_circle =
                circle_numeric_detail::compare(distance, 0.0L) == 0 &&
                circle_numeric_detail::compare(radius, other.radius) == 0;
            if(same_circle){
                if(other_index < index){
                    fully_covered = true;
                    break;
                }
                continue;
            }
            if(circle_numeric_detail::compare(
                distance + radius, other.radius
            ) <= 0){
                fully_covered = true;
                break;
            }
            if(circle_numeric_detail::compare(
                distance, radius + other.radius
            ) >= 0){
                continue;
            }
            if(circle_numeric_detail::compare(
                distance + other.radius, radius
            ) <= 0){
                continue;
            }

            long double cosine = (
                radius * radius + distance * distance
                - other.radius * other.radius
            ) / (2.0L * radius * distance);
            cosine = std::clamp(cosine, -1.0L, 1.0L);
            const long double half_width = std::acos(cosine);
            const long double center_angle = std::atan2(difference_y, difference_x);
            long double left = center_angle - half_width;
            long double right = center_angle + half_width;
            while(left < 0.0L){
                left += two_pi;
                right += two_pi;
            }
            while(left >= two_pi){
                left -= two_pi;
                right -= two_pi;
            }
            if(right <= two_pi){
                covered_intervals.emplace_back(left, right);
            }else{
                covered_intervals.emplace_back(left, two_pi);
                covered_intervals.emplace_back(0.0L, right - two_pi);
            }
        }
        if(fully_covered) continue;

        std::sort(covered_intervals.begin(), covered_intervals.end());
        const auto add_arc = [&](long double begin, long double end){
            if(end <= begin) return;
            result += 0.5L * (
                radius * current_circle.center.x
                    * (std::sin(end) - std::sin(begin))
                + radius * current_circle.center.y
                    * (std::cos(begin) - std::cos(end))
                + radius * radius * (end - begin)
            );
        };

        long double covered_until = 0.0L;
        for(const auto& [left, right]: covered_intervals){
            if(circle_numeric_detail::compare(left, covered_until) > 0){
                add_arc(covered_until, left);
            }
            if(right > covered_until) covered_until = right;
        }
        if(circle_numeric_detail::compare(covered_until, two_pi) < 0){
            add_arc(covered_until, two_pi);
        }
    }
    return result < 0.0L && geometry_sign(result) == 0 ? 0.0L : result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_CIRCLE_UNION_AREA_HPP_INCLUDED
