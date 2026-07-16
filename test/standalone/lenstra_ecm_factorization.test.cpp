// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/math/lenstra_ecm_factorization.hpp"

namespace{

void verify_factorization(
    const BigInteger& value,
    const std::vector<std::pair<BigInteger, int>>& factors
){
    BigInteger reconstructed = 1;
    BigInteger previous = 0;
    for(const auto& [prime, exponent]: factors){
        assert(prime > previous);
        assert(exponent > 0);
        LenstraEcmParameters primality_parameters;
        primality_parameters.curves = 0;
        assert(!lenstra_ecm_find_factor(prime, primality_parameters).has_value());
        for(int count = 0; count < exponent; ++count) reconstructed *= prime;
        previous = prime;
    }
    assert(reconstructed == abs(value));
}

void test_montgomery_arithmetic(){
    using lenstra_ecm_detail::Montgomery128;
    using lenstra_ecm_detail::u128;
    std::mt19937_64 random(2026071621ULL);
    for(int modulus_case = 0; modulus_case < 200; ++modulus_case){
        u128 modulus = (static_cast<u128>(random() & 0x3fffffffffffffffULL) << 64)
            | random();
        modulus |= 1;
        if(modulus < 3) modulus = 3;
        Montgomery128 arithmetic(modulus);
        for(int iteration = 0; iteration < 200; ++iteration){
            const u128 first =
                ((static_cast<u128>(random()) << 64) | random()) % modulus;
            const u128 second =
                ((static_cast<u128>(random()) << 64) | random()) % modulus;
            const u128 encoded_first = arithmetic.encode(first);
            const u128 encoded_second = arithmetic.encode(second);
            assert(arithmetic.decode(encoded_first) == first);
            assert(arithmetic.decode(arithmetic.add(encoded_first, encoded_second))
                == (first + second) % modulus);
            const u128 expected_product = [&]{
                u128 result = 0;
                u128 addend = first;
                u128 multiplier = second;
                while(multiplier != 0){
                    if((multiplier & 1) != 0){
                        result = result >= modulus - addend
                            ? result - (modulus - addend)
                            : result + addend;
                    }
                    addend = addend >= modulus - addend
                        ? addend - (modulus - addend)
                        : addend + addend;
                    multiplier >>= 1;
                }
                return result;
            }();
            assert(arithmetic.decode(
                arithmetic.multiply(encoded_first, encoded_second)
            ) == expected_product);
        }
    }
}

void test_edges_and_factorizations(){
    LenstraEcmParameters parameters;
    parameters.stage1_bound = 2'000;
    parameters.stage2_bound = 30'000;
    parameters.curves = 32;
    parameters.factorization_retries = 4;
    parameters.seed = 0x123456789abcdef0ULL;

    assert(!lenstra_ecm_find_factor(0, parameters).has_value());
    assert(!lenstra_ecm_find_factor(1, parameters).has_value());
    assert(!lenstra_ecm_find_factor(2, parameters).has_value());
    assert(!lenstra_ecm_find_factor(BigInteger("2305843009213693951"), parameters)
        .has_value());

    for(const std::string text: {
        "-10403",
        "1000036000099",
        "1000000016000000063",
        "999949000866995087",
        "1152921504606846976"
    }){
        const BigInteger value(text);
        const auto factors = factorize_lenstra_ecm(value, parameters);
        verify_factorization(value, factors);
    }
    LenstraEcmParameters large_parameters = parameters;
    large_parameters.stage1_bound = 20'000;
    large_parameters.stage2_bound = 400'000;
    large_parameters.curves = 48;
    large_parameters.seed = 0x6a09e667f3bcc909ULL;
    const BigInteger large_semiprime(
        "170141183460469230726339751698713544131"
    );
    verify_factorization(
        large_semiprime,
        factorize_lenstra_ecm(large_semiprime, large_parameters)
    );

    bool stage2_witnessed = false;
    for(std::uint64_t seed = 1; seed <= 256 && !stage2_witnessed; ++seed){
        LenstraEcmParameters first_stage;
        first_stage.stage1_bound = 20;
        first_stage.stage2_bound = 20;
        first_stage.curves = 1;
        first_stage.seed = seed;
        LenstraEcmParameters second_stage = first_stage;
        second_stage.stage2_bound = 500;
        const BigInteger stage2_semiprime("1022117");  // 1009 * 1013
        const auto without_stage2 =
            lenstra_ecm_find_factor(stage2_semiprime, first_stage);
        const auto with_stage2 =
            lenstra_ecm_find_factor(stage2_semiprime, second_stage);
        stage2_witnessed = !without_stage2.has_value()
            && with_stage2.has_value()
            && stage2_semiprime % *with_stage2 == 0;
    }
    assert(stage2_witnessed);

    bool invalid_thrown = false;
    try{
        LenstraEcmParameters invalid = parameters;
        invalid.gcd_batch_size = 0;
        (void)lenstra_ecm_find_factor(91, invalid);
    }catch(const std::invalid_argument&){
        invalid_thrown = true;
    }
    assert(invalid_thrown);

    bool zero_thrown = false;
    try{
        (void)factorize_lenstra_ecm(0, parameters);
    }catch(const std::invalid_argument&){
        zero_thrown = true;
    }
    assert(zero_thrown);

    bool overflow_thrown = false;
    try{
        (void)lenstra_ecm_find_factor(BigInteger("170141183460469231731687303715884105728"));
    }catch(const std::overflow_error&){
        overflow_thrown = true;
    }
    assert(overflow_thrown);
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t query_count;
    if(!(std::cin >> query_count)){
        test_montgomery_arithmetic();
        test_edges_and_factorizations();
        return 0;
    }
    while(query_count-- > 0){
        std::string text;
        LenstraEcmParameters parameters;
        std::cin >> text
            >> parameters.stage1_bound
            >> parameters.stage2_bound
            >> parameters.curves
            >> parameters.seed;
        parameters.factorization_retries = 6;
        const auto factors = factorize_lenstra_ecm(BigInteger(text), parameters);
        verify_factorization(BigInteger(text), factors);
        std::cout << factors.size();
        for(const auto& [prime, exponent]: factors){
            std::cout << ' ' << prime << ' ' << exponent;
        }
        std::cout << '\n';
    }
}
