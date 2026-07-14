#pragma once

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

namespace circle_polygon_intersection_internal{

inline long double edge_area(Point a, Point b, long double radius){
    const Point direction = b - a;
    const long double aa = norm(direction);
    if(geometry_sign(aa) == 0) return 0.0L;

    std::vector<long double> parameters = {0.0L, 1.0L};
    const long double bb = 2.0L * dot(a, direction);
    const long double cc = norm(a) - radius * radius;
    long double discriminant = bb * bb - 4.0L * aa * cc;
    if(discriminant >= -GEOMETRY_EPS){
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
        if(norm(middle) <= radius * radius + GEOMETRY_EPS){
            result += cross(p, q) / 2.0L;
        }else{
            result += radius * radius * std::atan2(cross(p, q), dot(p, q)) / 2.0L;
        }
    }
    return result;
}

} // namespace circle_polygon_intersection_internal
