// competitive-verifier: PROBLEM https://www.spoj.com/problems/DISQUERY/

#include <algorithm>
#include <iostream>
#include <limits>
#include <utility>

#include "../../src/algorithm/tree/static_tree_path_fold.hpp"
#include "../../src/structure/types/monoid.hpp"

using Range = std::pair<int, int>;

constexpr Range combine_range(Range left, Range right){
    return {
        std::min(left.first, right.first),
        std::max(left.second, right.second)
    };
}

constexpr Range empty_range(){
    return {
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::lowest()
    };
}

constexpr Monoid<combine_range, empty_range> range_monoid{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count;
    std::cin >> vertex_count;
    StaticTreePathFold<range_monoid, 100000> tree(vertex_count);
    for(int edge = 1; edge < vertex_count; ++edge){
        int left, right, weight;
        std::cin >> left >> right >> weight;
        tree.add_edge(left - 1, right - 1, {weight, weight});
    }
    tree.build();

    int query_count;
    std::cin >> query_count;
    while(query_count-- != 0){
        int left, right;
        std::cin >> left >> right;
        const auto [minimum, maximum] = tree.prod(left - 1, right - 1);
        std::cout << minimum << ' ' << maximum << '\n';
    }
}
