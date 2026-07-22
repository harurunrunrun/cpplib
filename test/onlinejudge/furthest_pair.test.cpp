// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/furthest_pair

#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/2d/query/furthest_pair.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        std::size_t size;
        std::cin >> size;
        std::vector<std::pair<long long, long long>> points(size);
        for(auto& [x, y]: points) std::cin >> x >> y;
        const FurthestPairResult result = furthest_pair(points);
        std::cout << result.first << ' ' << result.second << '\n';
    }
}
