// competitive-verifier: STANDALONE

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/combinatorics/binomial_mod_composite.hpp"
#include "../../src/algorithm/math/combinatorics/catalan_numbers.hpp"
#include "../../src/algorithm/math/combinatorics/eulerian_numbers.hpp"
#include "../../src/algorithm/math/combinatorics/generalized_lucas_theorem.hpp"
#include "../../src/algorithm/math/combinatorics/lucas_theorem.hpp"

namespace{

unsigned long long pascal_lucas(
    unsigned long long n,
    unsigned long long k,
    unsigned long long prime
){
    if(k > n) return 0;
    unsigned long long answer = 1;
    while(n != 0 || k != 0){
        const auto n_digit = n % prime;
        const auto k_digit = k % prime;
        if(k_digit > n_digit) return 0;
        std::vector<unsigned long long> row(
            static_cast<std::size_t>(k_digit + 1), 0
        );
        row[0] = 1;
        for(unsigned long long size = 1; size <= n_digit; ++size){
            const auto upper = std::min(size, k_digit);
            for(unsigned long long index = upper; index != 0; --index){
                row[static_cast<std::size_t>(index)] =
                    (row[static_cast<std::size_t>(index)]
                        + row[static_cast<std::size_t>(index - 1)]) % prime;
            }
        }
        answer = static_cast<unsigned long long>(
            static_cast<__uint128_t>(answer)
            * row[static_cast<std::size_t>(k_digit)] % prime
        );
        n /= prime;
        k /= prime;
    }
    return answer;
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    try{
        (void)math::lucas_binomial_mod_prime(5, 2, 4);
        return 6;
    }catch(const std::runtime_error&){
    }
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
            const auto lucas =
                math::lucas_binomial_mod_prime(n, k, prime);
            if(lucas != pascal_lucas(n, k, prime)
                || (exponent == 1 && value != lucas)){
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
