// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/math/polynomial/polynomial_division.hpp"

namespace{

constexpr int MOD = 998244353;
using mint = Modint<MOD>;
using polynomial = math::Polynomial<MOD>;

void trim(polynomial& values){
    while(!values.empty() && values.back() == mint(0)) values.pop_back();
}

std::pair<polynomial, polynomial> naive_divmod(
    polynomial dividend,
    polynomial divisor
){
    trim(dividend);
    trim(divisor);
    if(divisor.empty()) throw std::invalid_argument("zero divisor");
    if(dividend.size() < divisor.size()) return {{}, std::move(dividend)};

    polynomial quotient(dividend.size() - divisor.size() + 1);
    const mint inverse_leading = divisor.back().inv();
    for(std::size_t remaining = quotient.size(); remaining > 0; --remaining){
        const std::size_t shift = remaining - 1;
        const mint factor = dividend[shift + divisor.size() - 1] * inverse_leading;
        quotient[shift] = factor;
        for(std::size_t index = 0; index < divisor.size(); ++index){
            dividend[shift + index] -= factor * divisor[index];
        }
    }
    dividend.resize(divisor.size() - 1);
    trim(quotient);
    trim(dividend);
    return {std::move(quotient), std::move(dividend)};
}

void check_case(const polynomial& dividend, const polynomial& divisor){
    const auto expected = naive_divmod(dividend, divisor);
    const auto actual = math::polynomial_divmod<MOD>(dividend, divisor);
    assert(actual.quotient == expected.first);
    assert(actual.remainder == expected.second);
    assert(math::polynomial_remainder<MOD>(dividend, divisor) == expected.second);
}

void self_test(){
    check_case({}, {mint(1)});
    check_case({mint(3), mint(4)}, {mint(7)});
    check_case({mint(1), mint(2)}, {mint(1), mint(0), mint(1)});

    std::mt19937_64 engine(0x9e3779b97f4a7c15ULL);
    for(int iteration = 0; iteration < 500; ++iteration){
        const std::size_t dividend_size = engine() % 81;
        const std::size_t divisor_size = 1 + engine() % 36;
        polynomial dividend(dividend_size);
        polynomial divisor(divisor_size);
        for(mint& value: dividend) value = mint(static_cast<long long>(engine() % MOD));
        for(mint& value: divisor) value = mint(static_cast<long long>(engine() % MOD));
        divisor.back() = mint(1 + static_cast<long long>(engine() % (MOD - 1)));
        check_case(dividend, divisor);
    }

    polynomial large_dividend(320);
    polynomial large_divisor(129);
    for(mint& value: large_dividend) value = mint(static_cast<long long>(engine() % MOD));
    for(mint& value: large_divisor) value = mint(static_cast<long long>(engine() % MOD));
    large_divisor.back() = mint(17);
    check_case(large_dividend, large_divisor);

    bool thrown = false;
    try{
        (void)math::polynomial_divmod<MOD>({mint(1)}, {});
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}

void print_coefficients(const polynomial& values){
    for(std::size_t index = 0; index < values.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << values[index];
    }
    std::cout << '\n';
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    self_test();
    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- != 0){
        std::size_t dividend_size, divisor_size;
        std::cin >> dividend_size >> divisor_size;
        polynomial dividend(dividend_size), divisor(divisor_size);
        for(mint& value: dividend) std::cin >> value;
        for(mint& value: divisor) std::cin >> value;
        const auto result = math::polynomial_divmod<MOD>(
            std::move(dividend), std::move(divisor)
        );
        std::cout << result.quotient.size() << ' ' << result.remainder.size() << '\n';
        print_coefficients(result.quotient);
        print_coefficients(result.remainder);
    }
}
