// competitive-verifier: STANDALONE

#include <cstdint>
#include <iostream>
#include <vector>

#include "../../src/algorithm/fft/transform/convolution_mod_2_64.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 2;
    while(case_count--){
        int first_size;
        int second_size;
        std::cin >> first_size >> second_size;
        std::vector<std::uint64_t> first(
            static_cast<std::size_t>(first_size)
        );
        std::vector<std::uint64_t> second(
            static_cast<std::size_t>(second_size)
        );
        for(std::uint64_t& value: first) std::cin >> value;
        for(std::uint64_t& value: second) std::cin >> value;
        const auto answer = convolution_mod_2_64(first, second);
        for(std::size_t index = 0; index < answer.size(); ++index){
            if(index) std::cout << ' ';
            std::cout << answer[index];
        }
        std::cout << '\n';
    }
}
