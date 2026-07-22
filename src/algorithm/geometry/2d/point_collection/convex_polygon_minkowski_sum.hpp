#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_CONVEX_POLYGON_MINKOWSKI_SUM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_CONVEX_POLYGON_MINKOWSKI_SUM_HPP_INCLUDED

#include <cstddef>
#include <utility>
#include <vector>

#include "normalize_convex_polygon.hpp"

inline std::vector<Point> convex_polygon_minkowski_sum(
    std::vector<Point> first,
    std::vector<Point> second
){
    using namespace advanced_geometry_detail;

    first = normalize_convex_polygon(std::move(first));
    second = normalize_convex_polygon(std::move(second));
    if(first.empty() || second.empty()) return {};
    if(first.size() == 1){
        for(Point& point: second) point += first.front();
        return normalize_convex_polygon(std::move(second));
    }
    if(second.size() == 1){
        for(Point& point: first) point += second.front();
        return normalize_convex_polygon(std::move(first));
    }

    rotate_to_lowest(first);
    rotate_to_lowest(second);
    std::vector<Point> first_edges(first.size());
    std::vector<Point> second_edges(second.size());
    for(std::size_t index = 0; index < first.size(); ++index){
        first_edges[index] = first[(index + 1) % first.size()] - first[index];
    }
    for(std::size_t index = 0; index < second.size(); ++index){
        second_edges[index] = second[(index + 1) % second.size()] - second[index];
    }

    std::vector<Point> result;
    result.reserve(first.size() + second.size() + 1);
    result.push_back(first.front() + second.front());
    std::size_t first_index = 0;
    std::size_t second_index = 0;
    while(first_index < first_edges.size() || second_index < second_edges.size()){
        Point step;
        if(first_index == first_edges.size()){
            step = second_edges[second_index++];
        }else if(second_index == second_edges.size()){
            step = first_edges[first_index++];
        }else if(same_direction(first_edges[first_index], second_edges[second_index])){
            step = first_edges[first_index++] + second_edges[second_index++];
        }else if(direction_less(
            first_edges[first_index],
            second_edges[second_index]
        )){
            step = first_edges[first_index++];
        }else{
            step = second_edges[second_index++];
        }
        result.push_back(result.back() + step);
    }
    if(result.size() >= 2 && point_equal(result.front(), result.back())) result.pop_back();
    return normalize_convex_polygon(std::move(result));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_CONVEX_POLYGON_MINKOWSKI_SUM_HPP_INCLUDED
