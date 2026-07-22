#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_RADIUS_BALLS_FORM_VERTEX_PARTITION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_RADIUS_BALLS_FORM_VERTEX_PARTITION_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

struct RadiusBall{
    int center;
    int radius;
};

inline bool radius_balls_form_vertex_partition(
    int vertex_count,
    const std::vector<std::pair<int, int>>& undirected_edges,
    const std::vector<RadiusBall>& balls
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::invalid_argument(
            "radius_balls_form_vertex_partition negative vertex count"
        );
    }
    if(balls.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error(
            "radius_balls_form_vertex_partition has too many balls"
        );
    }

    std::vector<std::vector<int>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(const auto [left, right]: undirected_edges){
        if(left < 0 || vertex_count <= left ||
           right < 0 || vertex_count <= right)[[unlikely]]{
            throw std::out_of_range(
                "radius_balls_form_vertex_partition edge is out of range"
            );
        }
        graph[static_cast<std::size_t>(left)].push_back(right);
        graph[static_cast<std::size_t>(right)].push_back(left);
    }

    struct SearchState{
        int vertex;
        int remaining_radius;
        int ball_id;
    };
    std::vector<int> owner(static_cast<std::size_t>(vertex_count), -1);
    std::queue<SearchState> queue;
    for(int ball_id = 0; ball_id < static_cast<int>(balls.size());
        ++ball_id){
        const auto [center, radius] =
            balls[static_cast<std::size_t>(ball_id)];
        if(center < 0 || vertex_count <= center)[[unlikely]]{
            throw std::out_of_range(
                "radius_balls_form_vertex_partition center is out of range"
            );
        }
        if(radius < 0)[[unlikely]]{
            throw std::invalid_argument(
                "radius_balls_form_vertex_partition negative radius"
            );
        }
        if(owner[static_cast<std::size_t>(center)] != -1) return false;
        owner[static_cast<std::size_t>(center)] = ball_id;
        queue.push({center, radius, ball_id});
    }

    while(!queue.empty()){
        const auto [vertex, remaining_radius, ball_id] = queue.front();
        queue.pop();
        if(remaining_radius == 0) continue;
        for(const int next: graph[static_cast<std::size_t>(vertex)]){
            int& next_owner = owner[static_cast<std::size_t>(next)];
            if(next_owner == ball_id) continue;
            if(next_owner != -1) return false;
            next_owner = ball_id;
            queue.push({next, remaining_radius - 1, ball_id});
        }
    }

    for(const int ball_id: owner){
        if(ball_id == -1) return false;
    }
    return true;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_RADIUS_BALLS_FORM_VERTEX_PARTITION_HPP_INCLUDED
