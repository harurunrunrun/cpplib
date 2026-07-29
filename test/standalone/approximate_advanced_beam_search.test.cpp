// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/search/advanced_beam_search.hpp"
#include "../../src/approximate/search/bounded_heuristic_search.hpp"

using Edge = std::pair<int, int>;
using Graph = std::vector<std::vector<Edge>>;

void behavior_tests(){
    Graph graph(4);
    graph[0] = {{1, 1}, {2, 1}};
    graph[1] = {};
    graph[2] = {{3, 1}};
    const auto neighbors = [&](int vertex){
        return graph[static_cast<std::size_t>(vertex)];
    };
    const auto goal = [](int vertex){ return vertex == 3; };
    const auto misleading = [](int vertex){
        return vertex == 1 ? 0 : (vertex == 2 ? 1 : 0);
    };

    assert(!beam_search(0, goal, neighbors, misleading, 1, 2));
    const auto stack = beam_stack_search(0, goal, neighbors, misleading, 1, 2);
    assert(stack && *stack.cost == 2);

    const auto beam_a = beam_a_star_search(
        0, goal, neighbors, misleading, 2, 2
    );
    assert(beam_a && *beam_a.cost == 2);

    const auto beam_ida = beam_ida_star_search(
        0, goal, neighbors, [](int){ return 0; }, 2, 2, 4
    );
    assert(beam_ida && *beam_ida.cost == 2);

    assert(!chokudai_search(0, goal, neighbors, misleading, 1, 2, 1));
    const auto chokudai =
        chokudai_search(0, goal, neighbors, misleading, 1, 2, 2);
    assert(chokudai && *chokudai.cost == 2);

    std::mt19937_64 generator(123456789);
    const auto stochastic = stochastic_beam_search(
        0, goal, neighbors, misleading, 2, 2, generator
    );
    assert(stochastic && *stochastic.cost == 2);

    bool threw = false;
    try{
        static_cast<void>(beam_stack_search(
            0, goal, neighbors, misleading, 0, 2
        ));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);

    threw = false;
    try{
        static_cast<void>(stochastic_beam_search(
            0, goal, neighbors, misleading, 1, 2, generator, 0.0L
        ));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
}

int main(){
    behavior_tests();

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    for(int case_index = 0; case_index < test_count; ++case_index){
        int vertex_count;
        int edge_count;
        int source;
        int target;
        std::cin >> vertex_count >> edge_count >> source >> target;
        Graph graph(static_cast<std::size_t>(vertex_count));
        for(int edge_index = 0; edge_index < edge_count; ++edge_index){
            int from;
            int to;
            std::cin >> from >> to;
            graph[static_cast<std::size_t>(from)].emplace_back(to, 1);
        }
        const auto neighbors = [&](int vertex){
            return graph[static_cast<std::size_t>(vertex)];
        };
        const auto goal = [target](int vertex){ return vertex == target; };
        const auto zero = [](int){ return 0; };
        const std::size_t width = static_cast<std::size_t>(vertex_count);
        const std::size_t depth = static_cast<std::size_t>(vertex_count);
        const std::size_t expansions = std::numeric_limits<std::size_t>::max();
        std::mt19937_64 generator(
            0x9e3779b97f4a7c15ULL + static_cast<unsigned>(case_index)
        );

        const auto stochastic = stochastic_beam_search(
            source, goal, neighbors, zero, width, depth, generator
        );
        const auto stack = beam_stack_search(
            source, goal, neighbors, zero, width, depth, expansions
        );
        const auto beam_a = beam_a_star_search(
            source, goal, neighbors, zero, width, depth, expansions
        );
        const auto beam_ida = beam_ida_star_search(
            source, goal, neighbors, zero, width, depth, depth + 2
        );
        const auto chokudai = chokudai_search(
            source, goal, neighbors, zero, width, depth, depth + 1
        );
        std::cout
            << stochastic.cost.value_or(-1) << ' '
            << stack.cost.value_or(-1) << ' '
            << beam_a.cost.value_or(-1) << ' '
            << beam_ida.cost.value_or(-1) << ' '
            << chokudai.cost.value_or(-1) << '\n';
    }
}
