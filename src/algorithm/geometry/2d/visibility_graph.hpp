#pragma once

#include <cmath>
#include <cstddef>
#include <vector>

#include "contains.hpp"
#include "intersect.hpp"
#include "norm.hpp"
#include "on_segment.hpp"
#include "types.hpp"
#include "visibility_polygon.hpp"

struct VisibilityGraphEdge{
    std::size_t first;
    std::size_t second;
    long double distance;
};

inline std::vector<VisibilityGraphEdge> visibility_graph(
    const std::vector<Point>& polygon
){
    visibility_polygon_detail::validate_simple_polygon(polygon);
    const std::size_t size = polygon.size();
    std::vector<VisibilityGraphEdge> result;
    result.reserve(size * (size - 1) / 2);
    for(std::size_t first = 0; first < size; ++first){
        for(std::size_t second = first + 1; second < size; ++second){
            const bool adjacent = second == first + 1
                || (first == 0 && second + 1 == size);
            bool visible = adjacent;
            const Segment sight{polygon[first], polygon[second]};
            if(!adjacent){
                visible = true;
                for(std::size_t vertex = 0; vertex < size; ++vertex){
                    if(vertex != first && vertex != second
                        && on_segment(sight, polygon[vertex])){
                        visible = false;
                        break;
                    }
                }
                for(std::size_t edge = 0; visible && edge < size; ++edge){
                    const std::size_t next = (edge + 1) % size;
                    if(edge == first || next == first
                        || edge == second || next == second){
                        continue;
                    }
                    if(intersect(
                        sight, {polygon[edge], polygon[next]}
                    )){
                        visible = false;
                    }
                }
                if(visible){
                    const Point midpoint =
                        (polygon[first] + polygon[second]) / 2.0L;
                    visible = contains(polygon, midpoint) != 0;
                }
            }
            if(visible){
                result.push_back({
                    first,
                    second,
                    std::sqrt(norm(polygon[second] - polygon[first]))
                });
            }
        }
    }
    return result;
}
