#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_TREE_DECOMPOSITION_WIDTH_TWO_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_TREE_DECOMPOSITION_WIDTH_TWO_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <deque>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

struct TreeDecompositionWidthTwoResult{
    bool valid = false;
    std::vector<std::vector<int>> bags;
    std::vector<std::pair<int, int>> tree_edges;

    int width() const noexcept{
        if(!valid || bags.empty()) return valid ? -1 : 0;
        std::size_t maximum = 0;
        for(const auto& bag: bags) maximum = std::max(maximum, bag.size());
        return static_cast<int>(maximum) - 1;
    }
};

inline TreeDecompositionWidthTwoResult tree_decomposition_width_two(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::invalid_argument(
            "tree_decomposition_width_two: negative vertex count"
        );
    }
    std::vector<std::set<int>> adjacency(
        static_cast<std::size_t>(vertex_count)
    );
    for(const auto [first, second]: edges){
        if(first < 0 || vertex_count <= first ||
           second < 0 || vertex_count <= second || first == second)[[unlikely]]{
            throw std::invalid_argument(
                "tree_decomposition_width_two: invalid edge"
            );
        }
        adjacency[static_cast<std::size_t>(first)].insert(second);
        adjacency[static_cast<std::size_t>(second)].insert(first);
    }

    std::deque<int> candidates;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(adjacency[static_cast<std::size_t>(vertex)].size() <= 2){
            candidates.push_back(vertex);
        }
    }
    std::vector<unsigned char> removed(
        static_cast<std::size_t>(vertex_count), 0
    );
    std::vector<int> order;
    std::vector<std::vector<int>> later_neighbors;
    order.reserve(static_cast<std::size_t>(vertex_count));
    later_neighbors.reserve(static_cast<std::size_t>(vertex_count));

    while(!candidates.empty()){
        const int vertex = candidates.front();
        candidates.pop_front();
        if(removed[static_cast<std::size_t>(vertex)] ||
           adjacency[static_cast<std::size_t>(vertex)].size() > 2){
            continue;
        }
        std::vector<int> neighbors(
            adjacency[static_cast<std::size_t>(vertex)].begin(),
            adjacency[static_cast<std::size_t>(vertex)].end()
        );
        order.push_back(vertex);
        later_neighbors.push_back(neighbors);
        removed[static_cast<std::size_t>(vertex)] = 1;

        for(const int neighbor: neighbors){
            adjacency[static_cast<std::size_t>(neighbor)].erase(vertex);
        }
        adjacency[static_cast<std::size_t>(vertex)].clear();
        if(neighbors.size() == 2 && neighbors[0] != neighbors[1]){
            adjacency[static_cast<std::size_t>(neighbors[0])].insert(neighbors[1]);
            adjacency[static_cast<std::size_t>(neighbors[1])].insert(neighbors[0]);
        }
        for(const int neighbor: neighbors){
            if(adjacency[static_cast<std::size_t>(neighbor)].size() <= 2){
                candidates.push_back(neighbor);
            }
        }
    }

    if(order.size() != static_cast<std::size_t>(vertex_count)){
        return {};
    }

    TreeDecompositionWidthTwoResult result;
    result.valid = true;
    result.bags.resize(static_cast<std::size_t>(vertex_count));
    std::vector<int> position(static_cast<std::size_t>(vertex_count));
    for(int index = 0; index < vertex_count; ++index){
        position[static_cast<std::size_t>(order[static_cast<std::size_t>(index)])] = index;
        auto& bag = result.bags[static_cast<std::size_t>(index)];
        bag.push_back(order[static_cast<std::size_t>(index)]);
        bag.insert(
            bag.end(),
            later_neighbors[static_cast<std::size_t>(index)].begin(),
            later_neighbors[static_cast<std::size_t>(index)].end()
        );
        std::sort(bag.begin(), bag.end());
    }

    std::vector<int> component_roots;
    for(int index = 0; index < vertex_count; ++index){
        const auto& separator = later_neighbors[static_cast<std::size_t>(index)];
        if(separator.empty()){
            component_roots.push_back(index);
            continue;
        }
        int parent = position[static_cast<std::size_t>(separator[0])];
        if(separator.size() == 2){
            parent = std::min(
                parent,
                position[static_cast<std::size_t>(separator[1])]
            );
        }
        result.tree_edges.emplace_back(index, parent);
    }
    for(std::size_t index = 1; index < component_roots.size(); ++index){
        result.tree_edges.emplace_back(
            component_roots[index - 1], component_roots[index]
        );
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_TREE_DECOMPOSITION_WIDTH_TWO_HPP_INCLUDED
