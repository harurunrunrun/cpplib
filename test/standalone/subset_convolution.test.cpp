// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/polynomial/subset_convolution.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

int main(){
    try{
        (void)subset_convolution(std::vector<int>{1, 2}, std::vector<int>{1});
        return 1;
    }catch(const std::invalid_argument&){
    }
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int query_count;
    if(!(std::cin >> query_count)) return 1;
    while(query_count-- > 0){
        int bit_count;
        std::cin >> bit_count;
        const std::size_t size = std::size_t{1} << bit_count;
        std::vector<FastModint998244353> left(size), right(size);
        for(auto& value: left) std::cin >> value;
        for(auto& value: right) std::cin >> value;
        const auto answer = subset_convolution(left, right);
        for(std::size_t index = 0; index < answer.size(); ++index){
            if(index != 0) std::cout << ' ';
            std::cout << answer[index];
        }
        std::cout << '\n';
    }
}
