// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <stdexcept>

#include "../../src/algorithm/math/sqrt_mod.hpp"

namespace{

bool is_prime(std::uint64_t value){
    if(value < 2) return false;
    for(std::uint64_t divisor = 2; divisor * divisor <= value; ++divisor){
        if(value % divisor == 0) return false;
    }
    return true;
}

std::optional<std::uint64_t> brute(std::uint64_t value, std::uint64_t prime){
    for(std::uint64_t root = 0; root < prime; ++root){
        if(root * root % prime == value) return root;
    }
    return std::nullopt;
}

void self_test(){
    for(std::uint64_t prime = 2; prime <= 500; ++prime){
        if(!is_prime(prime)) continue;
        for(std::uint64_t value = 0; value < prime; ++value){
            assert(math::sqrt_mod(value, prime) == brute(value, prime));
        }
    }
    const auto large = math::sqrt_mod(4, 998244353);
    assert(large && *large == 2);

    bool thrown = false;
    try{
        (void)math::sqrt_mod(7, 7);
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}

} // namespace

int main(){
    self_test();
    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- != 0){
        std::uint64_t value, prime;
        std::cin >> value >> prime;
        const auto root = math::sqrt_mod(value, prime);
        if(root) std::cout << *root << '\n';
        else std::cout << -1 << '\n';
    }
}
