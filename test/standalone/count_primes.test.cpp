// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/count_primes.hpp"

int main(){
    const std::vector<long long> self_values{-5, 0, 1, 2, 3, 4, 97};
    assert(math::count_primes(self_values) == 3);
    assert(math::count_primes_with_sieve(self_values, 97) == 3);
    assert(math::count_primes_with_sieve(
        std::vector<bool>{false, true, true}, 1
    ) == 0);

    bool bound_threw = false;
    try{
        (void)math::count_primes_with_sieve(
            std::vector<unsigned>{2, 101}, 100
        );
    }catch(const std::invalid_argument&){
        bound_threw = true;
    }
    assert(bound_threw);

    bool length_threw = false;
    try{
        (void)math::count_primes_with_sieve(
            std::vector<unsigned>{},
            std::numeric_limits<std::size_t>::max()
        );
    }catch(const std::length_error&){
        length_threw = true;
    }
    assert(length_threw);

    int case_count;
    std::cin >> case_count;
    while(case_count--){
        char method;
        int size;
        std::cin >> method >> size;
        std::size_t maximum_value = 0;
        if(method == 'S') std::cin >> maximum_value;
        std::vector<long long> values(static_cast<std::size_t>(size));
        for(auto& value: values) std::cin >> value;
        if(method == 'M'){
            std::cout << math::count_primes(values) << '\n';
        }else if(method == 'S'){
            std::cout << math::count_primes_with_sieve(
                values, maximum_value
            ) << '\n';
        }else{
            throw std::runtime_error("unknown prime counting method");
        }
    }
}
