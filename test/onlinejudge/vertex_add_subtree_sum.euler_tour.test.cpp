// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/vertex_add_subtree_sum

#include <iostream>
#include <vector>

#include "../../src/algorithm/tree/euler_tour.hpp"

class FenwickTree {
    std::vector<long long> data;

public:
    explicit FenwickTree(int n) : data(n + 1, 0) {}

    void add(int index, long long value) {
        for (++index; index < static_cast<int>(data.size()); index += index & -index) {
            data[index] += value;
        }
    }

    long long prefix_sum(int right) const {
        long long result = 0;
        for (; right > 0; right -= right & -right) result += data[right];
        return result;
    }

    long long range_sum(int left, int right) const {
        return prefix_sum(right) - prefix_sum(left);
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    std::cin >> n >> q;
    std::vector<long long> value(n);
    for (long long& x : value) std::cin >> x;

    static TreeEulerTour<500000> tour(n);
    for (int vertex = 1; vertex < n; ++vertex) {
        int parent;
        std::cin >> parent;
        tour.add_edge(parent, vertex);
    }
    tour.build(0);

    FenwickTree fenwick(n);
    for (int vertex = 0; vertex < n; ++vertex) {
        fenwick.add(tour.tin(vertex), value[vertex]);
    }

    while (q--) {
        int type, vertex;
        std::cin >> type >> vertex;
        if (type == 0) {
            long long delta;
            std::cin >> delta;
            fenwick.add(tour.tin(vertex), delta);
        } else {
            const auto [left, right] = tour.subtree_range(vertex);
            std::cout << fenwick.range_sum(left, right) << '\n';
        }
    }
}
