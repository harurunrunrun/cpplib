// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/cartesian_tree

#include <iostream>
#include <vector>

#include "../../src/algorithm/tree/cartesian_tree.hpp"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<int> values(n);
    for (int& value : values) std::cin >> value;

    auto tree = cartesian_tree(values);
    tree.parent[tree.root] = tree.root;
    for (int vertex = 0; vertex < n; ++vertex) {
        if (vertex) std::cout << ' ';
        std::cout << tree.parent[vertex];
    }
    std::cout << '\n';
}
