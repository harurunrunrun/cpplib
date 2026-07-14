// competitive-verifier: STANDALONE

#include <iostream>
#include <vector>

#include "../../src/algorithm/math/range_lcm.hpp"

int main(){
    int case_count;
    std::cin >> case_count;
    while(case_count--){
        int size;
        std::cin >> size;
        std::vector<long long> values(static_cast<std::size_t>(size));
        for(auto& value: values) std::cin >> value;
        std::cout << math::range_lcm(values) << '\n';
    }
}
