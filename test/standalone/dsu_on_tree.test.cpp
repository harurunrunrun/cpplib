// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/tree/dsu_on_tree.hpp"

namespace{

constexpr int MAX_TEST_SIZE = 512;

[[gnu::noinline]]
void run_before_build(DsuOnTree<3>& sack){
    sack.run([](int){}, [](int){}, [](int){});
}

template<class Sack>
std::vector<std::pair<int, int>> solve_with_remove(
    Sack& sack,
    const std::vector<int>& color
){
    const int n = sack.size();
    std::vector<int> count(static_cast<std::size_t>(n), 0);
    std::vector<int> frequency(static_cast<std::size_t>(n + 1), 0);
    std::vector<std::pair<int, int>> answer(static_cast<std::size_t>(n));
    int maximum = 0;

    sack.run(
        [&](int vertex){
            const int value = color[static_cast<std::size_t>(vertex)];
            const int before = count[static_cast<std::size_t>(value)];
            if(before > 0) --frequency[static_cast<std::size_t>(before)];
            const int after = ++count[static_cast<std::size_t>(value)];
            ++frequency[static_cast<std::size_t>(after)];
            maximum = std::max(maximum, after);
        },
        [&](int vertex){
            const int value = color[static_cast<std::size_t>(vertex)];
            const int before = count[static_cast<std::size_t>(value)];
            --frequency[static_cast<std::size_t>(before)];
            const int after = --count[static_cast<std::size_t>(value)];
            if(after > 0) ++frequency[static_cast<std::size_t>(after)];
            while(maximum > 0 &&
                frequency[static_cast<std::size_t>(maximum)] == 0){
                --maximum;
            }
        },
        [&](int vertex){
            answer[static_cast<std::size_t>(vertex)] = {
                maximum,
                frequency[static_cast<std::size_t>(maximum)]
            };
        }
    );

    assert(maximum == 0);
    for(int value : count) assert(value == 0);
    return answer;
}

template<class Sack>
std::vector<std::pair<int, int>> solve_with_reset(
    Sack& sack,
    const std::vector<int>& color
){
    const int n = sack.size();
    std::vector<int> count(static_cast<std::size_t>(n), 0);
    std::vector<int> frequency(static_cast<std::size_t>(n + 1), 0);
    std::vector<int> touched;
    std::vector<std::pair<int, int>> answer(static_cast<std::size_t>(n));
    int maximum = 0;

    sack.run_with_reset(
        [&](int vertex){
            const int value = color[static_cast<std::size_t>(vertex)];
            if(count[static_cast<std::size_t>(value)] == 0){
                touched.push_back(value);
            }
            const int before = count[static_cast<std::size_t>(value)];
            if(before > 0) --frequency[static_cast<std::size_t>(before)];
            const int after = ++count[static_cast<std::size_t>(value)];
            ++frequency[static_cast<std::size_t>(after)];
            maximum = std::max(maximum, after);
        },
        [&]{
            for(int value : touched){
                count[static_cast<std::size_t>(value)] = 0;
            }
            std::fill(frequency.begin(), frequency.begin() + maximum + 1, 0);
            touched.clear();
            maximum = 0;
        },
        [&](int vertex){
            answer[static_cast<std::size_t>(vertex)] = {
                maximum,
                frequency[static_cast<std::size_t>(maximum)]
            };
        }
    );

    assert(maximum == 0);
    assert(touched.empty());
    for(int value : count) assert(value == 0);
    return answer;
}

std::vector<std::pair<int, int>> solve_naive(
    int n,
    int root,
    const std::vector<int>& color,
    const std::vector<std::pair<int, int>>& edges
){
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    for(auto [u, v] : edges){
        graph[static_cast<std::size_t>(u)].push_back(v);
        graph[static_cast<std::size_t>(v)].push_back(u);
    }
    std::vector<int> parent(static_cast<std::size_t>(n), -2);
    std::vector<int> order = {root};
    parent[static_cast<std::size_t>(root)] = -1;
    for(std::size_t index = 0; index < order.size(); ++index){
        const int vertex = order[index];
        for(int to : graph[static_cast<std::size_t>(vertex)]){
            if(to == parent[static_cast<std::size_t>(vertex)]) continue;
            parent[static_cast<std::size_t>(to)] = vertex;
            order.push_back(to);
        }
    }

    std::vector<std::pair<int, int>> answer(static_cast<std::size_t>(n));
    for(int root_vertex = 0; root_vertex < n; ++root_vertex){
        std::vector<int> count(static_cast<std::size_t>(n), 0);
        std::vector<int> stack = {root_vertex};
        int maximum = 0;
        while(!stack.empty()){
            const int vertex = stack.back();
            stack.pop_back();
            maximum = std::max(
                maximum,
                ++count[static_cast<std::size_t>(
                    color[static_cast<std::size_t>(vertex)]
                )]
            );
            for(int to : graph[static_cast<std::size_t>(vertex)]){
                if(parent[static_cast<std::size_t>(to)] == vertex){
                    stack.push_back(to);
                }
            }
        }
        int modes = 0;
        for(int value : count) modes += value == maximum;
        answer[static_cast<std::size_t>(root_vertex)] = {maximum, modes};
    }
    return answer;
}

void test_fixed_and_naive(){
    const std::vector<std::pair<int, int>> edges = {
        {0, 1}, {0, 2}, {1, 3}, {1, 4}, {2, 5}, {5, 6}
    };
    const std::vector<int> color = {0, 1, 0, 1, 2, 2, 2};
    DsuOnTree<8> sack(7);
    assert(sack.size() == 7);
    assert(sack.edge_count() == 0);
    for(auto [u, v] : edges) sack.add_edge(u, v);
    assert(sack.edge_count() == 6);
    sack.build(0);

    assert(sack.root() == 0);
    assert(sack.parent(0) == -1);
    assert(sack.subtree_size(0) == 7);
    assert(sack.subtree_size(5) == 2);
    assert(sack.heavy_child(0) == 1 || sack.heavy_child(0) == 2);
    for(int vertex = 0; vertex < 7; ++vertex){
        const auto [left, right] = sack.subtree_range(vertex);
        assert(sack.tin(vertex) == left);
        assert(sack.tout(vertex) == right);
        assert(right - left == sack.subtree_size(vertex));
        assert(sack.preorder_vertex(left) == vertex);
    }

    const auto expected = solve_naive(7, 0, color, edges);
    assert(solve_with_remove(sack, color) == expected);
    assert(solve_with_reset(sack, color) == expected);
}

void test_empty_and_exceptions(){
    DsuOnTree<0> empty;
    assert(empty.size() == 0);
    assert(empty.edge_count() == 0);
    empty.build();
    assert(empty.root() == -1);
    int calls = 0;
    empty.run(
        [&](int){ ++calls; },
        [&](int){ ++calls; },
        [&](int){ ++calls; }
    );
    empty.run_with_reset(
        [&](int){ ++calls; },
        [&]{ ++calls; },
        [&](int){ ++calls; }
    );
    assert(calls == 0);

    bool thrown = false;
    try{
        DsuOnTree<2> invalid(3);
        (void)invalid;
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    DsuOnTree<3> not_built_metadata(3);
    thrown = false;
    try{
        (void)not_built_metadata.tin(0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        DsuOnTree<3> not_built(3);
        run_before_build(not_built);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        DsuOnTree<3> cycle(3);
        cycle.add_edge(0, 1);
        cycle.add_edge(0, 1);
        cycle.build();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void test_random_against_naive(){
    std::mt19937 random(20260713);
    for(int n = 1; n <= 80; ++n){
        for(int iteration = 0; iteration < 20; ++iteration){
            std::vector<std::pair<int, int>> edges;
            for(int vertex = 1; vertex < n; ++vertex){
                edges.push_back({
                    static_cast<int>(random() % static_cast<unsigned>(vertex)),
                    vertex
                });
            }
            std::shuffle(edges.begin(), edges.end(), random);
            const int root =
                static_cast<int>(random() % static_cast<unsigned>(n));
            std::vector<int> color(static_cast<std::size_t>(n));
            const int color_count = 1 +
                static_cast<int>(random() % static_cast<unsigned>(n));
            for(int& value : color){
                value = static_cast<int>(
                    random() % static_cast<unsigned>(color_count)
                );
            }

            DsuOnTree<80> sack(n);
            for(auto [u, v] : edges) sack.add_edge(u, v);
            sack.build(root);
            const auto expected = solve_naive(n, root, color, edges);
            assert(solve_with_remove(sack, color) == expected);
            assert(solve_with_reset(sack, color) == expected);
        }
    }
}

void test_deep_path(){
    constexpr int N = 200000;
    static DsuOnTree<N> path(N);
    for(int vertex = 1; vertex < N; ++vertex){
        path.add_edge(vertex - 1, vertex);
    }
    path.build(0);

    int active = 0;
    int answers = 0;
    int additions = 0;
    int removals = 0;
    path.run(
        [&](int){ ++active; ++additions; },
        [&](int){ --active; ++removals; },
        [&](int vertex){
            assert(active == path.subtree_size(vertex));
            ++answers;
        }
    );
    assert(active == 0);
    assert(answers == N);
    assert(additions == N);
    assert(removals == N);

    active = 0;
    answers = 0;
    int resets = 0;
    path.run_with_reset(
        [&](int){ ++active; },
        [&]{ active = 0; ++resets; },
        [&](int vertex){
            assert(active == path.subtree_size(vertex));
            ++answers;
        }
    );
    assert(active == 0);
    assert(answers == N);
    assert(resets == 1);
}

void self_test(){
    test_fixed_and_naive();
    test_empty_and_exceptions();
    test_random_against_naive();
    test_deep_path();
}

} // namespace

int main(){
    int test_count;
    if(!(std::cin >> test_count)){
        self_test();
        return 0;
    }

    while(test_count--){
        int n, root;
        std::cin >> n >> root;
        std::vector<int> color(static_cast<std::size_t>(n));
        for(int& value : color) std::cin >> value;

        DsuOnTree<MAX_TEST_SIZE> sack(n);
        for(int edge = 0; edge + 1 < n; ++edge){
            int u, v;
            std::cin >> u >> v;
            sack.add_edge(u, v);
        }
        sack.build(root);

        const auto first = solve_with_remove(sack, color);
        const auto second = solve_with_reset(sack, color);
        assert(first == second);
        for(auto [maximum, modes] : first){
            std::cout << maximum << ' ' << modes << '\n';
        }
    }
}
