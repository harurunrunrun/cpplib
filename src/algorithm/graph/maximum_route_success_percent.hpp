#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "maximum_probability_path.hpp"

struct RouteSuccessPercentEdge{
    int left;
    int right;
    int percent;
};

inline double maximum_route_success_percent(
    int vertex_count,
    const std::vector<RouteSuccessPercentEdge>& edges
){
    if(vertex_count <= 0)[[unlikely]]{
        throw std::invalid_argument(
            "maximum_route_success_percent nonpositive vertex count"
        );
    }
    std::vector<std::vector<MaximumProbabilityPathEdge<double>>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(const auto [left, right, percent]: edges){
        if(left < 0 || vertex_count <= left || right < 0 ||
           vertex_count <= right)[[unlikely]]{
            throw std::out_of_range(
                "maximum_route_success_percent endpoint out of range"
            );
        }
        if(percent < 0 || 100 < percent)[[unlikely]]{
            throw std::invalid_argument(
                "maximum_route_success_percent invalid percentage"
            );
        }
        const double probability = static_cast<double>(percent) / 100.0;
        graph[static_cast<std::size_t>(left)].push_back({right, probability});
        graph[static_cast<std::size_t>(right)].push_back({left, probability});
    }
    const auto result = maximum_probability_path(graph, 0);
    return result.probability[static_cast<std::size_t>(vertex_count - 1)] * 100.0;
}
