#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_COLORING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_COLORING_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "detail/undirected_graph.hpp"
#include "independent_set.hpp"

namespace approximate::graph {

struct ColoringResult {
    std::vector<std::size_t> colors;
    std::size_t color_count = 0;
};

struct ColoringSearchResult {
    std::vector<std::size_t> colors;
    std::size_t conflicts = 0;
    std::size_t iterations = 0;
    bool converged = false;
};

namespace detail {

inline void require_colorable_simple_graph(const UndirectedGraph& graph){
    if(std::find(graph.self_loop.begin(), graph.self_loop.end(), true) != graph.self_loop.end()){
        throw std::invalid_argument("a graph with a self-loop has no proper coloring");
    }
}

}  // namespace detail

[[nodiscard]] inline ColoringResult greedy_coloring(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges,
    std::vector<std::size_t> order = {}
){
    const detail::UndirectedGraph graph = detail::make_undirected_graph(vertex_count, edges);
    detail::require_colorable_simple_graph(graph);
    if(order.empty()){
        order.resize(vertex_count);
        std::iota(order.begin(), order.end(), std::size_t{0});
    }else{
        detail::validate_vertex_order(vertex_count, order);
    }
    ColoringResult result;
    result.colors.assign(vertex_count, vertex_count);
    std::vector<std::size_t> forbidden(vertex_count, vertex_count);
    for(const std::size_t vertex : order){
        for(const std::size_t neighbor : graph.adjacency[vertex]){
            if(result.colors[neighbor] != vertex_count){
                forbidden[result.colors[neighbor]] = vertex;
            }
        }
        std::size_t color = 0;
        while(color < vertex_count && forbidden[color] == vertex) ++color;
        result.colors[vertex] = color;
        result.color_count = std::max(result.color_count, color + 1);
    }
    return result;
}

[[nodiscard]] inline std::vector<std::size_t> largest_first_order(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    const detail::UndirectedGraph graph = detail::make_undirected_graph(vertex_count, edges);
    detail::require_colorable_simple_graph(graph);
    std::vector<std::size_t> order(vertex_count);
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::sort(order.begin(), order.end(), [&](std::size_t left, std::size_t right){
        if(graph.adjacency[left].size() != graph.adjacency[right].size()){
            return graph.adjacency[left].size() > graph.adjacency[right].size();
        }
        return left < right;
    });
    return order;
}

[[nodiscard]] inline ColoringResult largest_first_coloring(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    return greedy_coloring(vertex_count, edges, largest_first_order(vertex_count, edges));
}

[[nodiscard]] inline ColoringResult welsh_powell_coloring(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    return largest_first_coloring(vertex_count, edges);
}

[[nodiscard]] inline std::vector<std::size_t> smallest_last_order(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    const detail::UndirectedGraph graph = detail::make_undirected_graph(vertex_count, edges);
    detail::require_colorable_simple_graph(graph);
    std::vector<std::size_t> degree(vertex_count);
    std::vector<bool> active(vertex_count, true);
    using Entry = std::pair<std::size_t, std::size_t>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> heap;
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        degree[vertex] = graph.adjacency[vertex].size();
        heap.emplace(degree[vertex], vertex);
    }
    std::vector<std::size_t> removal;
    removal.reserve(vertex_count);
    while(!heap.empty()){
        const auto [entry_degree, vertex] = heap.top();
        heap.pop();
        if(!active[vertex] || entry_degree != degree[vertex]) continue;
        active[vertex] = false;
        removal.push_back(vertex);
        for(const std::size_t neighbor : graph.adjacency[vertex]){
            if(active[neighbor]){
                --degree[neighbor];
                heap.emplace(degree[neighbor], neighbor);
            }
        }
    }
    std::reverse(removal.begin(), removal.end());
    return removal;
}

[[nodiscard]] inline ColoringResult smallest_last_coloring(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    return greedy_coloring(vertex_count, edges, smallest_last_order(vertex_count, edges));
}

[[nodiscard]] inline ColoringResult dsatur_coloring(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    const detail::UndirectedGraph graph = detail::make_undirected_graph(vertex_count, edges);
    detail::require_colorable_simple_graph(graph);
    ColoringResult result;
    result.colors.assign(vertex_count, vertex_count);
    std::vector<std::set<std::size_t>> neighbor_colors(vertex_count);
    std::vector<std::size_t> forbidden(vertex_count, vertex_count);
    for(std::size_t step = 0; step < vertex_count; ++step){
        std::size_t selected = vertex_count;
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            if(result.colors[vertex] != vertex_count) continue;
            if(selected == vertex_count ||
               neighbor_colors[vertex].size() > neighbor_colors[selected].size() ||
               (neighbor_colors[vertex].size() == neighbor_colors[selected].size() &&
                graph.adjacency[vertex].size() > graph.adjacency[selected].size()) ||
               (neighbor_colors[vertex].size() == neighbor_colors[selected].size() &&
                graph.adjacency[vertex].size() == graph.adjacency[selected].size() &&
                vertex < selected)){
                selected = vertex;
            }
        }
        for(const std::size_t neighbor : graph.adjacency[selected]){
            if(result.colors[neighbor] != vertex_count){
                forbidden[result.colors[neighbor]] = selected;
            }
        }
        std::size_t color = 0;
        while(color < vertex_count && forbidden[color] == selected) ++color;
        result.colors[selected] = color;
        result.color_count = std::max(result.color_count, color + 1);
        for(const std::size_t neighbor : graph.adjacency[selected]){
            if(result.colors[neighbor] == vertex_count){
                neighbor_colors[neighbor].insert(color);
            }
        }
    }
    return result;
}

namespace detail {

inline void validate_target_color_count(std::size_t vertex_count, std::size_t target_color_count){
    if((vertex_count == 0 && target_color_count != 0)
       || (vertex_count != 0 && (target_color_count == 0 || target_color_count > vertex_count))){
        throw std::invalid_argument("target color count is outside the feasible palette range");
    }
}

[[nodiscard]] inline std::vector<std::size_t> canonical_coloring(
    const std::vector<std::size_t>& colors
){
    std::map<std::size_t, std::size_t> canonical;
    std::vector<std::size_t> result(colors.size());
    std::size_t next_color = 0;
    for(std::size_t vertex = 0; vertex < colors.size(); ++vertex){
        const auto [iterator, inserted] = canonical.emplace(colors[vertex], next_color);
        if(inserted) ++next_color;
        result[vertex] = iterator->second;
    }
    return result;
}

inline void validate_proper_coloring(
    const UndirectedGraph& graph,
    const std::vector<std::size_t>& colors
){
    if(colors.size() != graph.adjacency.size()){
        throw std::invalid_argument("initial coloring has the wrong size");
    }
    for(std::size_t vertex = 0; vertex < graph.adjacency.size(); ++vertex){
        for(const std::size_t neighbor : graph.adjacency[vertex]){
            if(vertex < neighbor && colors[vertex] == colors[neighbor]){
                throw std::invalid_argument("initial coloring is not proper");
            }
        }
    }
}

[[nodiscard]] inline std::size_t used_color_count(const std::vector<std::size_t>& colors){
    if(colors.empty()) return 0;
    const std::size_t maximum = *std::max_element(colors.begin(), colors.end());
    if(maximum == std::numeric_limits<std::size_t>::max()){
        throw std::overflow_error("the number of used colors is not representable");
    }
    return maximum + 1;
}

[[nodiscard]] inline ColoringResult recursive_largest_first_from_graph(
    const UndirectedGraph& graph
){
    const std::size_t vertex_count = graph.adjacency.size();
    ColoringResult result;
    result.colors.assign(vertex_count, vertex_count);
    std::vector<bool> uncolored(vertex_count, true);
    std::size_t remaining = vertex_count;
    while(remaining != 0){
        std::size_t seed = vertex_count;
        std::size_t best_degree = 0;
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            if(!uncolored[vertex]) continue;
            std::size_t degree = 0;
            for(const std::size_t neighbor : graph.adjacency[vertex]){
                if(uncolored[neighbor]) ++degree;
            }
            if(seed == vertex_count || degree > best_degree
               || (degree == best_degree && vertex < seed)){
                seed = vertex;
                best_degree = degree;
            }
        }
        std::vector<bool> blocked(vertex_count, false);
        result.colors[seed] = result.color_count;
        uncolored[seed] = false;
        --remaining;
        for(const std::size_t neighbor : graph.adjacency[seed]){
            if(uncolored[neighbor]) blocked[neighbor] = true;
        }
        while(true){
            std::size_t selected = vertex_count;
            std::size_t best_blocked_neighbors = 0;
            std::size_t best_candidate_degree = 0;
            for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
                if(!uncolored[vertex] || blocked[vertex]) continue;
                std::size_t blocked_neighbors = 0;
                std::size_t candidate_degree = 0;
                for(const std::size_t neighbor : graph.adjacency[vertex]){
                    if(!uncolored[neighbor]) continue;
                    if(blocked[neighbor]) ++blocked_neighbors;
                    else ++candidate_degree;
                }
                if(selected == vertex_count || blocked_neighbors > best_blocked_neighbors
                   || (blocked_neighbors == best_blocked_neighbors
                       && candidate_degree > best_candidate_degree)
                   || (blocked_neighbors == best_blocked_neighbors
                       && candidate_degree == best_candidate_degree && vertex < selected)){
                    selected = vertex;
                    best_blocked_neighbors = blocked_neighbors;
                    best_candidate_degree = candidate_degree;
                }
            }
            if(selected == vertex_count) break;
            result.colors[selected] = result.color_count;
            uncolored[selected] = false;
            --remaining;
            for(const std::size_t neighbor : graph.adjacency[selected]){
                if(uncolored[neighbor]) blocked[neighbor] = true;
            }
        }
        ++result.color_count;
    }
    return result;
}

[[nodiscard]] inline std::size_t checked_tabu_expiration(
    std::size_t iteration,
    std::size_t tabu_tenure
){
    if(iteration == std::numeric_limits<std::size_t>::max()
       || tabu_tenure > std::numeric_limits<std::size_t>::max() - iteration - 1){
        throw std::overflow_error("tabu expiration is not representable");
    }
    return iteration + tabu_tenure + 1;
}

inline void validate_dense_color_table(
    std::size_t vertex_count,
    std::size_t target_color_count
){
    if(vertex_count != 0
       && target_color_count > std::numeric_limits<std::size_t>::max() / vertex_count){
        throw std::length_error("the dense vertex-color table is too large");
    }
}

[[nodiscard]] inline std::vector<std::size_t> neighbor_color_counts(
    const UndirectedGraph& graph,
    const std::vector<std::size_t>& colors,
    std::size_t target_color_count
){
    validate_dense_color_table(graph.adjacency.size(), target_color_count);
    std::vector<std::size_t> counts(graph.adjacency.size() * target_color_count, 0);
    for(std::size_t vertex = 0; vertex < graph.adjacency.size(); ++vertex){
        for(const std::size_t neighbor : graph.adjacency[vertex]){
            if(colors[neighbor] < target_color_count){
                ++counts[vertex * target_color_count + colors[neighbor]];
            }
        }
    }
    return counts;
}

[[nodiscard]] inline std::size_t edge_conflict_count(
    const UndirectedGraph& graph,
    const std::vector<std::size_t>& colors
){
    std::size_t result = 0;
    for(std::size_t vertex = 0; vertex < graph.adjacency.size(); ++vertex){
        for(const std::size_t neighbor : graph.adjacency[vertex]){
            if(vertex < neighbor && colors[vertex] == colors[neighbor]) ++result;
        }
    }
    return result;
}

template<class UniformRandomBitGenerator>
[[nodiscard]] bool replace_equal_candidate(
    std::size_t& tie_count,
    UniformRandomBitGenerator& generator
){
    if(tie_count == std::numeric_limits<std::size_t>::max()){
        throw std::overflow_error("too many tied coloring moves");
    }
    ++tie_count;
    std::uniform_int_distribution<std::size_t> distribution(1, tie_count);
    return distribution(generator) == 1;
}

}  // namespace detail

[[nodiscard]] inline ColoringResult recursive_largest_first_coloring(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    const detail::UndirectedGraph graph = detail::make_undirected_graph(vertex_count, edges);
    detail::require_colorable_simple_graph(graph);
    return detail::recursive_largest_first_from_graph(graph);
}

[[nodiscard]] inline ColoringSearchResult kempe_chain_interchange(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges,
    std::size_t target_color_count,
    std::uint64_t seed,
    std::size_t maximum_iterations = 100,
    std::vector<std::size_t> initial_colors = {}
){
    const detail::UndirectedGraph graph = detail::make_undirected_graph(vertex_count, edges);
    detail::require_colorable_simple_graph(graph);
    detail::validate_target_color_count(vertex_count, target_color_count);
    ColoringSearchResult result;
    if(vertex_count == 0){
        result.converged = true;
        return result;
    }
    if(initial_colors.empty()){
        initial_colors = detail::recursive_largest_first_from_graph(graph).colors;
    }else{
        detail::validate_proper_coloring(graph, initial_colors);
        initial_colors = detail::canonical_coloring(initial_colors);
    }
    std::vector<std::size_t> colors = std::move(initial_colors);
    std::size_t color_count = detail::used_color_count(colors);
    std::mt19937_64 generator(seed);
    for(std::size_t iteration = 0;
        iteration < maximum_iterations && color_count > target_color_count;
        ++iteration){
        const std::size_t highest_color = color_count - 1;
        std::vector<std::size_t> vertices;
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            if(colors[vertex] == highest_color) vertices.push_back(vertex);
        }
        std::shuffle(vertices.begin(), vertices.end(), generator);
        bool changed = false;
        for(const std::size_t vertex : vertices){
            if(colors[vertex] != highest_color) continue;
            std::vector<std::size_t> lower_colors(highest_color);
            std::iota(lower_colors.begin(), lower_colors.end(), std::size_t{0});
            std::shuffle(lower_colors.begin(), lower_colors.end(), generator);
            std::vector<bool> adjacent_color(highest_color, false);
            for(const std::size_t neighbor : graph.adjacency[vertex]){
                if(colors[neighbor] < highest_color) adjacent_color[colors[neighbor]] = true;
            }
            bool recolored = false;
            for(const std::size_t color : lower_colors){
                if(!adjacent_color[color]){
                    colors[vertex] = color;
                    recolored = true;
                    changed = true;
                    break;
                }
            }
            if(recolored) continue;
            for(const std::size_t first_color : lower_colors){
                std::vector<std::size_t> second_colors = lower_colors;
                std::shuffle(second_colors.begin(), second_colors.end(), generator);
                for(const std::size_t second_color : second_colors){
                    if(first_color == second_color) continue;
                    std::vector<bool> adjacent_second(vertex_count, false);
                    for(const std::size_t neighbor : graph.adjacency[vertex]){
                        if(colors[neighbor] == second_color) adjacent_second[neighbor] = true;
                    }
                    std::vector<bool> visited(vertex_count, false);
                    std::vector<bool> should_swap(vertex_count, false);
                    bool possible = true;
                    for(const std::size_t neighbor : graph.adjacency[vertex]){
                        if(colors[neighbor] != first_color || visited[neighbor]) continue;
                        std::queue<std::size_t> queue;
                        std::vector<std::size_t> component;
                        bool contains_second_neighbor = false;
                        visited[neighbor] = true;
                        queue.push(neighbor);
                        while(!queue.empty()){
                            const std::size_t current = queue.front();
                            queue.pop();
                            component.push_back(current);
                            contains_second_neighbor = contains_second_neighbor
                                || adjacent_second[current];
                            for(const std::size_t next : graph.adjacency[current]){
                                if(!visited[next]
                                   && (colors[next] == first_color
                                       || colors[next] == second_color)){
                                    visited[next] = true;
                                    queue.push(next);
                                }
                            }
                        }
                        if(contains_second_neighbor){
                            possible = false;
                            break;
                        }
                        for(const std::size_t current : component) should_swap[current] = true;
                    }
                    if(!possible) continue;
                    for(std::size_t current = 0; current < vertex_count; ++current){
                        if(!should_swap[current]) continue;
                        colors[current] = colors[current] == first_color
                            ? second_color
                            : first_color;
                    }
                    colors[vertex] = first_color;
                    recolored = true;
                    changed = true;
                    break;
                }
                if(recolored) break;
            }
        }
        result.iterations = iteration + 1;
        colors = detail::canonical_coloring(colors);
        color_count = detail::used_color_count(colors);
        if(!changed) break;
    }
    result.colors = std::move(colors);
    result.conflicts = 0;
    result.converged = color_count <= target_color_count;
    return result;
}

[[nodiscard]] inline ColoringSearchResult tabucol(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges,
    std::size_t target_color_count,
    std::uint64_t seed,
    std::size_t maximum_iterations = 10000,
    std::size_t tabu_tenure = 7,
    bool aspiration = true,
    std::vector<std::size_t> initial_colors = {}
){
    const detail::UndirectedGraph graph = detail::make_undirected_graph(vertex_count, edges);
    detail::require_colorable_simple_graph(graph);
    detail::validate_target_color_count(vertex_count, target_color_count);
    ColoringSearchResult result;
    if(vertex_count == 0){
        result.converged = true;
        return result;
    }
    detail::validate_dense_color_table(vertex_count, target_color_count);
    std::mt19937_64 generator(seed);
    std::vector<std::size_t> colors;
    if(initial_colors.empty()){
        const ColoringResult initial = detail::recursive_largest_first_from_graph(graph);
        std::vector<std::size_t> palette(target_color_count);
        std::iota(palette.begin(), palette.end(), std::size_t{0});
        std::shuffle(palette.begin(), palette.end(), generator);
        colors.resize(vertex_count);
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            colors[vertex] = palette[initial.colors[vertex] % target_color_count];
        }
    }else{
        if(initial_colors.size() != vertex_count){
            throw std::invalid_argument("initial coloring has the wrong size");
        }
        for(const std::size_t color : initial_colors){
            if(color >= target_color_count){
                throw std::invalid_argument("an initial color is outside the target palette");
            }
        }
        colors = std::move(initial_colors);
    }
    std::vector<std::size_t> counts = detail::neighbor_color_counts(
        graph, colors, target_color_count
    );
    std::vector<std::size_t> tabu_until(vertex_count * target_color_count, 0);
    std::size_t conflicts = detail::edge_conflict_count(graph, colors);
    std::size_t best_conflicts = conflicts;
    std::vector<std::size_t> best_colors = colors;
    for(std::size_t iteration = 0; iteration < maximum_iterations && best_conflicts != 0;
        ++iteration){
        bool found = false;
        std::size_t best_vertex = 0;
        std::size_t best_color = 0;
        std::size_t best_move_conflicts = 0;
        std::size_t tie_count = 0;
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            const std::size_t old_color = colors[vertex];
            const std::size_t old_incident = counts[vertex * target_color_count + old_color];
            if(old_incident == 0) continue;
            for(std::size_t color = 0; color < target_color_count; ++color){
                if(color == old_color) continue;
                const std::size_t candidate = conflicts - old_incident
                    + counts[vertex * target_color_count + color];
                const bool is_tabu = tabu_until[vertex * target_color_count + color] > iteration;
                if(is_tabu && !(aspiration && candidate < best_conflicts)) continue;
                if(!found || candidate < best_move_conflicts){
                    found = true;
                    best_vertex = vertex;
                    best_color = color;
                    best_move_conflicts = candidate;
                    tie_count = 1;
                }else if(candidate == best_move_conflicts
                    && detail::replace_equal_candidate(tie_count, generator)){
                    best_vertex = vertex;
                    best_color = color;
                }
            }
        }
        if(!found) break;
        const std::size_t old_color = colors[best_vertex];
        for(const std::size_t neighbor : graph.adjacency[best_vertex]){
            --counts[neighbor * target_color_count + old_color];
            ++counts[neighbor * target_color_count + best_color];
        }
        colors[best_vertex] = best_color;
        tabu_until[best_vertex * target_color_count + old_color]
            = detail::checked_tabu_expiration(iteration, tabu_tenure);
        conflicts = best_move_conflicts;
        result.iterations = iteration + 1;
        if(conflicts < best_conflicts){
            best_conflicts = conflicts;
            best_colors = colors;
        }
    }
    result.colors = std::move(best_colors);
    result.conflicts = best_conflicts;
    result.converged = best_conflicts == 0;
    return result;
}

[[nodiscard]] inline ColoringSearchResult partialcol(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges,
    std::size_t target_color_count,
    std::uint64_t seed,
    std::size_t maximum_iterations = 10000,
    std::size_t tabu_tenure = 7,
    bool aspiration = true,
    std::vector<std::size_t> initial_colors = {}
){
    const detail::UndirectedGraph graph = detail::make_undirected_graph(vertex_count, edges);
    detail::require_colorable_simple_graph(graph);
    detail::validate_target_color_count(vertex_count, target_color_count);
    ColoringSearchResult result;
    if(vertex_count == 0){
        result.converged = true;
        return result;
    }
    detail::validate_dense_color_table(vertex_count, target_color_count);
    std::mt19937_64 generator(seed);
    std::vector<std::size_t> colors(vertex_count, target_color_count);
    if(initial_colors.empty()){
        std::vector<std::size_t> order(vertex_count);
        std::iota(order.begin(), order.end(), std::size_t{0});
        std::shuffle(order.begin(), order.end(), generator);
        std::uniform_int_distribution<std::size_t> color_distribution(
            0, target_color_count - 1
        );
        std::vector<std::size_t> forbidden(target_color_count, vertex_count);
        for(const std::size_t vertex : order){
            for(const std::size_t neighbor : graph.adjacency[vertex]){
                if(colors[neighbor] < target_color_count){
                    forbidden[colors[neighbor]] = vertex;
                }
            }
            const std::size_t first = color_distribution(generator);
            for(std::size_t offset = 0; offset < target_color_count; ++offset){
                const std::size_t color = (first + offset) % target_color_count;
                if(forbidden[color] != vertex){
                    colors[vertex] = color;
                    break;
                }
            }
        }
    }else{
        if(initial_colors.size() != vertex_count){
            throw std::invalid_argument("initial partial coloring has the wrong size");
        }
        for(const std::size_t color : initial_colors){
            if(color > target_color_count){
                throw std::invalid_argument("an initial partial color is outside the palette");
            }
        }
        colors = std::move(initial_colors);
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            for(const std::size_t neighbor : graph.adjacency[vertex]){
                if(vertex < neighbor && colors[vertex] < target_color_count
                   && colors[vertex] == colors[neighbor]){
                    throw std::invalid_argument("initial partial coloring is not proper");
                }
            }
        }
    }
    std::vector<std::size_t> counts = detail::neighbor_color_counts(
        graph, colors, target_color_count
    );
    std::vector<std::size_t> tabu_until(vertex_count * target_color_count, 0);
    std::size_t uncolored = static_cast<std::size_t>(std::count(
        colors.begin(), colors.end(), target_color_count
    ));
    std::size_t best_uncolored = uncolored;
    std::vector<std::size_t> best_colors = colors;
    for(std::size_t iteration = 0; iteration < maximum_iterations && best_uncolored != 0;
        ++iteration){
        bool found = false;
        std::size_t best_vertex = 0;
        std::size_t best_color = 0;
        std::size_t best_candidate = 0;
        std::size_t tie_count = 0;
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            if(colors[vertex] != target_color_count) continue;
            for(std::size_t color = 0; color < target_color_count; ++color){
                const std::size_t candidate = uncolored - 1
                    + counts[vertex * target_color_count + color];
                const bool is_tabu = tabu_until[vertex * target_color_count + color] > iteration;
                if(is_tabu && !(aspiration && candidate < best_uncolored)) continue;
                if(!found || candidate < best_candidate){
                    found = true;
                    best_vertex = vertex;
                    best_color = color;
                    best_candidate = candidate;
                    tie_count = 1;
                }else if(candidate == best_candidate
                    && detail::replace_equal_candidate(tie_count, generator)){
                    best_vertex = vertex;
                    best_color = color;
                }
            }
        }
        if(!found) break;
        std::vector<std::size_t> displaced;
        for(const std::size_t neighbor : graph.adjacency[best_vertex]){
            if(colors[neighbor] == best_color) displaced.push_back(neighbor);
        }
        colors[best_vertex] = best_color;
        for(const std::size_t neighbor : graph.adjacency[best_vertex]){
            ++counts[neighbor * target_color_count + best_color];
        }
        if(!displaced.empty()){
            const std::size_t expiration = detail::checked_tabu_expiration(
                iteration, tabu_tenure
            );
            for(const std::size_t vertex : displaced){
                colors[vertex] = target_color_count;
                for(const std::size_t neighbor : graph.adjacency[vertex]){
                    --counts[neighbor * target_color_count + best_color];
                }
                tabu_until[vertex * target_color_count + best_color] = expiration;
            }
        }
        uncolored = best_candidate;
        result.iterations = iteration + 1;
        if(uncolored < best_uncolored){
            best_uncolored = uncolored;
            best_colors = colors;
        }
    }
    result.colors = std::move(best_colors);
    result.conflicts = best_uncolored;
    result.converged = best_uncolored == 0;
    return result;
}

}  // namespace approximate::graph

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_COLORING_HPP_INCLUDED
