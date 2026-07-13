// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/discrete_logarithm_fixed_mod.hpp"

namespace{

std::uint32_t multiply_mod(
    std::uint32_t left,
    std::uint32_t right,
    std::uint32_t modulus
){
    return static_cast<std::uint32_t>(
        static_cast<std::uint64_t>(left) * right % modulus
    );
}

std::uint32_t power_mod(
    std::uint32_t base,
    std::uint32_t exponent,
    std::uint32_t modulus
){
    std::uint32_t result = 1 % modulus;
    while(exponent != 0){
        if((exponent & 1) != 0){
            result = multiply_mod(result, base, modulus);
        }
        base = multiply_mod(base, base, modulus);
        exponent >>= 1;
    }
    return result;
}

std::vector<std::uint32_t> prime_factors(std::uint32_t value){
    std::vector<std::uint32_t> result;
    for(std::uint32_t factor = 2;
        static_cast<std::uint64_t>(factor) * factor <= value;
        ++factor){
        if(value % factor != 0) continue;
        result.push_back(factor);
        while(value % factor == 0) value /= factor;
    }
    if(value > 1) result.push_back(value);
    return result;
}

std::uint32_t primitive_root(std::uint32_t prime){
    const auto factors = prime_factors(prime - 1);
    for(std::uint32_t candidate = 1; candidate < prime; ++candidate){
        bool valid = true;
        for(std::uint32_t factor: factors){
            if(power_mod(candidate, (prime - 1) / factor, prime) == 1){
                valid = false;
                break;
            }
        }
        if(valid) return candidate;
    }
    throw std::runtime_error("primitive root not found");
}

void test_complete_small_tables(){
    for(std::uint32_t prime: {2U, 3U, 5U, 7U, 17U, 101U, 9973U}){
        const std::uint32_t root = primitive_root(prime);
        math::FixedModDiscreteLogarithm logarithm(prime, root);
        assert(logarithm.modulus() == prime);
        assert(logarithm.primitive_root() == root);

        std::vector<std::uint32_t> expected(
            prime,
            std::numeric_limits<std::uint32_t>::max()
        );
        std::uint32_t current = 1;
        for(std::uint32_t exponent = 0; exponent < prime - 1; ++exponent){
            expected[current] = exponent;
            current = multiply_mod(current, root, prime);
        }

        std::vector<std::uint32_t> values;
        for(std::uint32_t value = 1; value < prime; ++value){
            values.push_back(value);
            assert(logarithm.log(value) == expected[value]);
            assert(logarithm(value) == expected[value]);
        }
        const auto answers = logarithm.logs(values);
        assert(answers.size() == prime - 1);
        for(std::size_t index = 0; index < values.size(); ++index){
            assert(answers[index] == expected[values[index]]);
        }
    }
}

void test_farey_path(){
    constexpr std::uint32_t prime = 1'000'003;
    const std::uint32_t root = primitive_root(prime);
    math::FixedModDiscreteLogarithm logarithm(prime, root);

    std::mt19937 engine(0x9d0bc1a7U);
    for(int iteration = 0; iteration < 20'000; ++iteration){
        const std::uint32_t exponent = static_cast<std::uint32_t>(
            engine() % (prime - 1)
        );
        const std::uint32_t value = power_mod(root, exponent, prime);
        assert(logarithm.log(value) == exponent);
    }
}

void test_invalid_arguments(){
    bool thrown = false;
    try{
        math::FixedModDiscreteLogarithm logarithm(15, 2);
        (void)logarithm;
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        math::FixedModDiscreteLogarithm logarithm(7, 2);
        (void)logarithm;
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        math::FixedModDiscreteLogarithm logarithm(1'000'000'007, 5);
        (void)logarithm;
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    math::FixedModDiscreteLogarithm logarithm(101, 2);
    for(std::uint32_t invalid: {0U, 101U}){
        thrown = false;
        try{
            (void)logarithm.log(invalid);
        }catch(const std::invalid_argument&){
            thrown = true;
        }
        assert(thrown);
    }
}

void self_test(){
    test_complete_small_tables();
    test_farey_path();
    test_invalid_arguments();
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::uint32_t prime, root, query_count;
    if(!(std::cin >> prime >> root >> query_count)){
        self_test();
        return 0;
    }

    math::FixedModDiscreteLogarithm logarithm(prime, root);
    while(query_count-- != 0){
        std::uint32_t value;
        std::cin >> value;
        std::cout << logarithm.log(value) << '\n';
    }
}
