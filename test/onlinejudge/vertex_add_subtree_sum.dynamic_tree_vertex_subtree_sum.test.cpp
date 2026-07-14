// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/vertex_add_subtree_sum

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/structure/tree/dynamic_tree_vertex_subtree_sum.hpp"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    std::cin >> n >> q;
    std::vector<long long> value(n);
    for (long long& x : value) std::cin >> x;

    std::vector<std::pair<int, int>> edges;
    edges.reserve(static_cast<std::size_t>(n > 0 ? n - 1 : 0));
    for (int vertex = 1; vertex < n; ++vertex) {
        int parent;
        std::cin >> parent;
        edges.push_back({parent, vertex});
    }

    DynamicTreeVertexSubtreeSum<long long> tree(value, edges);
    while (q--) {
        int type, vertex;
        std::cin >> type >> vertex;
        if (type == 0) {
            long long delta;
            std::cin >> delta;
            tree.add(vertex, delta);
        } else {
            std::cout << tree.subtree_sum(vertex) << '\n';
        }
    }
}
