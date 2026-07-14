// competitive-verifier: STANDALONE

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/math/q_binomial_prime_mod.hpp"
#include "../../src/algorithm/math/q_binomial_prime_mod.hpp"

int main(){
    try{
        (void)QBinomialPrimeMod(4, 1, 3);
        return 1;
    }catch(const std::invalid_argument&){
    }
    try{
        (void)QBinomialPrimeMod(7, 7, 6);
        return 1;
    }catch(const std::invalid_argument&){
    }
    try{
        (void)QBinomialPrimeMod(7, 1, 7);
        return 1;
    }catch(const std::invalid_argument&){
    }

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t query_count;
    std::uint32_t modulus, q;
    if(!(std::cin >> query_count >> modulus >> q)) return 1;
    std::vector<std::pair<std::size_t, std::size_t>> queries(query_count);
    std::size_t maximum_n = 0;
    for(auto& [n, k] : queries){
        std::cin >> n >> k;
        if(n > maximum_n) maximum_n = n;
    }

    const QBinomialPrimeMod solver(modulus, q, maximum_n);
    if(solver.modulus() != modulus || solver.base() != q ||
       solver.maximum_n() != maximum_n) return 1;
    std::size_t expected_period = 0;
    std::uint64_t q_integer = 1 % modulus;
    for(std::size_t index = 1; index <= maximum_n; ++index){
        if(q_integer == 0){
            expected_period = index;
            break;
        }
        q_integer = (
            static_cast<std::uint64_t>(q) * q_integer + 1
        ) % modulus;
    }
    if(solver.period() != expected_period) return 1;
    try{
        (void)solver.binomial(maximum_n + 1, 0);
        return 1;
    }catch(const std::out_of_range&){
    }
    for(const auto& [n, k] : queries){
        std::cout << solver.binomial(n, k) << '\n';
    }
}
