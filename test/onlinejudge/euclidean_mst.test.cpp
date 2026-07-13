// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/euclidean_mst

#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/euclidean_mst.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t size;
    std::cin >> size;
    std::vector<std::pair<long long, long long>> points(size);
    for(auto& [x, y]: points) std::cin >> x >> y;
    for(const EuclideanMstEdge& edge: euclidean_mst(points).edges){
        std::cout << edge.from << ' ' << edge.to << '\n';
    }
}
