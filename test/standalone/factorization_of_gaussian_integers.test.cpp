// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/math/gaussian_integer_factorization.hpp"

namespace{

using math::GaussianInteger;
using math::GaussianIntegerFactorization;
using math::GaussianPrimePower;
using i128 = __int128_t;

long long small_norm(GaussianInteger value){
    return value.real * value.real + value.imag * value.imag;
}

bool rational_prime(long long value){
    if(value < 2) return false;
    for(long long divisor = 2; divisor * divisor <= value; ++divisor){
        if(value % divisor == 0) return false;
    }
    return true;
}

bool gaussian_prime(GaussianInteger value){
    if(value.real == 0 || value.imag == 0){
        const long long component = value.real == 0 ? value.imag : value.real;
        return rational_prime(component) && component % 4 == 3;
    }
    return rational_prime(small_norm(value));
}

bool divide_exact(GaussianInteger& value, GaussianInteger divisor){
    const long long denominator = small_norm(divisor);
    const long long real_numerator = value.real * divisor.real + value.imag * divisor.imag;
    const long long imag_numerator = value.imag * divisor.real - value.real * divisor.imag;
    if(real_numerator % denominator != 0 || imag_numerator % denominator != 0){
        return false;
    }
    value = {real_numerator / denominator, imag_numerator / denominator};
    return true;
}

GaussianIntegerFactorization brute_factorization(GaussianInteger value){
    const long long norm = small_norm(value);
    std::vector<GaussianInteger> primes;
    for(long long real = 1; real * real <= norm; ++real){
        for(long long imag = 0; real * real + imag * imag <= norm; ++imag){
            const GaussianInteger candidate{real, imag};
            if(gaussian_prime(candidate)) primes.push_back(candidate);
        }
    }
    std::sort(primes.begin(), primes.end(), [](GaussianInteger left, GaussianInteger right){
        return std::pair{left.real, left.imag} < std::pair{right.real, right.imag};
    });

    GaussianIntegerFactorization result;
    for(const GaussianInteger prime: primes){
        int exponent = 0;
        while(divide_exact(value, prime)) ++exponent;
        if(exponent != 0) result.factors.push_back({prime, exponent});
    }
    result.unit = value;
    return result;
}

GaussianInteger multiply(GaussianInteger left, GaussianInteger right){
    const i128 real = static_cast<i128>(left.real) * right.real
        - static_cast<i128>(left.imag) * right.imag;
    const i128 imag = static_cast<i128>(left.real) * right.imag
        + static_cast<i128>(left.imag) * right.real;
    return {static_cast<long long>(real), static_cast<long long>(imag)};
}

GaussianInteger reconstruct(const GaussianIntegerFactorization& factorization){
    GaussianInteger result = factorization.unit;
    for(const GaussianPrimePower& factor: factorization.factors){
        for(int exponent = 0; exponent < factor.exponent; ++exponent){
            result = multiply(result, factor.prime);
        }
    }
    return result;
}

void validate(GaussianInteger value, const GaussianIntegerFactorization& factorization){
    assert(small_norm(factorization.unit) == 1);
    assert(reconstruct(factorization) == value);
    for(std::size_t index = 0; index < factorization.factors.size(); ++index){
        const auto& factor = factorization.factors[index];
        assert(factor.exponent > 0);
        assert(factor.prime.real > 0 && factor.prime.imag >= 0);
        assert(gaussian_prime(factor.prime));
        if(index != 0){
            const auto& previous = factorization.factors[index - 1].prime;
            assert((std::pair{previous.real, previous.imag}
                    < std::pair{factor.prime.real, factor.prime.imag}));
        }
    }
}

void self_test(){
    for(const GaussianInteger unit: std::vector<GaussianInteger>{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}){
        const auto factorization = math::factorize_gaussian_integer(unit);
        assert(factorization.unit == unit);
        assert(factorization.factors.empty());
    }

    for(const GaussianInteger value: std::vector<GaussianInteger>{
            {2, 0}, {3, 0}, {5, 0}, {4, 2}, {-18, 24}, {81, -153}}){
        const auto factorization = math::factorize_gaussian_integer(value);
        validate(value, factorization);
        assert(factorization == brute_factorization(value));
    }

    std::mt19937 random(895);
    std::uniform_int_distribution<int> component(-35, 35);
    for(int iteration = 0; iteration < 1200; ++iteration){
        GaussianInteger value{component(random), component(random)};
        if(value == GaussianInteger{}) value = {1, 0};
        const auto factorization = math::factorize_gaussian_integer(value);
        validate(value, factorization);
        assert(factorization == brute_factorization(value));
    }

    const GaussianInteger large_split{4'294'967'295LL, 65'536};
    const auto split_factorization = math::factorize_gaussian_integer(large_split);
    assert((split_factorization == GaussianIntegerFactorization{
        {1, 0}, {{{4'294'967'295LL, 65'536}, 1}}
    }));

    const GaussianInteger large_inert{4'294'967'291LL, 0};
    const auto inert_factorization = math::factorize_gaussian_integer(large_inert);
    assert((inert_factorization == GaussianIntegerFactorization{
        {1, 0}, {{{4'294'967'291LL, 0}, 1}}
    }));

    bool threw = false;
    try{
        (void)math::factorize_gaussian_integer({0, 0});
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);

    threw = false;
    try{
        (void)math::factorize_gaussian_integer({4'294'967'296LL, 0});
    }catch(const std::overflow_error&){
        threw = true;
    }
    assert(threw);

    threw = false;
    try{
        (void)math::factorize_gaussian_integer({
            std::numeric_limits<long long>::min(),
            std::numeric_limits<long long>::min()
        });
    }catch(const std::overflow_error&){
        threw = true;
    }
    assert(threw);
}

void print(const GaussianIntegerFactorization& factorization){
    std::cout << factorization.unit.real << ' ' << factorization.unit.imag << ' '
              << factorization.factors.size();
    for(const GaussianPrimePower& factor: factorization.factors){
        std::cout << ' ' << factor.prime.real << ' ' << factor.prime.imag
                  << ' ' << factor.exponent;
    }
    std::cout << '\n';
}

} // namespace

int main(){
    self_test();
    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- != 0){
        GaussianInteger value;
        std::cin >> value.real >> value.imag;
        print(math::factorize_gaussian_integer(value));
    }
}
