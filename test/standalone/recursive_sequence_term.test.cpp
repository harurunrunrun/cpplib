// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/math/recursive_sequence_term.hpp"

namespace{

void self_test(){
    const std::vector<std::uint64_t> initial{0, 1};
    const std::vector<std::uint64_t> coefficient{1, 1};
    assert(math::recursive_sequence_term(initial, coefficient, 11) == 55);
    assert(
        math::recursive_sequence_term_static_modulus<998244353>(
            initial, coefficient, 11
        ) == 55
    );
    assert(math::recursive_sequence_term(
        initial, coefficient, 11, 97
    ) == 55);

    bool rejected = false;
    try{
        static_cast<void>(math::recursive_sequence_term(
            {}, {}, 1
        ));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    assert(rejected);
    rejected = false;
    try{
        static_cast<void>(
            math::recursive_sequence_term_static_modulus<998244353>(
                initial, coefficient, 0
            )
        );
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    assert(rejected);
    rejected = false;
    try{
        static_cast<void>(math::recursive_sequence_term(
            initial, coefficient, 1, 0
        ));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    assert(rejected);
}

}  // namespace

int main(){
    self_test();
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t query_count;
    if(!(std::cin >> query_count)) return 1;
    while(query_count-- > 0){
        std::string mode;
        std::size_t degree;
        std::uint64_t index;
        std::cin >> mode >> degree >> index;
        std::uint64_t modulus = 0;
        if(mode == "RUNTIME") std::cin >> modulus;
        std::vector<std::uint64_t> initial(degree);
        std::vector<std::uint64_t> coefficient(degree);
        for(auto& value: initial) std::cin >> value;
        for(auto& value: coefficient) std::cin >> value;

        if(mode == "DEFAULT"){
            std::cout << math::recursive_sequence_term(
                initial, coefficient, index
            ) << '\n';
        }else if(mode == "STATIC998"){
            std::cout
                << math::recursive_sequence_term_static_modulus<998244353>(
                    initial, coefficient, index
                ) << '\n';
        }else if(mode == "RUNTIME"){
            std::cout << math::recursive_sequence_term(
                initial, coefficient, index, modulus
            ) << '\n';
        }else{
            return 2;
        }
    }
}
