#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

struct TreeEdgeSplitDiameter{
    int first_diameter;
    int second_diameter;

    friend constexpr bool operator==(const TreeEdgeSplitDiameter&, const TreeEdgeSplitDiameter&) =
        default;
};

struct TreeTwoDisjointPathsResult{
    long long product;
    int cut_edge_index;
    int cut_u;
    int cut_v;
    int first_diameter;
    int second_diameter;

    friend constexpr bool operator==(
        const TreeTwoDisjointPathsResult&,
        const TreeTwoDisjointPathsResult&
    ) = default;
};

namespace tree_edge_split_diameters_detail{

struct ComponentState{
    int height;
    int diameter;
};

template<std::size_t COUNT>
struct TopValues{
    std::array<int, COUNT> value;
    std::array<int, COUNT> source;

    TopValues(){
        value.fill(0);
        source.fill(-1);
    }

    void insert(int candidate, int candidate_source){
        for(std::size_t index = 0; index < COUNT; ++index){
            if(candidate <= value[index]) continue;
            for(std::size_t shift = COUNT - 1; shift > index; --shift){
                value[shift] = value[shift - 1];
                source[shift] = source[shift - 1];
            }
            value[index] = candidate;
            source[index] = candidate_source;
            return;
        }
    }
};

inline ComponentState component_excluding(
    const TopValues<3>& heights,
    const TopValues<2>& diameters,
    int excluded_source
){
    int first_height = 0;
    int second_height = 0;
    bool found_first = false;
    for(std::size_t index = 0; index < heights.value.size(); ++index){
        if(heights.source[index] == excluded_source) continue;
        if(!found_first){
            first_height = heights.value[index];
            found_first = true;
        }else{
            second_height = heights.value[index];
            break;
        }
    }

    int inner_diameter = 0;
    for(std::size_t index = 0; index < diameters.value.size(); ++index){
        if(diameters.source[index] != excluded_source){
            inner_diameter = diameters.value[index];
            break;
        }
    }
    return {
        first_height,
        std::max(inner_diameter, first_height + second_height)
    };
}

template<int MAX_SIZE>
struct Workspace{
    static_assert(MAX_SIZE <= std::numeric_limits<int>::max() / 2);
    static constexpr int EDGE_CAPACITY = MAX_SIZE - 1;
    static constexpr std::size_t VERTEX_CAPACITY = static_cast<std::size_t>(MAX_SIZE);
    static constexpr std::size_t ADJACENCY_CAPACITY =
        static_cast<std::size_t>(2) * static_cast<std::size_t>(EDGE_CAPACITY);

    std::array<int, VERTEX_CAPACITY + 1> offset;
    std::array<int, ADJACENCY_CAPACITY> adjacency_to;
    std::array<int, ADJACENCY_CAPACITY> adjacency_edge;
    std::array<int, VERTEX_CAPACITY> parent;
    std::array<int, VERTEX_CAPACITY> parent_edge;
    std::array<int, VERTEX_CAPACITY> order;
    std::array<int, VERTEX_CAPACITY> stack;
    std::array<ComponentState, VERTEX_CAPACITY> downward;
    std::array<ComponentState, VERTEX_CAPACITY> outside;
};

template<int MAX_SIZE>
std::vector<TreeEdgeSplitDiameter> compute(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges,
    const char* range_message,
    const char* tree_message
){
    static_assert(MAX_SIZE > 0);
    if(vertex_count <= 0 || MAX_SIZE < vertex_count)[[unlikely]]{
        throw std::runtime_error(range_message);
    }
    const int edge_count = vertex_count - 1;
    if(edges.size() != static_cast<std::size_t>(edge_count))[[unlikely]]{
        throw std::runtime_error(tree_message);
    }

    auto workspace = std::make_unique_for_overwrite<Workspace<MAX_SIZE>>();
    std::fill_n(workspace->offset.begin(), vertex_count + 1, 0);
    for(int edge = 0; edge < edge_count; ++edge){
        const auto [left, right] = edges[static_cast<std::size_t>(edge)];
        if(left < 0 || vertex_count <= left || right < 0 || vertex_count <= right)[[unlikely]]{
            throw std::runtime_error(range_message);
        }
        ++workspace->offset[static_cast<std::size_t>(left + 1)];
        ++workspace->offset[static_cast<std::size_t>(right + 1)];
    }
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        workspace->offset[static_cast<std::size_t>(vertex + 1)] +=
            workspace->offset[static_cast<std::size_t>(vertex)];
    }

    std::copy_n(workspace->offset.begin(), vertex_count, workspace->stack.begin());
    for(int edge = 0; edge < edge_count; ++edge){
        const auto [left, right] = edges[static_cast<std::size_t>(edge)];
        int& left_position = workspace->stack[static_cast<std::size_t>(left)];
        workspace->adjacency_to[static_cast<std::size_t>(left_position)] = right;
        workspace->adjacency_edge[static_cast<std::size_t>(left_position++)] = edge;
        int& right_position = workspace->stack[static_cast<std::size_t>(right)];
        workspace->adjacency_to[static_cast<std::size_t>(right_position)] = left;
        workspace->adjacency_edge[static_cast<std::size_t>(right_position++)] = edge;
    }

    std::fill_n(workspace->parent.begin(), vertex_count, -2);
    std::fill_n(workspace->parent_edge.begin(), vertex_count, -1);
    workspace->parent[0] = -1;
    int stack_size = 1;
    int order_size = 0;
    workspace->stack[0] = 0;
    while(stack_size != 0){
        const int vertex = workspace->stack[static_cast<std::size_t>(--stack_size)];
        workspace->order[static_cast<std::size_t>(order_size++)] = vertex;
        for(
            int position = workspace->offset[static_cast<std::size_t>(vertex)];
            position < workspace->offset[static_cast<std::size_t>(vertex + 1)];
            ++position
        ){
            const int edge = workspace->adjacency_edge[static_cast<std::size_t>(position)];
            if(edge == workspace->parent_edge[static_cast<std::size_t>(vertex)]) continue;
            const int to = workspace->adjacency_to[static_cast<std::size_t>(position)];
            if(workspace->parent[static_cast<std::size_t>(to)] != -2)[[unlikely]]{
                throw std::runtime_error(tree_message);
            }
            workspace->parent[static_cast<std::size_t>(to)] = vertex;
            workspace->parent_edge[static_cast<std::size_t>(to)] = edge;
            workspace->stack[static_cast<std::size_t>(stack_size++)] = to;
        }
    }
    if(order_size != vertex_count)[[unlikely]]{
        throw std::runtime_error(tree_message);
    }

    for(int order_index = vertex_count - 1; order_index >= 0; --order_index){
        const int vertex = workspace->order[static_cast<std::size_t>(order_index)];
        TopValues<3> heights;
        TopValues<2> diameters;
        for(
            int position = workspace->offset[static_cast<std::size_t>(vertex)];
            position < workspace->offset[static_cast<std::size_t>(vertex + 1)];
            ++position
        ){
            const int to = workspace->adjacency_to[static_cast<std::size_t>(position)];
            if(workspace->parent[static_cast<std::size_t>(to)] != vertex) continue;
            const int edge = workspace->adjacency_edge[static_cast<std::size_t>(position)];
            const ComponentState child = workspace->downward[static_cast<std::size_t>(to)];
            heights.insert(child.height + 1, edge);
            diameters.insert(child.diameter, edge);
        }
        workspace->downward[static_cast<std::size_t>(vertex)] = {
            heights.value[0],
            std::max(diameters.value[0], heights.value[0] + heights.value[1])
        };
    }

    workspace->outside[0] = {0, 0};
    std::vector<TreeEdgeSplitDiameter> result(static_cast<std::size_t>(edge_count));
    for(int order_index = 0; order_index < vertex_count; ++order_index){
        const int vertex = workspace->order[static_cast<std::size_t>(order_index)];
        TopValues<3> heights;
        TopValues<2> diameters;
        for(
            int position = workspace->offset[static_cast<std::size_t>(vertex)];
            position < workspace->offset[static_cast<std::size_t>(vertex + 1)];
            ++position
        ){
            const int to = workspace->adjacency_to[static_cast<std::size_t>(position)];
            const int edge = workspace->adjacency_edge[static_cast<std::size_t>(position)];
            const ComponentState side = edge == workspace->parent_edge[static_cast<std::size_t>(vertex)]
                ? workspace->outside[static_cast<std::size_t>(vertex)]
                : workspace->downward[static_cast<std::size_t>(to)];
            heights.insert(side.height + 1, edge);
            diameters.insert(side.diameter, edge);
        }

        for(
            int position = workspace->offset[static_cast<std::size_t>(vertex)];
            position < workspace->offset[static_cast<std::size_t>(vertex + 1)];
            ++position
        ){
            const int to = workspace->adjacency_to[static_cast<std::size_t>(position)];
            if(workspace->parent[static_cast<std::size_t>(to)] != vertex) continue;
            const int edge = workspace->adjacency_edge[static_cast<std::size_t>(position)];
            const ComponentState parent_side = component_excluding(heights, diameters, edge);
            workspace->outside[static_cast<std::size_t>(to)] = parent_side;
            const int child_diameter = workspace->downward[static_cast<std::size_t>(to)].diameter;
            const auto [first_endpoint, second_endpoint] = edges[static_cast<std::size_t>(edge)];
            (void)second_endpoint;
            if(first_endpoint == to){
                result[static_cast<std::size_t>(edge)] = {
                    child_diameter,
                    parent_side.diameter
                };
            }else{
                result[static_cast<std::size_t>(edge)] = {
                    parent_side.diameter,
                    child_diameter
                };
            }
        }
    }
    return result;
}

}

template<int MAX_SIZE>
std::vector<TreeEdgeSplitDiameter> tree_edge_split_diameters(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    return tree_edge_split_diameters_detail::compute<MAX_SIZE>(
        vertex_count,
        edges,
        "library assertion fault: range violation (tree_edge_split_diameters).",
        "library assertion fault: graph is not a tree (tree_edge_split_diameters)."
    );
}

template<int MAX_SIZE>
TreeTwoDisjointPathsResult maximum_product_of_two_vertex_disjoint_tree_paths(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    const auto split = tree_edge_split_diameters_detail::compute<MAX_SIZE>(
        vertex_count,
        edges,
        "library assertion fault: range violation "
        "(maximum_product_of_two_vertex_disjoint_tree_paths).",
        "library assertion fault: graph is not a tree "
        "(maximum_product_of_two_vertex_disjoint_tree_paths)."
    );
    TreeTwoDisjointPathsResult result{0, -1, -1, -1, 0, 0};
    for(int edge = 0; edge < static_cast<int>(split.size()); ++edge){
        const TreeEdgeSplitDiameter current = split[static_cast<std::size_t>(edge)];
        const long long product =
            static_cast<long long>(current.first_diameter) * current.second_diameter;
        if(result.cut_edge_index != -1 && product <= result.product) continue;
        result = {
            product,
            edge,
            edges[static_cast<std::size_t>(edge)].first,
            edges[static_cast<std::size_t>(edge)].second,
            current.first_diameter,
            current.second_diameter
        };
    }
    return result;
}
