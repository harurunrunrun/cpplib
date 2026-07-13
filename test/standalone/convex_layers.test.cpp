// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/convex_layers.hpp"
#include "../../src/algorithm/geometry/convex_layers.hpp"

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
        const std::vector<int> layers = convex_layers(points);
        for(std::size_t index = 0; index < layers.size(); ++index){
            if(index != 0) std::cout << ' ';
            std::cout << layers[index];
        }
        std::cout << '\n';
    }
}
