#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POWER_DIAGRAM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POWER_DIAGRAM_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../detail/numeric/plus_geometry_detail.hpp"
#include "../../type/definition/power_diagram_result.hpp"
#include "../../type/definition/weighted_point.hpp"

namespace power_diagram_detail{

inline std::vector<Point> clip(
    const std::vector<Point>& polygon,
    const Point& normal,
    long double limit
){
    std::vector<Point> result;
    if(polygon.empty()) return result;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Point first = polygon[index];
        const Point second = polygon[(index + 1) % polygon.size()];
        const long double first_value = dot(normal, first) - limit;
        const long double second_value = dot(normal, second) - limit;
        const bool first_inside =
            plus_geometry_detail::sign(first_value, std::abs(limit)) <= 0;
        const bool second_inside =
            plus_geometry_detail::sign(second_value, std::abs(limit)) <= 0;
        if(first_inside) result.push_back(first);
        if(first_inside == second_inside) continue;
        const long double denominator = first_value - second_value;
        if(plus_geometry_detail::sign(
            denominator,
            std::abs(first_value) + std::abs(second_value)
        ) == 0){
            continue;
        }
        const long double parameter = first_value / denominator;
        result.push_back(first + (second - first) * parameter);
    }
    return plus_geometry_detail::normalized_polygon(std::move(result));
}

}  // namespace power_diagram_detail

inline PowerDiagramResult power_diagram(
    const std::vector<WeightedPoint>& sites,
    std::vector<Point> bounding_polygon
){
    bounding_polygon =
        plus_geometry_detail::normalized_polygon(std::move(bounding_polygon));
    if(bounding_polygon.size() < 3){
        throw std::invalid_argument(
            "a power diagram requires a nondegenerate bounding polygon"
        );
    }
    for(const Point& point: bounding_polygon){
        plus_geometry_detail::validate_finite(point);
    }
    for(const WeightedPoint& site: sites){
        plus_geometry_detail::validate_finite(site.point);
        if(!std::isfinite(site.weight)){
            throw std::invalid_argument("power weights must be finite");
        }
    }

    PowerDiagramResult result;
    result.cells.resize(sites.size());
    for(std::size_t first = 0; first < sites.size(); ++first){
        std::vector<Point> cell = bounding_polygon;
        for(std::size_t second = 0;
            second < sites.size() && !cell.empty();
            ++second){
            if(first == second) continue;
            const Point normal =
                (sites[second].point - sites[first].point) * 2.0L;
            const long double limit =
                dot(sites[second].point, sites[second].point)
                - sites[second].weight
                - dot(sites[first].point, sites[first].point)
                + sites[first].weight;
            if(plus_geometry_detail::sign(
                dot(normal, normal)
            ) == 0){
                if(plus_geometry_detail::sign(limit) < 0) cell.clear();
                continue;
            }
            cell = power_diagram_detail::clip(cell, normal, limit);
        }
        result.cells[first] = std::move(cell);
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POWER_DIAGRAM_HPP_INCLUDED
