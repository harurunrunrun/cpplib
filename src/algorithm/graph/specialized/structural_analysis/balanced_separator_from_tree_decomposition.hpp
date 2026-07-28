#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_STRUCTURAL_ANALYSIS_BALANCED_SEPARATOR_FROM_TREE_DECOMPOSITION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_STRUCTURAL_ANALYSIS_BALANCED_SEPARATOR_FROM_TREE_DECOMPOSITION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

struct TreeDecompositionBalancedSeparator{
    int bag = -1;
    std::vector<int> vertices;
    long long maximum_component_weight = 0;
};

inline TreeDecompositionBalancedSeparator
balanced_separator_from_tree_decomposition(
    const std::vector<std::vector<int>>& bags,
    const std::vector<std::pair<int, int>>& bag_edges,
    const std::vector<long long>& vertex_weights
){
    const int bag_count = static_cast<int>(bags.size());
    const int vertex_count = static_cast<int>(vertex_weights.size());
    if(bag_count == 0){
        if(!bag_edges.empty() || vertex_count != 0)[[unlikely]]{
            throw std::invalid_argument(
                "a nonempty decomposition requires at least one bag"
            );
        }
        return {};
    }
    if(bag_edges.size() != bags.size() - 1)[[unlikely]]{
        throw std::invalid_argument("the bag graph must be a tree");
    }
    for(const long long weight: vertex_weights){
        if(weight < 0)[[unlikely]]{
            throw std::invalid_argument(
                "balanced separator weights must be nonnegative"
            );
        }
    }

    std::vector<std::vector<int>> tree(bags.size());
    for(const auto [first, second]: bag_edges){
        if(first < 0 || bag_count <= first ||
           second < 0 || bag_count <= second ||
           first == second)[[unlikely]]{
            throw std::out_of_range("bag edge endpoint is out of range");
        }
        tree[static_cast<std::size_t>(first)].push_back(second);
        tree[static_cast<std::size_t>(second)].push_back(first);
    }

    std::vector<int> parent(bags.size(), -2);
    std::vector<int> order;
    order.reserve(bags.size());
    parent[0] = -1;
    order.push_back(0);
    for(std::size_t index = 0; index < order.size(); ++index){
        const int bag = order[index];
        for(const int next: tree[static_cast<std::size_t>(bag)]){
            if(next == parent[static_cast<std::size_t>(bag)]) continue;
            if(parent[static_cast<std::size_t>(next)] != -2)[[unlikely]]{
                throw std::invalid_argument("the bag graph contains a cycle");
            }
            parent[static_cast<std::size_t>(next)] = bag;
            order.push_back(next);
        }
    }
    if(order.size() != bags.size())[[unlikely]]{
        throw std::invalid_argument("the bag graph is disconnected");
    }

    std::vector<int> owner(vertex_weights.size(), -1);
    std::vector<long long> owned_weight(bags.size());
    for(int bag = 0; bag < bag_count; ++bag){
        std::vector<int> unique_vertices = bags[static_cast<std::size_t>(bag)];
        std::sort(unique_vertices.begin(), unique_vertices.end());
        if(std::adjacent_find(
            unique_vertices.begin(), unique_vertices.end()
        ) != unique_vertices.end())[[unlikely]]{
            throw std::invalid_argument("a bag contains a duplicate vertex");
        }
        for(const int vertex: unique_vertices){
            if(vertex < 0 || vertex_count <= vertex)[[unlikely]]{
                throw std::out_of_range("bag vertex is out of range");
            }
            if(owner[static_cast<std::size_t>(vertex)] == -1){
                owner[static_cast<std::size_t>(vertex)] = bag;
                if(owned_weight[static_cast<std::size_t>(bag)] >
                   std::numeric_limits<long long>::max() -
                       vertex_weights[static_cast<std::size_t>(vertex)])[[unlikely]]{
                    throw std::overflow_error("separator weight sum overflow");
                }
                owned_weight[static_cast<std::size_t>(bag)] +=
                    vertex_weights[static_cast<std::size_t>(vertex)];
            }
        }
    }
    for(const int bag: owner){
        if(bag == -1)[[unlikely]]{
            throw std::invalid_argument(
                "every weighted vertex must occur in a bag"
            );
        }
    }

    std::vector<long long> subtree_weight = owned_weight;
    for(auto iterator = order.rbegin(); iterator != order.rend(); ++iterator){
        const int bag = *iterator;
        const int ancestor = parent[static_cast<std::size_t>(bag)];
        if(ancestor < 0) continue;
        if(subtree_weight[static_cast<std::size_t>(ancestor)] >
           std::numeric_limits<long long>::max() -
               subtree_weight[static_cast<std::size_t>(bag)])[[unlikely]]{
            throw std::overflow_error("separator weight sum overflow");
        }
        subtree_weight[static_cast<std::size_t>(ancestor)] +=
            subtree_weight[static_cast<std::size_t>(bag)];
    }
    const long long total_weight = subtree_weight[0];

    int separator_bag = 0;
    while(true){
        int heavy_neighbor = -1;
        for(const int next: tree[static_cast<std::size_t>(separator_bag)]){
            const long long component_weight =
                parent[static_cast<std::size_t>(next)] == separator_bag
                ? subtree_weight[static_cast<std::size_t>(next)]
                : total_weight -
                    subtree_weight[static_cast<std::size_t>(separator_bag)];
            if(component_weight > total_weight / 2){
                heavy_neighbor = next;
                break;
            }
        }
        if(heavy_neighbor == -1) break;
        separator_bag = heavy_neighbor;
    }

    TreeDecompositionBalancedSeparator result;
    result.bag = separator_bag;
    result.vertices = bags[static_cast<std::size_t>(separator_bag)];
    std::sort(result.vertices.begin(), result.vertices.end());

    std::vector<unsigned char> is_separator(vertex_weights.size());
    for(const int vertex: result.vertices){
        is_separator[static_cast<std::size_t>(vertex)] = 1;
    }
    std::vector<int> component(bags.size(), -1);
    int component_count = 0;
    for(const int first: tree[static_cast<std::size_t>(separator_bag)]){
        std::vector<int> stack{first};
        component[static_cast<std::size_t>(first)] = component_count;
        while(!stack.empty()){
            const int bag = stack.back();
            stack.pop_back();
            for(const int next: tree[static_cast<std::size_t>(bag)]){
                if(next == separator_bag ||
                   component[static_cast<std::size_t>(next)] != -1){
                    continue;
                }
                component[static_cast<std::size_t>(next)] = component_count;
                stack.push_back(next);
            }
        }
        ++component_count;
    }
    std::vector<long long> component_weight(
        static_cast<std::size_t>(component_count)
    );
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(is_separator[static_cast<std::size_t>(vertex)]) continue;
        const int owner_bag = owner[static_cast<std::size_t>(vertex)];
        const int identifier = component[static_cast<std::size_t>(owner_bag)];
        if(identifier < 0)[[unlikely]]{
            throw std::logic_error("nonseparator vertex owned by separator bag");
        }
        component_weight[static_cast<std::size_t>(identifier)] +=
            vertex_weights[static_cast<std::size_t>(vertex)];
    }
    for(const long long weight: component_weight){
        result.maximum_component_weight =
            std::max(result.maximum_component_weight, weight);
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_STRUCTURAL_ANALYSIS_BALANCED_SEPARATOR_FROM_TREE_DECOMPOSITION_HPP_INCLUDED
