// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/convex_layers

#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/2d/point_collection/convex_layers.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t size;
    std::cin >> size;
    std::vector<std::pair<long long, long long>> points(size);
    for(auto& [x, y]: points) std::cin >> x >> y;
    for(int layer: convex_layers(points)) std::cout << layer << '\n';
}
