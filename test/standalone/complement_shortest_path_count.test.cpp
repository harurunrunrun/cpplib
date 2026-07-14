// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/complement_shortest_path_count.hpp"

void self_test(){
    constexpr long long MOD = 998244353;
    assert(complement_shortest_path_count(2, {}, 0, 1, MOD) == 1);
    assert(complement_shortest_path_count(2, {{0, 1}}, 0, 1, MOD) == -1);
    assert(complement_shortest_path_count(1, {}, 0, 0, MOD) == 1);
    assert(complement_shortest_path_count(
        3, {{0, 2}, {0, 2}, {1, 1}}, 0, 2, MOD
    ) == 1);
}

int main(){
    self_test();

    constexpr long long MOD = 998244353;
    int n, m;
    if(!(std::cin >> n >> m)) return 0;
    std::vector<std::pair<int, int>> excluded_edges;
    excluded_edges.reserve(static_cast<std::size_t>(m));
    for(int edge = 0; edge < m; ++edge){
        int first, second;
        std::cin >> first >> second;
        excluded_edges.emplace_back(first - 1, second - 1);
    }
    std::cout << complement_shortest_path_count(
        n, excluded_edges, 0, n - 1, MOD
    ) << '\n';
}
