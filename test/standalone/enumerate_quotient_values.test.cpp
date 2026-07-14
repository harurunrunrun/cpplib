// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/enumerate_quotient_values.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    long long number;
    if(!(std::cin >> number)){
        assert(math::enumerate_quotient_values(0).empty());
        assert((math::enumerate_quotient_values(10) ==
            std::vector<long long>{1, 2, 3, 5, 10}));
        return 0;
    }
    const auto answer = math::enumerate_quotient_values(number);
    std::cout << answer.size() << '\n';
    for(std::size_t index = 0; index < answer.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << answer[index];
    }
    std::cout << '\n';
}
