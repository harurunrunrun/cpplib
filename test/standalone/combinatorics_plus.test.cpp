// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>

#include "../../src/algorithm/math/combinatorics/binomial_mod_composite.hpp"
#include "../../src/algorithm/math/combinatorics/catalan_numbers.hpp"
#include "../../src/algorithm/math/combinatorics/eulerian_numbers.hpp"
#include "../../src/algorithm/math/combinatorics/generalized_lucas_theorem.hpp"
#include "../../src/algorithm/math/combinatorics/lucas_theorem.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int kind;
        std::cin >> kind;
        if(kind == 0){
            unsigned long long n;
            unsigned long long k;
            unsigned long long prime;
            int exponent;
            unsigned long long modulus;
            std::cin >> n >> k >> prime >> exponent >> modulus;
            auto value = math::generalized_lucas_binomial_mod_prime_power(
                n, k, prime, exponent
            );
            if(exponent == 1
                && value != math::lucas_binomial_mod_prime(n, k, prime)){
                return 2;
            }
            std::cout << value << ' '
                      << math::binomial_mod_composite(n, k, modulus)
                      << '\n';
        }else if(kind == 1){
            std::size_t maximum_index;
            std::cin >> maximum_index;
            auto values = math::catalan_numbers<998244353>(maximum_index);
            for(std::size_t index = 0; index < values.size(); ++index){
                if(index != 0) std::cout << ' ';
                std::cout << values[index].val();
            }
            std::cout << '\n';
        }else{
            std::size_t order;
            std::cin >> order;
            auto values = math::eulerian_numbers<998244353>(order);
            for(std::size_t index = 0; index < values.size(); ++index){
                if(index != 0) std::cout << ' ';
                std::cout << values[index].val();
            }
            std::cout << '\n';
        }
    }
}
