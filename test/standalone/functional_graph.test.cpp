// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

#include "../../src/algorithm/graph/traversal/functional_graph.hpp"

namespace{

struct NaiveOrbit{
    std::vector<int> path;
    int cycle_begin;
};

NaiveOrbit naive_orbit(const std::vector<int>& successor, int start){
    std::vector<int> first(successor.size(), -1);
    std::vector<int> path;
    int v = start;
    while(first[static_cast<std::size_t>(v)] == -1){
        first[static_cast<std::size_t>(v)] = static_cast<int>(path.size());
        path.push_back(v);
        v = successor[static_cast<std::size_t>(v)];
    }
    return {path, first[static_cast<std::size_t>(v)]};
}

int naive_jump(const std::vector<int>& successor, int start, unsigned long long step){
    const auto orbit = naive_orbit(successor, start);
    if(step < orbit.path.size()) return orbit.path[static_cast<std::size_t>(step)];
    const unsigned long long cycle_length = orbit.path.size() - orbit.cycle_begin;
    const unsigned long long index = static_cast<unsigned long long>(orbit.cycle_begin) +
        (step - static_cast<unsigned long long>(orbit.cycle_begin)) % cycle_length;
    return orbit.path[static_cast<std::size_t>(index)];
}

long long naive_steps_to(const std::vector<int>& successor, int from, int to){
    int v = from;
    for(int step = 0; step <= static_cast<int>(successor.size()); ++step){
        if(v == to) return step;
        v = successor[static_cast<std::size_t>(v)];
    }
    return -1;
}

template<int MAX_SIZE>
void check_graph(const std::vector<int>& successor, const FunctionalGraph<MAX_SIZE>& graph){
    const int n = static_cast<int>(successor.size());
    assert(graph.size() == n);
    assert(graph.empty() == (n == 0));
    int component_size_sum = 0;
    for(int id = 0; id < graph.component_count(); ++id){
        const auto cycle = graph.cycle(id);
        assert(!cycle.empty());
        assert(static_cast<int>(cycle.size()) == graph.cycle_size(id));
        for(int i = 0; i < static_cast<int>(cycle.size()); ++i){
            assert(graph.component_id(cycle[static_cast<std::size_t>(i)]) == id);
            assert(graph.cycle_position(cycle[static_cast<std::size_t>(i)]) == i);
            assert(successor[static_cast<std::size_t>(cycle[static_cast<std::size_t>(i)])] ==
                cycle[static_cast<std::size_t>((i + 1) % cycle.size())]);
        }
        component_size_sum += graph.component_size(id);
    }
    assert(component_size_sum == n);

    constexpr std::array<unsigned long long, 10> steps = {
        0ULL,
        1ULL,
        2ULL,
        3ULL,
        63ULL,
        64ULL,
        65ULL,
        1000000000000000000ULL,
        std::numeric_limits<unsigned long long>::max() - 1,
        std::numeric_limits<unsigned long long>::max()
    };
    for(int v = 0; v < n; ++v){
        const auto orbit = naive_orbit(successor, v);
        const int entry = orbit.path[static_cast<std::size_t>(orbit.cycle_begin)];
        const int length = static_cast<int>(orbit.path.size()) - orbit.cycle_begin;
        assert(graph.successor(v) == successor[static_cast<std::size_t>(v)]);
        assert(graph.component_id(v) == graph.cycle_id(v));
        assert(graph.on_cycle(v) == (orbit.cycle_begin == 0));
        assert(graph.distance_to_cycle(v) == orbit.cycle_begin);
        assert(graph.cycle_entry(v) == entry);
        assert(graph.cycle_length(v) == length);
        assert(graph.orbit_size(v) == static_cast<int>(orbit.path.size()));
        assert(graph.cycle_position(v) ==
            (orbit.cycle_begin == 0 ? graph.cycle_position(entry) : -1));
        for(unsigned long long step: steps){
            assert(graph.jump(v, step) == naive_jump(successor, v, step));
        }
        for(int to = 0; to < n; ++to){
            assert(graph.steps_to(v, to) == naive_steps_to(successor, v, to));
        }
    }
}

void self_test(){
    {
        FunctionalGraph<0> graph(std::array<int, 0>{});
        assert(graph.empty());
        assert(graph.component_count() == 0);
    }
    {
        const std::array<int, 6> successor = {1, 2, 0, 1, 3, 5};
        FunctionalGraph<6> graph(successor);
        check_graph(std::vector<int>(successor.begin(), successor.end()), graph);
        assert(graph.distance_to_cycle(4) == 2);
        assert(graph.cycle_entry(4) == 1);
        assert(graph.cycle_length(4) == 3);
        assert(graph.steps_to(4, 2) == 3);
        assert(graph.steps_to(2, 4) == -1);
    }

    std::mt19937 rng(20261020);
    for(int n = 1; n <= 40; ++n){
        for(int iteration = 0; iteration < 100; ++iteration){
            std::vector<int> successor(static_cast<std::size_t>(n));
            for(int& to: successor) to = static_cast<int>(rng() % static_cast<unsigned>(n));
            FunctionalGraph<40> graph(successor);
            check_graph(successor, graph);
        }
    }
}

} // namespace

int main(){
    int n, query_count;
    if(!(std::cin >> n >> query_count)){
        self_test();
        return 0;
    }
    std::vector<int> successor(static_cast<std::size_t>(n));
    for(int& to: successor) std::cin >> to;
    static FunctionalGraph<50000> graph;
    graph.build(successor);
    std::cout << graph.size() << ' ' << graph.empty() << ' '
              << graph.component_count() << '\n';
    for(int query = 0; query < query_count; ++query){
        int type;
        std::cin >> type;
        if(type == 0){
            int v;
            std::cin >> v;
            std::cout << graph.successor(v) << ' '
                      << graph.component_id(v) << ' '
                      << graph.cycle_id(v) << ' '
                      << graph.on_cycle(v) << ' '
                      << graph.distance_to_cycle(v) << ' '
                      << graph.cycle_entry(v) << ' '
                      << graph.cycle_position(v) << ' '
                      << graph.cycle_length(v) << ' '
                      << graph.orbit_size(v) << '\n';
        }else if(type == 1){
            int v;
            unsigned long long step;
            std::cin >> v >> step;
            std::cout << graph.jump(v, step) << '\n';
        }else if(type == 2){
            int from, to;
            std::cin >> from >> to;
            std::cout << graph.steps_to(from, to) << '\n';
        }else{
            int id;
            std::cin >> id;
            const auto cycle = graph.cycle(id);
            std::cout << graph.component_size(id) << ' ' << graph.cycle_size(id);
            for(int v: cycle) std::cout << ' ' << v;
            std::cout << '\n';
        }
    }
}
