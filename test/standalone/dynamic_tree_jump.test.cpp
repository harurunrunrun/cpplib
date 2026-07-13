// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <queue>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/structure/tree/lazy_link_cut_tree.hpp"
#include "../../src/structure/tree/link_cut_tree.hpp"
#include "../../src/structure/types/monoid.hpp"
#include "../../src/structure/types/monoid_act_len.hpp"

long long add_values(long long left, long long right){ return left + right; }
long long zero_value(){ return 0; }
constexpr Monoid<add_values, zero_value> sum_monoid;

long long add_with_length(long long left, long long, long long right, long long){
    return left + right;
}
long long map_add(long long action, long long value, long long length){
    return value + action * length;
}
long long compose_add(long long action, long long accumulated){
    return action + accumulated;
}
constexpr Monoid_Act_Len<add_with_length, zero_value, map_add, compose_add, zero_value>
    range_add_sum;

std::vector<int> naive_path(int start,
                            int goal,
                            const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    std::vector<int> parent(static_cast<std::size_t>(n), -1);
    std::queue<int> queue;
    parent[static_cast<std::size_t>(start)] = start;
    queue.push(start);
    while(!queue.empty()){
        const int vertex = queue.front();
        queue.pop();
        if(vertex == goal) break;
        for(const int next: graph[static_cast<std::size_t>(vertex)]){
            if(parent[static_cast<std::size_t>(next)] == -1){
                parent[static_cast<std::size_t>(next)] = vertex;
                queue.push(next);
            }
        }
    }
    if(parent[static_cast<std::size_t>(goal)] == -1) return {};
    std::vector<int> result;
    for(int vertex = goal;; vertex = parent[static_cast<std::size_t>(vertex)]){
        result.push_back(vertex);
        if(vertex == start) break;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

void add_edge(int left, int right, std::vector<std::vector<int>>& graph){
    graph[static_cast<std::size_t>(left)].push_back(right);
    graph[static_cast<std::size_t>(right)].push_back(left);
}

void remove_edge(int left, int right, std::vector<std::vector<int>>& graph){
    auto& left_edges = graph[static_cast<std::size_t>(left)];
    left_edges.erase(std::find(left_edges.begin(), left_edges.end(), right));
    auto& right_edges = graph[static_cast<std::size_t>(right)];
    right_edges.erase(std::find(right_edges.begin(), right_edges.end(), left));
}

bool has_edge(int left, int right, const std::vector<std::vector<int>>& graph){
    const auto& edges = graph[static_cast<std::size_t>(left)];
    return std::find(edges.begin(), edges.end(), right) != edges.end();
}

template<class PlainTree, class LazyTree>
void check_path(PlainTree& plain,
                LazyTree& lazy,
                int start,
                int goal,
                const std::vector<std::vector<int>>& graph){
    const std::vector<int> path = naive_path(start, goal, graph);
    const int expected_size = path.empty() ? -1 : static_cast<int>(path.size());
    assert(plain.path_size(start, goal) == expected_size);
    assert(lazy.path_size(start, goal) == expected_size);

    std::vector<int> indices{-1, 0, 1, expected_size - 1, expected_size, expected_size + 1};
    for(const int index: indices){
        const int expected = index >= 0 && index < expected_size ?
            path[static_cast<std::size_t>(index)] : -1;
        assert(plain.kth_on_path(start, goal, index) == expected);
        assert(lazy.kth_on_path(start, goal, index) == expected);
        assert(plain.jump(start, goal, index) == expected);
        assert(lazy.jump(start, goal, index) == expected);
    }
}

void test_random_forest(){
    constexpr int n = 80;
    std::vector<long long> initial(static_cast<std::size_t>(n), 1);
    LinkCutTree<sum_monoid, 100> plain(initial);
    LazyLinkCutTree<range_add_sum, 100> lazy(initial);
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    std::vector<std::pair<int, int>> edges;
    std::mt19937 rng(2026071331);

    for(int step = 0; step < 18000; ++step){
        const int operation = static_cast<int>(rng() % 12U);
        if(operation < 3){
            std::vector<std::pair<int, int>> candidates;
            for(int left = 0; left < n; ++left){
                for(int right = left + 1; right < n; ++right){
                    if(naive_path(left, right, graph).empty()){
                        candidates.emplace_back(left, right);
                    }
                }
            }
            if(!candidates.empty()){
                const auto [left, right] =
                    candidates[static_cast<std::size_t>(rng() % candidates.size())];
                assert(plain.link(left, right));
                assert(lazy.link(left, right));
                add_edge(left, right, graph);
                edges.emplace_back(left, right);
            }
        }else if(operation < 5 && !edges.empty()){
            const std::size_t index = static_cast<std::size_t>(rng() % edges.size());
            const auto [left, right] = edges[index];
            assert(plain.cut(left, right));
            assert(lazy.cut(left, right));
            remove_edge(left, right, graph);
            edges.erase(edges.begin() + static_cast<std::ptrdiff_t>(index));
            check_path(plain, lazy, left, right, graph);
        }else if(operation == 5){
            const int left = static_cast<int>(rng() % n);
            const int right = static_cast<int>(rng() % n);
            const bool connected = !naive_path(left, right, graph).empty();
            assert(plain.link(left, right) == !connected);
            assert(lazy.link(left, right) == !connected);
            if(!connected){
                add_edge(left, right, graph);
                edges.emplace_back(left, right);
            }
        }else if(operation == 6){
            int left = static_cast<int>(rng() % n);
            int right = static_cast<int>(rng() % n);
            for(int attempt = 0; attempt < n && has_edge(left, right, graph); ++attempt){
                right = (right + 1) % n;
            }
            if(!has_edge(left, right, graph)){
                assert(!plain.cut(left, right));
                assert(!lazy.cut(left, right));
            }
        }else if(operation == 7){
            const int vertex = static_cast<int>(rng() % n);
            plain.evert(vertex);
            lazy.evert(vertex);
            assert(plain.root(vertex) == vertex);
            assert(lazy.root(vertex) == vertex);
        }else{
            const int start = static_cast<int>(rng() % n);
            const int goal = static_cast<int>(rng() % n);
            check_path(plain, lazy, start, goal, graph);
            const bool expected_connected = !naive_path(start, goal, graph).empty();
            assert(plain.connected(start, goal) == expected_connected);
            assert(lazy.connected(start, goal) == expected_connected);
        }
    }
}

template<class Tree>
void check_invalid_vertices(Tree& tree){
    bool thrown = false;
    try{ (void)tree.path_size(-1, 0); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)tree.kth_on_path(0, tree.size(), 0); }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{ (void)tree.jump(tree.size(), 0, 0); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
}

void test_large_chain(){
    constexpr int n = 40000;
    std::vector<long long> initial(static_cast<std::size_t>(n), 1);
    LinkCutTree<sum_monoid, n> plain(initial);
    LazyLinkCutTree<range_add_sum, n> lazy(initial);
    for(int vertex = 1; vertex < n; ++vertex){
        assert(plain.link(vertex - 1, vertex));
        assert(lazy.link(vertex - 1, vertex));
    }
    lazy.path_apply(0, n - 1, 5);
    assert(plain.path_prod(0, n - 1) == n);
    assert(lazy.path_prod(0, n - 1) == 6LL * n);

    std::mt19937 rng(2026071332);
    for(int iteration = 0; iteration < 25000; ++iteration){
        const int start = static_cast<int>(rng() % n);
        const int goal = static_cast<int>(rng() % n);
        const int length = std::abs(goal - start) + 1;
        const int index = static_cast<int>(rng() % static_cast<unsigned>(length + 2)) - 1;
        const int expected = index >= 0 && index < length ?
            start + (goal >= start ? index : -index) : -1;
        assert(plain.path_size(start, goal) == length);
        assert(lazy.path_size(start, goal) == length);
        assert(plain.jump(start, goal, index) == expected);
        assert(lazy.kth_on_path(start, goal, index) == expected);
        if(iteration % 1000 == 0){
            plain.evert(start);
            lazy.evert(start);
        }
    }

    const int middle = n / 2;
    assert(plain.cut(middle - 1, middle));
    assert(lazy.cut(middle - 1, middle));
    assert(plain.path_size(0, n - 1) == -1);
    assert(lazy.path_size(0, n - 1) == -1);
    assert(plain.jump(0, n - 1, 0) == -1);
    assert(lazy.jump(0, n - 1, 0) == -1);
    assert(plain.link(middle - 1, middle));
    assert(lazy.link(middle - 1, middle));
    assert(plain.kth_on_path(n - 1, 0, n - 1) == 0);
    assert(lazy.kth_on_path(n - 1, 0, n - 1) == 0);

    check_invalid_vertices(plain);
    check_invalid_vertices(lazy);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, edge_count, query_count;
    if(!(std::cin >> n >> edge_count >> query_count)){
        test_random_forest();
        test_large_chain();
        return 0;
    }
    std::vector<long long> initial(static_cast<std::size_t>(n), 0);
    LinkCutTree<sum_monoid, 20000> plain(initial);
    LazyLinkCutTree<range_add_sum, 20000> lazy(initial);
    for(int index = 0; index < edge_count; ++index){
        int left, right;
        std::cin >> left >> right;
        assert(plain.link(left, right));
        assert(lazy.link(left, right));
    }
    while(query_count-- > 0){
        std::string operation;
        std::cin >> operation;
        if(operation == "LINK" || operation == "CUT"){
            int left, right;
            std::cin >> left >> right;
            const bool plain_result = operation == "LINK" ?
                plain.link(left, right) : plain.cut(left, right);
            const bool lazy_result = operation == "LINK" ?
                lazy.link(left, right) : lazy.cut(left, right);
            assert(plain_result == lazy_result);
            std::cout << plain_result << '\n';
        }else if(operation == "EVERT"){
            int vertex;
            std::cin >> vertex;
            plain.evert(vertex);
            lazy.evert(vertex);
        }else if(operation == "ROOT"){
            int vertex;
            std::cin >> vertex;
            const int plain_result = plain.root(vertex);
            const int lazy_result = lazy.root(vertex);
            assert(plain_result == lazy_result);
            std::cout << plain_result << '\n';
        }else if(operation == "CONNECTED"){
            int left, right;
            std::cin >> left >> right;
            const bool plain_result = plain.connected(left, right);
            const bool lazy_result = lazy.connected(left, right);
            assert(plain_result == lazy_result);
            std::cout << plain_result << '\n';
        }else{
            int start, goal;
            std::cin >> start >> goal;
            int plain_result;
            int lazy_result;
            if(operation == "PATHSIZE"){
                plain_result = plain.path_size(start, goal);
                lazy_result = lazy.path_size(start, goal);
            }else{
                int index;
                std::cin >> index;
                if(operation == "KTH"){
                    plain_result = plain.kth_on_path(start, goal, index);
                    lazy_result = lazy.kth_on_path(start, goal, index);
                }else{
                    plain_result = plain.jump(start, goal, index);
                    lazy_result = lazy.jump(start, goal, index);
                }
            }
            assert(plain_result == lazy_result);
            std::cout << plain_result << '\n';
        }
    }
}
