// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/furthest_pair.hpp"
#include "../../src/algorithm/geometry/furthest_pair.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    assert(!furthest_pair(std::vector<std::pair<long long, long long>>{}).exists());
    assert(!furthest_pair(
        std::vector<std::pair<long long, long long>>{{3, 4}}
    ).exists());

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        std::size_t size;
        std::cin >> size;
        std::vector<std::pair<long long, long long>> points(size);
        for(auto& [x, y]: points) std::cin >> x >> y;
        const FurthestPairResult result = furthest_pair(points);
        if(result.exists()){
            std::cout << result.first << ' ' << result.second << '\n';
        }else{
            std::cout << "-1 -1\n";
        }
    }
}
