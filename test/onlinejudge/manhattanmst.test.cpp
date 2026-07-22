// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/manhattanmst

#include <iostream>
#include <vector>
#include "../../src/algorithm/graph/spanning_tree/manhattan_mst.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;
    std::vector<ManhattanMstPoint<long long>> points(static_cast<std::size_t>(n));
    for(auto& point: points) std::cin >> point.x >> point.y;
    const auto result = manhattan_mst(points);
    std::cout << result.cost << '\n';
    for(const auto& edge: result.edges) std::cout << edge.from << ' ' << edge.to << '\n';
}
