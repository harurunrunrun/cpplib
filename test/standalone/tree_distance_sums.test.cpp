// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/tree/tree_distance_sums.hpp"

void self_test(){
    assert(tree_distance_sums(0, {}).empty());
    assert(tree_distance_sums(1, {}) == std::vector<long long>({0}));
    assert(tree_distance_sums(3, {{0, 1}, {1, 2}}) ==
        std::vector<long long>({3, 2, 3}));
    assert(tree_distance_sums(4, {{0, 1}, {0, 2}, {0, 3}}) ==
        std::vector<long long>({3, 5, 5, 5}));

    bool thrown = false;
    try{
        static_cast<void>(tree_distance_sums(-1, {}));
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        static_cast<void>(tree_distance_sums(3, {{0, 1}}));
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        static_cast<void>(tree_distance_sums(2, {{0, 2}}));
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        static_cast<void>(tree_distance_sums(3, {{0, 1}, {0, 1}}));
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    self_test();

    int n, m;
    if(!(std::cin >> n >> m)) return 0;
    std::vector<std::pair<int, int>> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int index = 0; index < m; index++){
        int left, right;
        std::cin >> left >> right;
        edges.emplace_back(left, right);
    }
    for(long long value: tree_distance_sums(n, edges)){
        std::cout << value << '\n';
    }
}
