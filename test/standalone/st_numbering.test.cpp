// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/st_numbering.hpp"

bool valid_numbering(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    int source,
    int sink,
    const STNumberingResult& result
){
    if(result.number.size() != static_cast<std::size_t>(n) ||
       result.order.size() != static_cast<std::size_t>(n)) return false;
    std::vector<int> seen(static_cast<std::size_t>(n));
    for(int vertex = 0; vertex < n; ++vertex){
        const int value = result.number[static_cast<std::size_t>(vertex)];
        if(value < 0 || n <= value || seen[static_cast<std::size_t>(value)]){
            return false;
        }
        seen[static_cast<std::size_t>(value)] = 1;
        if(result.order[static_cast<std::size_t>(value)] != vertex) return false;
    }
    if(result.number[static_cast<std::size_t>(source)] != 0 ||
       result.number[static_cast<std::size_t>(sink)] != n - 1) return false;
    std::vector<unsigned char> lower(static_cast<std::size_t>(n));
    std::vector<unsigned char> higher(static_cast<std::size_t>(n));
    for(const auto& [left, right]: edges){
        if(result.number[static_cast<std::size_t>(left)] <
           result.number[static_cast<std::size_t>(right)]){
            higher[static_cast<std::size_t>(left)] = 1;
            lower[static_cast<std::size_t>(right)] = 1;
        }else{
            higher[static_cast<std::size_t>(right)] = 1;
            lower[static_cast<std::size_t>(left)] = 1;
        }
    }
    for(int vertex = 0; vertex < n; ++vertex){
        if(vertex != source && !lower[static_cast<std::size_t>(vertex)]){
            return false;
        }
        if(vertex != sink && !higher[static_cast<std::size_t>(vertex)]){
            return false;
        }
    }
    return true;
}

bool brute_exists(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    int source,
    int sink
){
    std::vector<int> middle;
    for(int vertex = 0; vertex < n; ++vertex){
        if(vertex != source && vertex != sink) middle.push_back(vertex);
    }
    std::sort(middle.begin(), middle.end());
    do{
        STNumberingResult candidate;
        candidate.order.push_back(source);
        candidate.order.insert(
            candidate.order.end(), middle.begin(), middle.end()
        );
        if(sink != source) candidate.order.push_back(sink);
        candidate.number.assign(static_cast<std::size_t>(n), -1);
        for(int index = 0; index < n; ++index){
            candidate.number[static_cast<std::size_t>(
                candidate.order[static_cast<std::size_t>(index)]
            )] = index;
        }
        if(valid_numbering(n, edges, source, sink, candidate)) return true;
    }while(std::next_permutation(middle.begin(), middle.end()));
    return false;
}

void check_case(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    int source,
    int sink,
    bool compare_brute
){
    const auto result = st_numbering(n, edges, source, sink);
    if(result) assert(valid_numbering(n, edges, source, sink, *result));
    if(compare_brute){
        assert(result.has_value() == brute_exists(n, edges, source, sink));
    }
}

void test_random(){
    std::mt19937 rng(2026071601);
    for(int iteration = 0; iteration < 700; ++iteration){
        const int n = 2 + static_cast<int>(rng() % 7U);
        int source = static_cast<int>(rng() % static_cast<unsigned int>(n));
        int sink = static_cast<int>(rng() % static_cast<unsigned int>(n - 1));
        if(sink >= source) ++sink;
        std::vector<std::pair<int, int>> edges;
        for(int left = 0; left < n; ++left){
            for(int right = left + 1; right < n; ++right){
                const int copies = static_cast<int>(rng() % 3U);
                for(int copy = 0; copy < copies; ++copy){
                    edges.push_back({left, right});
                }
            }
        }
        check_case(n, edges, source, sink, true);
    }
}

void test_boundaries(){
    check_case(1, {}, 0, 0, true);
    check_case(4, {{0, 1}, {1, 2}, {2, 3}}, 0, 3, true);
    check_case(5, {{0, 2}, {1, 2}, {2, 3}, {2, 4}}, 0, 1, true);
    check_case(4, {{0, 1}, {2, 3}}, 0, 1, true);

    constexpr int n = 200000;
    std::vector<std::pair<int, int>> path;
    path.reserve(static_cast<std::size_t>(n - 1));
    for(int vertex = 0; vertex + 1 < n; ++vertex){
        path.push_back({vertex, vertex + 1});
    }
    check_case(n, path, 0, n - 1, false);

    bool thrown = false;
    try{ (void)st_numbering(3, {{0, 3}}, 0, 2); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)st_numbering(3, {{1, 1}}, 0, 2); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)){
        test_random();
        test_boundaries();
        return 0;
    }
    while(test_count--){
        int n, m, source, sink;
        std::cin >> n >> m >> source >> sink;
        std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
        for(auto& [left, right]: edges) std::cin >> left >> right;
        const auto result = st_numbering(n, edges, source, sink);
        if(result && !valid_numbering(n, edges, source, sink, *result)){
            throw std::runtime_error("invalid st-numbering certificate");
        }
        std::cout << (result ? "YES\n" : "NO\n");
    }
}
