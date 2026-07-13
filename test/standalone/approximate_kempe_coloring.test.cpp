// competitive-verifier: STANDALONE

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/graph/coloring.hpp"

namespace {

void require(bool condition){
    if(!condition) throw std::runtime_error("Kempe coloring test failed");
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

std::size_t used_color_count(const std::vector<std::size_t>& colors){
    return colors.empty() ? 0 : *std::max_element(colors.begin(), colors.end()) + 1;
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
    require(result.conflicts == 0);
    require(result.iterations <= maximum_iterations);
    for(const std::size_t color : result.colors) require(color < vertex_count);
    for(const auto [left, right] : edges){
        if(left != right) require(result.colors[left] != result.colors[right]);
    }
    const std::size_t used = used_color_count(result.colors);
    std::vector<bool> seen(used, false);
    std::size_t distinct = 0;
    for(const std::size_t color : result.colors){
        if(seen[color]) continue;
        seen[color] = true;
        ++distinct;
    }
    require(distinct == used);
    require(used >= chromatic_number);
    require(result.converged == (used <= target));
    if(target < chromatic_number) require(!result.converged);
    if(require_convergence) require(result.converged);
}

void boundary_tests(){
    using approximate::graph::kempe_chain_interchange;
    require_throws<std::invalid_argument>([]{
        static_cast<void>(kempe_chain_interchange(1, {{0, 0}}, 1, 0));
    });
    require_throws<std::out_of_range>([]{
        static_cast<void>(kempe_chain_interchange(2, {{0, 2}}, 1, 0));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(kempe_chain_interchange(0, {}, 1, 0));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(kempe_chain_interchange(2, {}, 0, 0));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(kempe_chain_interchange(2, {}, 3, 0));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(kempe_chain_interchange(2, {{0, 1}}, 2, 0, 1, {0}));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(kempe_chain_interchange(2, {{0, 1}}, 2, 0, 1, {0, 0}));
    });
    const auto empty = kempe_chain_interchange(0, {}, 0, 0);
    require(empty.colors.empty() && empty.conflicts == 0 && empty.converged);

    const std::size_t maximum = std::numeric_limits<std::size_t>::max();
    const auto canonical = kempe_chain_interchange(2, {}, 1, 7, 2, {maximum, 0});
    require(canonical.converged);
    require(used_color_count(canonical.colors) == 1);

    const std::vector<std::pair<std::size_t, std::size_t>> path{{0, 1}, {0, 2}};
    const auto first = kempe_chain_interchange(3, path, 2, 19, 10, {2, 0, 1});
    const auto second = kempe_chain_interchange(3, path, 2, 19, 10, {2, 0, 1});
    require(first.colors == second.colors);
    require(first.iterations == second.iterations);
    require(first.converged);

    const auto stopped = kempe_chain_interchange(3, path, 2, 19, 0, {2, 0, 1});
    require(!stopped.converged && stopped.iterations == 0);
}

}  // namespace

int main(){
    using approximate::graph::kempe_chain_interchange;
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
        std::size_t chromatic_number = 0;
        bool require_convergence = false;
        bool has_initial = false;
        std::cin >> identifier >> vertex_count >> edge_count >> target >> seed
                 >> maximum_iterations >> chromatic_number >> require_convergence >> has_initial;
        std::vector<std::pair<std::size_t, std::size_t>> edges(edge_count);
        for(auto& [left, right] : edges) std::cin >> left >> right;
        std::vector<std::size_t> initial;
        if(has_initial){
            initial.resize(vertex_count);
            for(std::size_t& color : initial) std::cin >> color;
        }
        const auto result = kempe_chain_interchange(
            vertex_count, edges, target, seed, maximum_iterations, initial
        );
        check_result(
            vertex_count, edges, target, maximum_iterations, chromatic_number,
            require_convergence, result
        );
        const auto repeated = kempe_chain_interchange(
            vertex_count, edges, target, seed, maximum_iterations, initial
        );
        require(result.colors == repeated.colors);
        require(result.iterations == repeated.iterations);
        std::cout << "OK " << identifier << '\n';
    }
}
