#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_COUNT_MINIMUM_SPANNING_TREES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_COUNT_MINIMUM_SPANNING_TREES_HPP_INCLUDED

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

template<class Weight>
struct MinimumSpanningTreeCountEdge{
    int from;
    int to;
    Weight weight;
};

template<class Weight>
struct MinimumSpanningTreeCountResult{
    bool connected;
    Weight minimum_weight;
    std::uint64_t count_mod;
};

namespace count_minimum_spanning_trees_internal{

struct DisjointSetUnion{
    std::vector<int> parent_or_size;
    int component_count;

    explicit DisjointSetUnion(int n): parent_or_size(n, -1), component_count(n){}

    int leader(int vertex){
        int root = vertex;
        while(parent_or_size[root] >= 0) root = parent_or_size[root];
        while(vertex != root){
            const int parent = parent_or_size[vertex];
            parent_or_size[vertex] = root;
            vertex = parent;
        }
        return root;
    }

    bool merge(int first, int second){
        first = leader(first);
        second = leader(second);
        if(first == second) return false;
        if(-parent_or_size[first] < -parent_or_size[second]){
            std::swap(first, second);
        }
        parent_or_size[first] += parent_or_size[second];
        parent_or_size[second] = first;
        --component_count;
        return true;
    }
};

template<std::uint64_t MOD>
std::uint64_t multiply_mod(std::uint64_t first, std::uint64_t second){
    return static_cast<std::uint64_t>(
        static_cast<__uint128_t>(first) * second % MOD
    );
}

template<std::uint64_t MOD>
std::uint64_t subtract_product_mod(
    std::uint64_t value,
    std::uint64_t factor,
    std::uint64_t multiplier
){
    const std::uint64_t product = multiply_mod<MOD>(factor, multiplier);
    if(value >= product) return value - product;
    return MOD - (product - value);
}

template<std::uint64_t MOD>
void increment_mod(std::uint64_t& value){
    value = value == MOD - 1 ? 0 : value + 1;
}

template<std::uint64_t MOD>
void decrement_mod(std::uint64_t& value){
    value = value == 0 ? MOD - 1 : value - 1;
}

// Euclidean row elimination works over Z / MOD Z and does not require MOD to
// be prime.  Every iteration replaces one pivot-column value by its ordinary
// integer remainder, so the inner loop terminates.
template<std::uint64_t MOD>
std::uint64_t determinant_mod(std::vector<std::vector<std::uint64_t>> matrix){
    const int n = static_cast<int>(matrix.size());
    std::uint64_t determinant = 1 % MOD;
    for(int column = 0; column < n; ++column){
        for(int row = column + 1; row < n; ++row){
            while(matrix[row][column] != 0){
                const std::uint64_t quotient =
                    matrix[column][column] / matrix[row][column];
                for(int next_column = column; next_column < n; ++next_column){
                    matrix[column][next_column] = subtract_product_mod<MOD>(
                        matrix[column][next_column],
                        matrix[row][next_column],
                        quotient
                    );
                }
                std::swap(matrix[column], matrix[row]);
                if(determinant != 0) determinant = MOD - determinant;
            }
        }
        determinant = multiply_mod<MOD>(
            determinant,
            matrix[column][column]
        );
        if(determinant == 0) return 0;
    }
    return determinant;
}

template<std::uint64_t MOD>
std::uint64_t count_component_spanning_trees(
    const std::vector<int>& vertices,
    const std::vector<std::pair<int, int>>& edges,
    std::vector<int>& position
){
    const int vertex_count = static_cast<int>(vertices.size());
    if(vertex_count <= 1) return 1 % MOD;
    for(int index = 0; index < vertex_count; ++index){
        position[vertices[index]] = index;
    }

    const int minor_size = vertex_count - 1;
    std::vector<std::vector<std::uint64_t>> laplacian_minor(
        minor_size,
        std::vector<std::uint64_t>(minor_size)
    );
    for(const auto& [first_vertex, second_vertex]: edges){
        const int first = position[first_vertex];
        const int second = position[second_vertex];
        if(first < minor_size){
            increment_mod<MOD>(laplacian_minor[first][first]);
        }
        if(second < minor_size){
            increment_mod<MOD>(laplacian_minor[second][second]);
        }
        if(first < minor_size && second < minor_size){
            decrement_mod<MOD>(laplacian_minor[first][second]);
            decrement_mod<MOD>(laplacian_minor[second][first]);
        }
    }

    for(const int vertex: vertices) position[vertex] = -1;
    return determinant_mod<MOD>(std::move(laplacian_minor));
}

} // namespace count_minimum_spanning_trees_internal

template<std::uint64_t MOD, class Weight>
MinimumSpanningTreeCountResult<Weight> count_minimum_spanning_trees(
    int vertex_count,
    const std::vector<MinimumSpanningTreeCountEdge<Weight>>& input_edges
){
    static_assert(MOD >= 2, "the counting modulus must be at least two");
    if(vertex_count < 0)[[unlikely]]{
        throw std::invalid_argument(
            "count_minimum_spanning_trees: negative vertex count"
        );
    }
    for(const auto& edge: input_edges){
        if(edge.from < 0 || vertex_count <= edge.from ||
           edge.to < 0 || vertex_count <= edge.to)[[unlikely]]{
            throw std::out_of_range(
                "count_minimum_spanning_trees: edge endpoint out of range"
            );
        }
    }
    if(vertex_count <= 1){
        return {true, Weight{}, 1 % MOD};
    }

    std::vector<MinimumSpanningTreeCountEdge<Weight>> edges = input_edges;
    std::sort(edges.begin(), edges.end(), [](const auto& first, const auto& second){
        return first.weight < second.weight;
    });

    using count_minimum_spanning_trees_internal::DisjointSetUnion;
    DisjointSetUnion components(vertex_count);
    Weight minimum_weight{};
    std::uint64_t count = 1 % MOD;
    std::vector<int> root_to_local(vertex_count, -1);

    for(std::size_t group_begin = 0; group_begin < edges.size();){
        std::size_t group_end = group_begin + 1;
        while(group_end < edges.size() &&
              !(edges[group_begin].weight < edges[group_end].weight) &&
              !(edges[group_end].weight < edges[group_begin].weight)){
            ++group_end;
        }

        std::vector<int> touched_roots;
        std::vector<std::pair<int, int>> local_edges;
        touched_roots.reserve(2 * (group_end - group_begin));
        local_edges.reserve(group_end - group_begin);
        for(std::size_t index = group_begin; index < group_end; ++index){
            const int first_root = components.leader(edges[index].from);
            const int second_root = components.leader(edges[index].to);
            if(first_root == second_root) continue;
            if(root_to_local[first_root] == -1){
                root_to_local[first_root] = static_cast<int>(touched_roots.size());
                touched_roots.push_back(first_root);
            }
            if(root_to_local[second_root] == -1){
                root_to_local[second_root] = static_cast<int>(touched_roots.size());
                touched_roots.push_back(second_root);
            }
            local_edges.push_back({
                root_to_local[first_root],
                root_to_local[second_root]
            });
        }

        DisjointSetUnion local_components(static_cast<int>(touched_roots.size()));
        for(const auto& [first, second]: local_edges){
            local_components.merge(first, second);
        }
        std::vector<int> component_index(touched_roots.size(), -1);
        std::vector<std::vector<int>> component_vertices;
        for(int vertex = 0; vertex < static_cast<int>(touched_roots.size()); ++vertex){
            const int root = local_components.leader(vertex);
            if(component_index[root] == -1){
                component_index[root] = static_cast<int>(component_vertices.size());
                component_vertices.push_back({});
            }
            component_vertices[component_index[root]].push_back(vertex);
        }
        std::vector<std::vector<std::pair<int, int>>> component_edges(
            component_vertices.size()
        );
        for(const auto& edge: local_edges){
            const int root = local_components.leader(edge.first);
            component_edges[component_index[root]].push_back(edge);
        }
        std::vector<int> position(touched_roots.size(), -1);
        for(std::size_t index = 0; index < component_vertices.size(); ++index){
            const std::uint64_t choices =
                count_minimum_spanning_trees_internal::
                    count_component_spanning_trees<MOD>(
                        component_vertices[index],
                        component_edges[index],
                        position
                    );
            count = count_minimum_spanning_trees_internal::multiply_mod<MOD>(
                count,
                choices
            );
        }
        for(const int root: touched_roots) root_to_local[root] = -1;

        for(std::size_t index = group_begin; index < group_end; ++index){
            if(components.merge(edges[index].from, edges[index].to)){
                minimum_weight = minimum_weight + edges[index].weight;
            }
        }
        group_begin = group_end;
    }

    if(components.component_count != 1){
        return {false, Weight{}, 0};
    }
    return {true, minimum_weight, count};
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_COUNT_MINIMUM_SPANNING_TREES_HPP_INCLUDED
