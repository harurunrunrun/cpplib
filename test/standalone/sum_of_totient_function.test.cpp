// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/number_theory/sum_of_totient_function.hpp"

namespace{

template<int MOD>
std::vector<int> naive_prefix(std::size_t limit){
    std::vector<std::uint64_t> phi(limit + 1);
    for(std::size_t value = 0; value <= limit; ++value) phi[value] = value;
    for(std::size_t prime = 2; prime <= limit; ++prime){
        if(phi[prime] != prime) continue;
        for(std::size_t multiple = prime;
            multiple <= limit;
            multiple += prime){
            phi[multiple] -= phi[multiple] / prime;
        }
    }
    std::vector<int> result(limit + 1);
    std::uint64_t sum = 0;
    for(std::size_t value = 1; value <= limit; ++value){
        sum = (sum + phi[value]) % MOD;
        result[value] = static_cast<int>(sum);
    }
    return result;
}

void self_test(){
    constexpr std::size_t limit = 200000;
    const auto expected = naive_prefix<998244353>(limit);
    for(std::size_t value = 0; value <= 200; ++value){
        assert(math::sum_of_totient_function(value).val() == expected[value]);
        assert(math::sum_totient(value).val() == expected[value]);
    }

    std::mt19937_64 engine(0x2ffd72dbd01adfb7ULL);
    for(int iteration = 0; iteration < 350; ++iteration){
        const std::size_t value = engine() % (limit + 1);
        assert(math::sum_totient(value).val() == expected[value]);
    }

    const auto other_mod = naive_prefix<1'000'000'007>(5000);
    for(std::size_t value: {1U, 2U, 10U, 999U, 5000U}){
        assert(
            math::sum_totient<1'000'000'007>(value).val() ==
            other_mod[value]
        );
    }

    assert(math::sum_totient(1'000'000'000ULL).val() == 422516288);
    assert(math::sum_totient(10'000'000'000ULL).val() == 866849765);

    bool thrown = false;
    try{
        (void)math::sum_totient(math::sum_totient_function_max_n + 1);
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    self_test();
    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- != 0){
        std::uint64_t n;
        std::cin >> n;
        std::cout << math::sum_of_totient_function(n) << '\n';
    }
}
