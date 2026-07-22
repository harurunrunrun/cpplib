// competitive-verifier: STANDALONE

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/matching/general/general_matching.hpp"
#include "../../src/algorithm/matching/general/micali_vazirani_matching.hpp"

namespace{

using Edge = std::pair<int, int>;

[[noreturn]] void fail(const std::string& message){
    throw std::runtime_error(
        "micali_vazirani_matching.test.cpp: " + message
    );
}

int brute_cardinality(int vertex_count, const std::vector<Edge>& edges){
    if(vertex_count < 0 || 14 < vertex_count){
        fail("the exhaustive oracle only accepts 0 <= n <= 14");
    }
    std::vector<std::uint32_t> adjacency(
        static_cast<std::size_t>(vertex_count), 0
    );
    for(auto [left, right]: edges){
        if(left == right) continue;
        adjacency[static_cast<std::size_t>(left)] |=
            std::uint32_t{1} << right;
        adjacency[static_cast<std::size_t>(right)] |=
            std::uint32_t{1} << left;
    }

    const std::size_t state_count =
        std::size_t{1} << static_cast<unsigned>(vertex_count);
    std::vector<int> memo(state_count, -1);
    auto search = [&](auto&& self, std::uint32_t available) -> int {
        int& cached = memo[static_cast<std::size_t>(available)];
        if(cached != -1) return cached;
        if(available == 0){
            cached = 0;
            return cached;
        }
        const int first = static_cast<int>(std::countr_zero(available));
        const std::uint32_t without_first =
            available & ~(std::uint32_t{1} << first);
        int answer = self(self, without_first);
        std::uint32_t candidates =
            adjacency[static_cast<std::size_t>(first)] & without_first;
        while(candidates != 0){
            const int second =
                static_cast<int>(std::countr_zero(candidates));
            candidates &= candidates - 1;
            answer = std::max(
                answer,
                1 + self(
                    self,
                    without_first & ~(std::uint32_t{1} << second)
                )
            );
        }
        cached = answer;
        return cached;
    };
    return search(
        search,
        static_cast<std::uint32_t>(state_count - std::size_t{1})
    );
}

void validate_result(
    int vertex_count,
    const std::vector<Edge>& input_edges,
    const MicaliVaziraniMatchingResult& result
){
    if(result.mate.size() != static_cast<std::size_t>(vertex_count)){
        fail("mate has the wrong length");
    }
    std::vector<Edge> edges;
    edges.reserve(input_edges.size());
    for(auto [left, right]: input_edges){
        if(left == right) continue;
        if(right < left) std::swap(left, right);
        edges.push_back({left, right});
    }
    std::sort(edges.begin(), edges.end());
    edges.erase(std::unique(edges.begin(), edges.end()), edges.end());

    int cardinality = 0;
    for(int vertex = 0; vertex < vertex_count; vertex++){
        const int mate = result.mate[static_cast<std::size_t>(vertex)];
        if(mate == -1) continue;
        if(mate < 0 || vertex_count <= mate || mate == vertex){
            fail("mate contains an invalid vertex");
        }
        if(result.mate[static_cast<std::size_t>(mate)] != vertex){
            fail("mate is not symmetric");
        }
        if(!std::binary_search(
                edges.begin(),
                edges.end(),
                Edge{std::min(vertex, mate), std::max(vertex, mate)}
            )){
            fail("mate uses a missing edge");
        }
        if(vertex < mate) cardinality++;
    }
    if(cardinality != result.cardinality){
        fail("cardinality and mate disagree");
    }
}

MicaliVaziraniMatchingResult solve_checked(
    int vertex_count, const std::vector<Edge>& edges
){
    MicaliVaziraniMatching graph(vertex_count);
    for(const auto [left, right]: edges) graph.add_edge(left, right);
    const auto result = graph.solve();
    validate_result(vertex_count, edges, result);
    return result;
}

void compare_with_brute(int vertex_count, const std::vector<Edge>& edges){
    const auto result = solve_checked(vertex_count, edges);
    const int expected = brute_cardinality(vertex_count, edges);
    if(result.cardinality != expected){
        fail(
            "exhaustive oracle mismatch: expected "
            + std::to_string(expected) + ", got "
            + std::to_string(result.cardinality)
        );
    }
}

void compare_with_legacy(int vertex_count, const std::vector<Edge>& edges){
    MicaliVaziraniMatching fast(vertex_count);
    GeneralMatching legacy(vertex_count);
    for(const auto [left, right]: edges){
        fast.add_edge(left, right);
        legacy.add_edge(left, right);
    }
    const auto fast_result = fast.solve();
    const auto legacy_result = legacy.solve();
    validate_result(vertex_count, edges, fast_result);
    if(fast_result.cardinality != legacy_result.size){
        fail(
            "legacy blossom mismatch: expected "
            + std::to_string(legacy_result.size) + ", got "
            + std::to_string(fast_result.cardinality)
        );
    }
}

void test_contracts(){
    bool thrown = false;
    try{
        [[maybe_unused]] MicaliVaziraniMatching invalid(-1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    if(!thrown) fail("negative vertex count was accepted");

    MicaliVaziraniMatching graph(4);
    if(graph.vertex_count() != 4 || graph.edge_count() != 0){
        fail("initial metadata is wrong");
    }
    graph.add_edge(0, 0);
    if(graph.edge_count() != 0) fail("a self-loop was retained");

    for(const Edge invalid: std::vector<Edge>{{-1, 0}, {0, -1}, {4, 0}, {0, 4}}){
        thrown = false;
        try{
            graph.add_edge(invalid.first, invalid.second);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        if(!thrown) fail("an out-of-range endpoint was accepted");
    }

    graph.add_edge(0, 1);
    graph.add_edge(0, 1);
    if(graph.edge_count() != 2) fail("parallel-edge metadata is wrong");
    const MicaliVaziraniMatching& constant_graph = graph;
    const auto first = constant_graph.solve();
    const auto repeated = constant_graph.solve();
    validate_result(4, {{0, 1}, {0, 1}}, first);
    validate_result(4, {{0, 1}, {0, 1}}, repeated);
    if(first.cardinality != 1 || repeated.cardinality != 1){
        fail("const or repeated solve is wrong");
    }

    graph.add_edge(2, 3);
    const auto extended = graph.solve();
    validate_result(4, {{0, 1}, {0, 1}, {2, 3}}, extended);
    if(extended.cardinality != 2){
        fail("solve after add_edge is wrong");
    }

    const auto empty = solve_checked(0, {});
    if(empty.cardinality != 0) fail("the empty graph is wrong");
    const auto isolated = solve_checked(1000, {});
    if(isolated.cardinality != 0) fail("isolated vertices are wrong");
}

void test_all_graphs_through_six_vertices(){
    for(int vertex_count = 0; vertex_count <= 6; vertex_count++){
        std::vector<Edge> universe;
        for(int left = 0; left < vertex_count; left++){
            for(int right = left + 1; right < vertex_count; right++){
                universe.push_back({left, right});
            }
        }
        const std::uint32_t graph_count =
            std::uint32_t{1} << static_cast<unsigned>(universe.size());
        for(std::uint32_t mask = 0; mask < graph_count; mask++){
            std::vector<Edge> edges;
            for(std::size_t index = 0; index < universe.size(); index++){
                if(((mask >> index) & 1U) != 0){
                    edges.push_back(universe[index]);
                }
            }
            compare_with_brute(vertex_count, edges);
        }
    }
}

void test_random_exhaustive_oracle(
    std::mt19937_64& random, int trial_count
){
    for(int trial = 0; trial < trial_count; trial++){
        const int vertex_count = 7 + static_cast<int>(random() % 8);
        const unsigned density = static_cast<unsigned>(random() % 101);
        std::vector<Edge> edges;
        for(int left = 0; left < vertex_count; left++){
            for(int right = left + 1; right < vertex_count; right++){
                if(random() % 100 < density){
                    edges.push_back({left, right});
                    if(random() % 37 == 0) edges.push_back({left, right});
                }
            }
        }
        std::shuffle(edges.begin(), edges.end(), random);
        compare_with_brute(vertex_count, edges);
    }
}

void test_random_legacy_differential(
    std::mt19937_64& random, int trial_count
){
    for(int trial = 0; trial < trial_count; trial++){
        const int vertex_count = static_cast<int>(random() % 97);
        unsigned density = static_cast<unsigned>(random() % 101);
        if((trial & 3) != 0) density %= 18;
        std::vector<Edge> edges;
        for(int left = 0; left < vertex_count; left++){
            for(int right = left + 1; right < vertex_count; right++){
                if(random() % 100 < density){
                    edges.push_back({left, right});
                    if(random() % 43 == 0) edges.push_back({left, right});
                }
            }
        }
        std::shuffle(edges.begin(), edges.end(), random);
        compare_with_legacy(vertex_count, edges);
    }
}

void test_blossoms_and_bridges(){
    const std::vector<Edge> parallel_regression{
        {0, 2}, {0, 8}, {1, 2}, {1, 3}, {1, 3}, {1, 4},
        {3, 8}, {4, 6}, {5, 7}, {5, 9}, {6, 7}
    };
    compare_with_legacy(10, parallel_regression);

    const std::vector<Edge> petersen{
        {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0},
        {5, 7}, {7, 9}, {9, 6}, {6, 8}, {8, 5},
        {0, 5}, {1, 6}, {2, 7}, {3, 8}, {4, 9}
    };
    compare_with_brute(10, petersen);

    for(int cycle_size = 3; cycle_size <= 51; cycle_size += 2){
        std::vector<Edge> edges;
        for(int vertex = 0; vertex < cycle_size; vertex++){
            edges.push_back({vertex, (vertex + 1) % cycle_size});
        }
        compare_with_legacy(cycle_size, edges);

        const int stem_begin = cycle_size;
        for(int vertex = 0; vertex + 1 < 10; vertex++){
            edges.push_back({stem_begin + vertex, stem_begin + vertex + 1});
        }
        edges.push_back({0, stem_begin});
        edges.push_back({cycle_size / 2, stem_begin + 4});
        edges.push_back({cycle_size - 1, stem_begin + 9});
        compare_with_legacy(cycle_size + 10, edges);
    }

    {
        std::vector<Edge> edges;
        constexpr int triangle_count = 80;
        for(int triangle = 0; triangle < triangle_count; triangle++){
            const int base = 3 * triangle;
            edges.push_back({base, base + 1});
            edges.push_back({base + 1, base + 2});
            edges.push_back({base + 2, base});
            if(triangle != 0){
                edges.push_back({base - 1, base});
                edges.push_back({base - 2, base + 1});
            }
        }
        compare_with_legacy(3 * triangle_count, edges);
    }

    {
        std::vector<Edge> edges;
        constexpr int ring_count = 14;
        constexpr int ring_size = 9;
        for(int ring = 0; ring < ring_count; ring++){
            const int base = ring * ring_size;
            for(int offset = 0; offset < ring_size; offset++){
                edges.push_back({
                    base + offset,
                    base + (offset + 1) % ring_size
                });
            }
            edges.push_back({base, base + 3});
            edges.push_back({base + 3, base + 6});
            if(ring != 0){
                edges.push_back({base - ring_size, base});
                edges.push_back({base - 1, base + 4});
            }
        }
        compare_with_legacy(ring_count * ring_size, edges);
    }
}

void test_large_sparse(std::mt19937_64& random, int requested_size){
    const int vertex_count = std::max(2, requested_size);
    std::vector<Edge> edges;
    edges.reserve(static_cast<std::size_t>(vertex_count) * 6);
    for(int vertex = 0; vertex + 1 < vertex_count; vertex += 2){
        edges.push_back({vertex, vertex + 1});
    }
    for(int vertex = 0; vertex < vertex_count; vertex++){
        edges.push_back({vertex, (vertex + 1) % vertex_count});
        edges.push_back({vertex, (vertex + 3) % vertex_count});
        for(int repeat = 0; repeat < 2; repeat++){
            int to = static_cast<int>(
                random() % static_cast<std::uint64_t>(vertex_count)
            );
            if(to == vertex) to = (to + 1) % vertex_count;
            edges.push_back({vertex, to});
        }
    }

    const auto result = solve_checked(vertex_count, edges);
    if(result.cardinality != vertex_count / 2){
        fail("large sparse graph lost its planted maximum matching");
    }
}

}  // namespace

int main(){
    std::uint64_t seed = UINT64_C(0x6d766d6174636869);
    int exhaustive_trials = 1800;
    int differential_trials = 6000;
    int performance_size = 30000;
    if(std::cin >> seed){
        if(!(std::cin >> exhaustive_trials
                >> differential_trials >> performance_size)){
            fail("incomplete test configuration");
        }
    }
    if(exhaustive_trials < 0
            || differential_trials < 0 || performance_size < 0){
        fail("negative test configuration");
    }

    std::mt19937_64 random(seed);
    test_contracts();
    test_all_graphs_through_six_vertices();
    test_random_exhaustive_oracle(random, exhaustive_trials);
    test_random_legacy_differential(random, differential_trials);
    test_blossoms_and_bridges();
    test_large_sparse(random, performance_size);
    std::cout << "OK\n";
}
