// competitive-verifier: STANDALONE

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/graph/coloring.hpp"

namespace {

void require(bool condition){
    if(!condition) throw std::runtime_error("TabuCol test failed");
}

template<class Exception, class Function>
void require_throws(Function&& function){
    bool thrown = false;
    try{
        std::forward<Function>(function)();
    }catch(const Exception&){
        thrown = true;
    }
    require(thrown);
}

std::size_t conflict_count(
    const std::vector<std::pair<std::size_t, std::size_t>>& edges,
    const std::vector<std::size_t>& colors
){
    std::set<std::pair<std::size_t, std::size_t>> simple_edges;
    for(auto [left, right] : edges){
        if(left > right) std::swap(left, right);
        if(left != right) simple_edges.emplace(left, right);
    }
    std::size_t conflicts = 0;
    for(const auto [left, right] : simple_edges){
        if(colors[left] == colors[right]) ++conflicts;
    }
    return conflicts;
}

void check_result(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges,
    std::size_t target,
    std::size_t maximum_iterations,
    std::size_t chromatic_number,
    bool require_convergence,
    const approximate::graph::ColoringSearchResult& result
){
    require(result.colors.size() == vertex_count);
    for(const std::size_t color : result.colors) require(color < target);
    require(result.iterations <= maximum_iterations);
    require(result.conflicts == conflict_count(edges, result.colors));
    require(result.converged == (result.conflicts == 0));
    if(target < chromatic_number) require(!result.converged);
    if(require_convergence) require(result.converged);
}

void boundary_tests(){
    using approximate::graph::tabucol;
    require_throws<std::invalid_argument>([]{
        static_cast<void>(tabucol(1, {{0, 0}}, 1, 0));
    });
    require_throws<std::out_of_range>([]{
        static_cast<void>(tabucol(2, {{0, 2}}, 1, 0));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(tabucol(0, {}, 1, 0));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(tabucol(2, {}, 0, 0));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(tabucol(2, {}, 3, 0));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(tabucol(2, {}, 2, 0, 0, 1, true, {0}));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(tabucol(2, {}, 2, 0, 0, 1, true, {0, 2}));
    });
    const auto empty = tabucol(0, {}, 0, 0);
    require(empty.colors.empty() && empty.conflicts == 0 && empty.converged);

    const std::vector<std::pair<std::size_t, std::size_t>> path{{0, 1}, {1, 2}};
    const auto stopped = tabucol(3, path, 2, 5, 0, 3, true, {0, 0, 1});
    require(stopped.colors == std::vector<std::size_t>({0, 0, 1}));
    require(stopped.conflicts == 1 && stopped.iterations == 0 && !stopped.converged);

    const auto duplicated = tabucol(
        3, {{0, 1}, {1, 0}, {0, 1}, {1, 2}}, 2, 17, 100
    );
    const auto simple = tabucol(3, path, 2, 17, 100);
    require(duplicated.colors == simple.colors);
    require(duplicated.conflicts == simple.conflicts);
    require(duplicated.iterations == simple.iterations);

    const auto first = tabucol(5, {{0, 1}, {1, 2}, {2, 3}, {3, 4}}, 2, 23, 100);
    const auto second = tabucol(5, {{0, 1}, {1, 2}, {2, 3}, {3, 4}}, 2, 23, 100);
    require(first.colors == second.colors);
    require(first.conflicts == second.conflicts);
    require(first.iterations == second.iterations);

    require_throws<std::overflow_error>([]{
        static_cast<void>(approximate::graph::tabucol(
            3, {{0, 1}, {1, 2}, {2, 0}}, 2, 1, 1,
            std::numeric_limits<std::size_t>::max(), true, {0, 0, 0}
        ));
    });
    std::mt19937_64 generator(0);
    std::size_t tie_count = std::numeric_limits<std::size_t>::max();
    require_throws<std::overflow_error>([&]{
        static_cast<void>(approximate::graph::detail::replace_equal_candidate(
            tie_count, generator
        ));
    });
    require_throws<std::length_error>([]{
        approximate::graph::detail::validate_dense_color_table(
            std::numeric_limits<std::size_t>::max(), 2
        );
    });
}

}  // namespace

int main(){
    using approximate::graph::tabucol;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    boundary_tests();

    std::size_t case_count = 0;
    std::cin >> case_count;
    for(std::size_t case_index = 0; case_index < case_count; ++case_index){
        std::size_t identifier = 0;
        std::size_t vertex_count = 0;
        std::size_t edge_count = 0;
        std::size_t target = 0;
        std::uint64_t seed = 0;
        std::size_t maximum_iterations = 0;
        std::size_t tabu_tenure = 0;
        bool aspiration = false;
        std::size_t chromatic_number = 0;
        bool require_convergence = false;
        bool has_initial = false;
        std::cin >> identifier >> vertex_count >> edge_count >> target >> seed
                 >> maximum_iterations >> tabu_tenure >> aspiration >> chromatic_number
                 >> require_convergence >> has_initial;
        std::vector<std::pair<std::size_t, std::size_t>> edges(edge_count);
        for(auto& [left, right] : edges) std::cin >> left >> right;
        std::vector<std::size_t> initial;
        if(has_initial){
            initial.resize(vertex_count);
            for(std::size_t& color : initial) std::cin >> color;
        }
        const auto result = tabucol(
            vertex_count, edges, target, seed, maximum_iterations, tabu_tenure,
            aspiration, initial
        );
        check_result(
            vertex_count, edges, target, maximum_iterations, chromatic_number,
            require_convergence, result
        );
        const auto repeated = tabucol(
            vertex_count, edges, target, seed, maximum_iterations, tabu_tenure,
            aspiration, initial
        );
        require(result.colors == repeated.colors);
        require(result.conflicts == repeated.conflicts);
        require(result.iterations == repeated.iterations);
        std::cout << "OK " << identifier << '\n';
    }
}
