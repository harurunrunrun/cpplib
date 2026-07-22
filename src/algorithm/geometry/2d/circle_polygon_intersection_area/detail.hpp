#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_POLYGON_INTERSECTION_AREA_DETAIL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_POLYGON_INTERSECTION_AREA_DETAIL_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

#include "../constants.hpp"
#include "../cross.hpp"
#include "../dot.hpp"
#include "../geometry_sign.hpp"
#include "../norm.hpp"
#include "../types.hpp"
#include "../validate_circle.hpp"

namespace circle_polygon_intersection_internal{

inline long double edge_area(Point a, Point b, long double radius){
    const Point direction = b - a;
    const long double aa = norm(direction);
    if(aa == 0.0L) return 0.0L;

    std::vector<long double> parameters = {0.0L, 1.0L};
    const long double bb = 2.0L * dot(a, direction);
    const long double cc = norm(a) - radius * radius;
    long double discriminant = bb * bb - 4.0L * aa * cc;
    const int discriminant_sign = circle_numeric_detail::scaled_sign(
        discriminant,
        bb * bb + std::fabs(4.0L * aa * cc)
    );
    if(discriminant_sign >= 0){
        discriminant = std::max(0.0L, discriminant);
        const long double root = std::sqrt(discriminant);
        const long double first = (-bb - root) / (2.0L * aa);
        const long double second = (-bb + root) / (2.0L * aa);
        if(GEOMETRY_EPS < first && first < 1.0L - GEOMETRY_EPS){
            parameters.push_back(first);
        }
        if(GEOMETRY_EPS < second && second < 1.0L - GEOMETRY_EPS){
            parameters.push_back(second);
        }
    }
    std::sort(parameters.begin(), parameters.end());
    parameters.erase(std::unique(parameters.begin(), parameters.end(), [](long double x, long double y){
        return std::abs(x - y) <= GEOMETRY_EPS;
    }), parameters.end());

    long double result = 0.0L;
    for(std::size_t i = 0; i + 1 < parameters.size(); i++){
        const long double left = parameters[i];
        const long double right = parameters[i + 1];
        const Point p = a + direction * left;
        const Point q = a + direction * right;
        const Point middle = a + direction * ((left + right) / 2.0L);
        const long double middle_squared = norm(middle);
        const long double radius_squared = radius * radius;
        if(circle_numeric_detail::scaled_sign(
            middle_squared - radius_squared, middle_squared + radius_squared
        ) <= 0){
            result += cross(p, q) / 2.0L;
        }else{
            result += radius * radius * std::atan2(cross(p, q), dot(p, q)) / 2.0L;
        }
    }
    return result;
}

} // namespace circle_polygon_intersection_internal

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_POLYGON_INTERSECTION_AREA_DETAIL_HPP_INCLUDED
