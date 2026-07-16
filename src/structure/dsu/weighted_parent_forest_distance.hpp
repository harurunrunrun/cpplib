#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T, int MAX_SIZE>
class WeightedParentForestDistance{
    static_assert(0 <= MAX_SIZE);

    int vertex_count_;
    std::vector<int> parent_;
    std::vector<T> distance_to_parent_;

    static int checked_size(int vertex_count){
        if(vertex_count < 0 || MAX_SIZE < vertex_count)[[unlikely]]{
            throw std::out_of_range(
                "WeightedParentForestDistance size is out of range"
            );
        }
        return vertex_count;
    }

    void check_vertex(int vertex) const{
        if(vertex < 0 || vertex_count_ <= vertex)[[unlikely]]{
            throw std::out_of_range(
                "WeightedParentForestDistance vertex is out of range"
            );
        }
    }

    std::pair<int, T> compress(int vertex){
        std::vector<int> path;
        int current = vertex;
        T total{};
        while(parent_[static_cast<std::size_t>(current)] != current){
            path.push_back(current);
            total = total +
                distance_to_parent_[static_cast<std::size_t>(current)];
            current = parent_[static_cast<std::size_t>(current)];
        }

        const int root_vertex = current;
        T suffix_distance{};
        for(auto iterator = path.rbegin(); iterator != path.rend();
            ++iterator){
            const int path_vertex = *iterator;
            suffix_distance =
                distance_to_parent_[static_cast<std::size_t>(path_vertex)] +
                suffix_distance;
            parent_[static_cast<std::size_t>(path_vertex)] = root_vertex;
            distance_to_parent_[static_cast<std::size_t>(path_vertex)] =
                suffix_distance;
        }
        return {root_vertex, total};
    }

public:
    explicit WeightedParentForestDistance(int vertex_count)
        : vertex_count_(checked_size(vertex_count)),
          parent_(static_cast<std::size_t>(vertex_count_)),
          distance_to_parent_(
              static_cast<std::size_t>(vertex_count_), T{}
          ){
        for(int vertex = 0; vertex < vertex_count_; ++vertex){
            parent_[static_cast<std::size_t>(vertex)] = vertex;
        }
    }

    int size() const{
        return vertex_count_;
    }

    int root(int vertex){
        check_vertex(vertex);
        return compress(vertex).first;
    }

    T distance_to_root(int vertex){
        check_vertex(vertex);
        return compress(vertex).second;
    }

    void link_root(int root_vertex, int parent_vertex, T edge_weight){
        check_vertex(root_vertex);
        check_vertex(parent_vertex);
        if(parent_[static_cast<std::size_t>(root_vertex)] != root_vertex)
            [[unlikely]]{
            throw std::invalid_argument(
                "WeightedParentForestDistance can only link a root"
            );
        }
        const int destination_root = compress(parent_vertex).first;
        if(destination_root == root_vertex)[[unlikely]]{
            throw std::invalid_argument(
                "WeightedParentForestDistance link would create a cycle"
            );
        }
        parent_[static_cast<std::size_t>(root_vertex)] = parent_vertex;
        distance_to_parent_[static_cast<std::size_t>(root_vertex)] =
            std::move(edge_weight);
    }
};
