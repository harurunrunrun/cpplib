// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/fft/min_plus_convolution_concave.hpp"

namespace{

bool api_contract_is_satisfied(){
    if(!is_discrete_concave(std::vector<long long>{3, 4, 4, 1})) return false;
    if(is_discrete_concave(std::vector<long long>{0, 0, 1})) return false;
    if(!min_plus_convolution_concave_arbitrary(
           std::vector<long long>{}, std::vector<long long>{1}).empty()) return false;
    if(min_plus_convolution_arbitrary_concave(
           std::vector<long long>{4, -3, 8, 1},
           std::vector<long long>{0, 3, 5, 6, 6}) !=
       min_plus_convolution_concave_arbitrary(
           std::vector<long long>{0, 3, 5, 6, 6},
           std::vector<long long>{4, -3, 8, 1})) return false;

    try{
        (void)min_plus_convolution_concave_arbitrary(
            std::vector<long long>{0, 0, 1}, std::vector<long long>{0}
        );
        return false;
    }catch(const std::invalid_argument&){
    }
    try{
        (void)min_plus_convolution_concave_arbitrary(
            std::vector<long long>{std::numeric_limits<long long>::max()},
            std::vector<long long>{1}
        );
        return false;
    }catch(const std::overflow_error&){
    }
    return true;
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    if(!api_contract_is_satisfied()) return 1;

    int query_count;
    if(!(std::cin >> query_count)) return 1;
    while(query_count-- > 0){
        int n, m;
        std::cin >> n >> m;
        std::vector<long long> concave(static_cast<std::size_t>(n));
        std::vector<long long> arbitrary(static_cast<std::size_t>(m));
        for(long long& value: concave) std::cin >> value;
        for(long long& value: arbitrary) std::cin >> value;
        const auto answer =
            min_plus_convolution_concave_arbitrary(concave, arbitrary);
        for(std::size_t i = 0; i < answer.size(); ++i){
            if(i != 0) std::cout << ' ';
            std::cout << answer[i];
        }
        std::cout << '\n';
    }
}
