// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/cartesian_tree

#include <iostream>
#include <vector>

#include "../../src/algorithm/tree/cartesian_tree_parent_array.hpp"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<int> values(n);
    for (int& value : values) std::cin >> value;

    const auto parent = cartesian_tree_parent_array(values);
    for (int vertex = 0; vertex < n; ++vertex) {
        if (vertex) std::cout << ' ';
        std::cout << parent[static_cast<std::size_t>(vertex)];
    }
    std::cout << '\n';
}
