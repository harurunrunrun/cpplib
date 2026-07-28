#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_BIPARTITE_B_MATCHING_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_BIPARTITE_B_MATCHING_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../flow/dinic.hpp"

template<class Capacity>
struct BipartiteBMatchingResult{
    Capacity size;
    std::vector<Capacity> edge_flow;
    std::vector<Capacity> left_degree;
    std::vector<Capacity> right_degree;
};

template<class Capacity = long long>
struct BipartiteBMatching{
    struct Edge{
        int left;
        int right;
        Capacity capacity;
    };

    int left_size;
    int right_size;
    std::vector<Capacity> left_capacity;
    std::vector<Capacity> right_capacity;
    std::vector<Edge> edges;

    BipartiteBMatching(
        std::vector<Capacity> left_capacity_,
        std::vector<Capacity> right_capacity_
    )
        : left_size(static_cast<int>(left_capacity_.size())),
          right_size(static_cast<int>(right_capacity_.size())),
          left_capacity(std::move(left_capacity_)),
          right_capacity(std::move(right_capacity_)){
        for(Capacity value: left_capacity){
            if(value < Capacity(0))[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: negative capacity "
                    "(BipartiteBMatching)."
                );
            }
        }
        for(Capacity value: right_capacity){
            if(value < Capacity(0))[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: negative capacity "
                    "(BipartiteBMatching)."
                );
            }
        }
    }

    int add_edge(
        int left,
        int right,
        Capacity capacity = Capacity(1)
    ){
        if(left < 0 || left_size <= left || right < 0 || right_size <= right
            || capacity < Capacity(0))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(BipartiteBMatching::add_edge)."
            );
        }
        edges.push_back({left, right, capacity});
        return static_cast<int>(edges.size()) - 1;
    }

    BipartiteBMatchingResult<Capacity> solve(
        Capacity limit = std::numeric_limits<Capacity>::max() / Capacity(4)
    ) const{
        if(limit < Capacity(0))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative limit "
                "(BipartiteBMatching::solve)."
            );
        }
        const int source = left_size + right_size;
        const int sink = source + 1;
        Dinic<Capacity> flow(sink + 1);
        for(int left = 0; left < left_size; ++left){
            flow.add_edge(
                source, left, left_capacity[static_cast<std::size_t>(left)]
            );
        }
        std::vector<int> flow_edge_ids;
        flow_edge_ids.reserve(edges.size());
        for(const Edge& edge: edges){
            flow_edge_ids.push_back(flow.add_edge(
                edge.left, left_size + edge.right, edge.capacity
            ));
        }
        for(int right = 0; right < right_size; ++right){
            flow.add_edge(
                left_size + right, sink,
                right_capacity[static_cast<std::size_t>(right)]
            );
        }
        BipartiteBMatchingResult<Capacity> result;
        result.size = flow.max_flow(source, sink, limit);
        result.edge_flow.resize(edges.size());
        result.left_degree.assign(
            static_cast<std::size_t>(left_size), Capacity(0)
        );
        result.right_degree.assign(
            static_cast<std::size_t>(right_size), Capacity(0)
        );
        for(std::size_t index = 0; index < edges.size(); ++index){
            Capacity value = flow.edges[
                static_cast<std::size_t>(flow_edge_ids[index])
            ].flow;
            result.edge_flow[index] = value;
            result.left_degree[
                static_cast<std::size_t>(edges[index].left)
            ] += value;
            result.right_degree[
                static_cast<std::size_t>(edges[index].right)
            ] += value;
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_BIPARTITE_B_MATCHING_HPP_INCLUDED
