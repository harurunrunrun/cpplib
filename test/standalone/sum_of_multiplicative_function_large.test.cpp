// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/sum_of_multiplicative_function_large.hpp"

namespace{

constexpr int MOD = 469762049;
using mint = Modint<MOD>;

mint naive_official(std::uint64_t n, mint a, mint b){
    mint answer = 1;
    for(std::uint64_t value = 2; value <= n; ++value){
        std::uint64_t remaining = value;
        mint function_value = 1;
        for(std::uint64_t prime = 2; prime <= remaining / prime; ++prime){
            if(remaining % prime != 0) continue;
            std::uint32_t exponent = 0;
            while(remaining % prime == 0){
                remaining /= prime;
                ++exponent;
            }
            function_value *=
                a * mint(static_cast<long long>(exponent)) +
                b * mint(static_cast<long long>(prime));
        }
        if(remaining > 1){
            function_value *= a + b * mint(static_cast<long long>(remaining));
        }
        answer += function_value;
    }
    return n == 0 ? mint(0) : answer;
}

void generic_self_test(){
    constexpr std::uint64_t n = 10000;
    const math::LinearPrimePowerMultiplicativeFunction<MOD> linear{
        mint(2), mint(3)
    };
    assert(linear(5, 4) == mint(23));

    math::LargeMultiplicativeFunctionSummatory<MOD> summatory(n);
    assert(summatory.limit() == n);
    const auto values = summatory.quotient_values();
    const auto components = summatory.prime_prefix_components();
    assert(values.size() == summatory.quotient_table_size());
    assert(components.prime_count.size() == values.size());
    assert(components.prime_sum.size() == values.size());
    assert(!values.empty() && values[0] == 0);

    const auto one = [](std::uint64_t, std::uint32_t){
        return mint(1);
    };
    assert(summatory.sum(one, components.prime_count) == mint(n));

    const auto identity = [](std::uint64_t prime, std::uint32_t exponent){
        return mint(static_cast<long long>(prime)).pow(exponent);
    };
    assert(
        summatory.sum(identity, components.prime_sum) ==
        mint(n) * mint(n + 1) / mint(2)
    );

    std::vector<mint> divisor_prime_prefix = components.prime_count;
    for(mint& value: divisor_prime_prefix) value *= mint(2);
    const auto divisor_count = [](std::uint64_t, std::uint32_t exponent){
        return mint(static_cast<long long>(exponent + 1));
    };
    mint divisor_sum = 0;
    for(std::uint64_t value = 1; value <= n; ++value){
        divisor_sum += mint(static_cast<long long>(n / value));
    }
    assert(
        summatory.sum(divisor_count, divisor_prime_prefix) == divisor_sum
    );

    std::vector<mint> prime_square_prefix(n + 1);
    std::vector<bool> is_prime(n + 1, true);
    is_prime[0] = is_prime[1] = false;
    for(std::uint64_t prime = 2; prime <= n; ++prime){
        if(is_prime[prime] && prime <= n / prime){
            for(std::uint64_t multiple = prime * prime;
                multiple <= n;
                multiple += prime){
                is_prime[multiple] = false;
            }
        }
        prime_square_prefix[prime] = prime_square_prefix[prime - 1];
        if(is_prime[prime]){
            prime_square_prefix[prime] +=
                mint(static_cast<long long>(prime)).pow(2);
        }
    }
    std::vector<mint> square_table(values.size());
    for(std::size_t index = 1; index < values.size(); ++index){
        square_table[index] =
            prime_square_prefix[static_cast<std::size_t>(values[index])];
    }
    const auto square = [](std::uint64_t prime, std::uint32_t exponent){
        return mint(static_cast<long long>(prime)).pow(2LL * exponent);
    };
    assert(
        summatory.sum(square, square_table) ==
        mint(n) * mint(n + 1) * mint(2 * n + 1) / mint(6)
    );

    bool thrown = false;
    try{
        (void)summatory.sum(one, std::vector<mint>(1));
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}

void self_test(){
    assert(
        math::sum_of_multiplicative_function_large(
            0, mint(123), mint(456)
        ) == mint(0)
    );
    generic_self_test();

    std::mt19937_64 engine(0xba7c9045f12c7f99ULL);
    for(int iteration = 0; iteration < 320; ++iteration){
        const std::uint64_t n = 1 + engine() % 500;
        const mint a(static_cast<long long>(engine() % MOD));
        const mint b(static_cast<long long>(engine() % MOD));
        assert(
            math::sum_of_multiplicative_function_large(n, a, b) ==
            naive_official(n, a, b)
        );
    }

    assert(
        math::sum_of_multiplicative_function_large(
            1'000'000,
            mint(MOD - 1),
            mint(MOD - 1)
        ) == mint(371700649)
    );

    bool thrown = false;
    try{
        math::LargeMultiplicativeFunctionSummatory<MOD> invalid(
            math::sum_multiplicative_function_large_max_n + 1
        );
        (void)invalid;
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    math::LargeMultiplicativeFunctionSummatory<MOD> empty(0);
    assert(empty.limit() == 0);
    assert(empty.quotient_values().empty());
    assert(empty.prime_prefix_components().prime_count.empty());
    assert(empty.sum(
        [](std::uint64_t, std::uint32_t){ return mint(1); },
        {}
    ) == mint(0));
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    self_test();
    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- != 0){
        std::uint64_t n;
        mint a, b;
        std::cin >> n >> a >> b;
        std::cout << math::sum_of_multiplicative_function_large(n, a, b)
                  << '\n';
    }
}
