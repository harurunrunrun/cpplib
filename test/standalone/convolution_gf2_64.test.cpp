// competitive-verifier: STANDALONE

#include <cstdint>
#include <iostream>
#include <vector>

#include "../../src/algorithm/fft/transform/convolution_gf2_64.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    for(const std::uint64_t value: {
        std::uint64_t{1},
        std::uint64_t{2},
        std::uint64_t{0xFEDCBA9876543210ULL},
        ~std::uint64_t{0},
    }){
        const Gf2_64 element{value};
        if((element * element.inverse()).value() != 1) return 3;
        if(!(element.pow(0) == Gf2_64{1})) return 3;
        if(!(element.pow(3) == element * element * element)) return 3;
    }
    try{
        static_cast<void>(Gf2_64{}.inverse());
        return 3;
    }catch(const std::domain_error&){
    }

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
        const auto answer = gf2_64_convolution(first, second);
        for(std::size_t index = 0; index < answer.size(); ++index){
            if(index) std::cout << ' ';
            std::cout << answer[index];
        }
        std::cout << '\n';
    }
}
