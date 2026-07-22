// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>

#include "../../src/algorithm/math/number_theory/gaussian_integer.hpp"

namespace{

using math::GaussianInteger;

long long norm(GaussianInteger value){
    return value.real * value.real + value.imag * value.imag;
}

bool divides(GaussianInteger divisor, GaussianInteger value){
    if(divisor.real == 0 && divisor.imag == 0){
        return value.real == 0 && value.imag == 0;
    }
    const long long denominator = norm(divisor);
    const long long real_numerator = value.real * divisor.real + value.imag * divisor.imag;
    const long long imag_numerator = value.imag * divisor.real - value.real * divisor.imag;
    return real_numerator % denominator == 0 && imag_numerator % denominator == 0;
}

GaussianInteger canonical(GaussianInteger value){
    const GaussianInteger candidates[] = {
        value,
        {-value.imag, value.real},
        {-value.real, -value.imag},
        {value.imag, -value.real},
    };
    for(GaussianInteger candidate: candidates){
        if(candidate.real > 0 && candidate.imag >= 0) return candidate;
    }
    return {};
}

GaussianInteger brute(GaussianInteger left, GaussianInteger right){
    if(left == GaussianInteger{} && right == GaussianInteger{}) return {};
    long long limit = std::min(
        left == GaussianInteger{} ? norm(right) : norm(left),
        right == GaussianInteger{} ? norm(left) : norm(right)
    );
    long long best_norm = 0;
    GaussianInteger best{};
    for(long long real = -20; real <= 20; ++real){
        for(long long imag = -20; imag <= 20; ++imag){
            GaussianInteger candidate{real, imag};
            const long long candidate_norm = norm(candidate);
            if(candidate_norm == 0 || candidate_norm > limit) continue;
            if(divides(candidate, left) && divides(candidate, right) &&
               candidate_norm > best_norm){
                best_norm = candidate_norm;
                best = canonical(candidate);
            }
        }
    }
    return best;
}

void self_test(){
    assert(math::gaussian_gcd({0, 0}, {0, 0}) == GaussianInteger{});
    assert(math::gaussian_gcd({6, 8}, {4, 2}) == brute({6, 8}, {4, 2}));
    assert(math::gaussian_gcd({-7, 5}, {3, 4}) == brute({-7, 5}, {3, 4}));

    std::mt19937 generator(20260713);
    std::uniform_int_distribution<int> component(-8, 8);
    for(int iteration = 0; iteration < 1000; ++iteration){
        const GaussianInteger left{component(generator), component(generator)};
        const GaussianInteger right{component(generator), component(generator)};
        assert(math::gaussian_gcd(left, right) == brute(left, right));
    }

    const GaussianInteger large = math::gaussian_gcd(
        {1'000'000'000, 1'000'000'000},
        {1'000'000'000, -1'000'000'000}
    );
    assert(large == (GaussianInteger{1'000'000'000, 1'000'000'000}));
}

} // namespace

int main(){
    self_test();
    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- != 0){
        math::GaussianInteger left, right;
        std::cin >> left.real >> left.imag >> right.real >> right.imag;
        const auto result = math::gaussian_gcd(left, right);
        std::cout << result.real << ' ' << result.imag << '\n';
    }
}
