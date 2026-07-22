// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/math/polynomial/polynomial_discrete_sum.hpp"
#include "../../src/structure/modint/fast_modint.hpp"
#include "../../src/structure/modint/modint.hpp"

namespace{

using mint = Modint998244353;

constexpr std::size_t MAX_POWER = 200000;
constexpr std::size_t MAX_POLYNOMIAL_DEGREE = 100000;

math::PolynomialDiscreteSum<mint, MAX_POWER> power_summation;
math::PolynomialDiscreteSum<mint, MAX_POLYNOMIAL_DEGREE> polynomial_summation;
math::PolynomialDiscreteSum<mint, MAX_POLYNOMIAL_DEGREE>::CoefficientArray coefficients{};

struct WrappedMint{
    mint value{};

    WrappedMint() = default;
    WrappedMint(long long source) : value(source){}
    WrappedMint(mint source) : value(source){}

    WrappedMint pow(long long exponent) const{
        return WrappedMint(value.pow(exponent));
    }

    WrappedMint inv() const{
        return WrappedMint(value.inv());
    }

    WrappedMint& operator+=(const WrappedMint& other){
        value += other.value;
        return *this;
    }

    WrappedMint& operator-=(const WrappedMint& other){
        value -= other.value;
        return *this;
    }

    WrappedMint& operator*=(const WrappedMint& other){
        value *= other.value;
        return *this;
    }

    WrappedMint& operator/=(const WrappedMint& other){
        value /= other.value;
        return *this;
    }

    friend WrappedMint operator+(WrappedMint left, const WrappedMint& right){ return left += right; }
    friend WrappedMint operator-(WrappedMint left, const WrappedMint& right){ return left -= right; }
    friend WrappedMint operator*(WrappedMint left, const WrappedMint& right){ return left *= right; }
    friend WrappedMint operator/(WrappedMint left, const WrappedMint& right){ return left /= right; }
    friend bool operator==(const WrappedMint&, const WrappedMint&) = default;
};

void self_check(){
    assert(power_summation.power_sum(10, 0) == mint(10));
    assert(power_summation.power_sum(10, 1) == mint(45));
    assert(power_summation.power_sum(10, 2) == mint(285));

    coefficients[0] = mint(3);
    coefficients[1] = mint(2);
    const auto antiderivative = polynomial_summation.discrete_antiderivative(
        coefficients, 2
    );
    assert(antiderivative[0] == mint(0));
    assert(antiderivative[1] == mint(2));
    assert(antiderivative[2] == mint(1));
    assert(polynomial_summation.polynomial_prefix_sum(
        coefficients, 2, 10LL
    ) == mint(120));

    for(std::size_t index = 0; index < 32; ++index){
        coefficients[index] = mint(
            static_cast<long long>(index * index + 7 * index + 11)
        );
    }
    const auto checked_antiderivative =
        polynomial_summation.discrete_antiderivative(coefficients, 32);
    assert(checked_antiderivative[0] == mint(0));
    for(long long x = -8; x <= 40; ++x){
        const mint difference = math::polynomial_evaluate(
            checked_antiderivative, 33, mint(x + 1)
        ) - math::polynomial_evaluate(
            checked_antiderivative, 33, mint(x)
        );
        assert(difference == math::polynomial_evaluate(
            coefficients, 32, mint(x)
        ));
    }

    math::PolynomialDiscreteSum<FastModint998244353, 8> fast_summation;
    assert(fast_summation.power_sum(8, 3) == FastModint998244353(784));
    decltype(fast_summation)::CoefficientArray fast_coefficients{};
    fast_coefficients[0] = FastModint998244353(3);
    fast_coefficients[1] = FastModint998244353(2);
    const auto fast_antiderivative =
        fast_summation.discrete_antiderivative(fast_coefficients, 2);
    assert(fast_antiderivative[1] == FastModint998244353(2));
    assert(fast_antiderivative[2] == FastModint998244353(1));

    math::PolynomialDiscreteSum<Modint<1000000007>, 8>
        arbitrary_mod_summation;
    decltype(arbitrary_mod_summation)::CoefficientArray
        arbitrary_mod_coefficients{};
    arbitrary_mod_coefficients[0] = Modint<1000000007>(3);
    arbitrary_mod_coefficients[1] = Modint<1000000007>(2);
    const auto arbitrary_mod_antiderivative =
        arbitrary_mod_summation.discrete_antiderivative(
            arbitrary_mod_coefficients, 2
        );
    assert(arbitrary_mod_antiderivative[1] == Modint<1000000007>(2));
    assert(arbitrary_mod_antiderivative[2] == Modint<1000000007>(1));

    math::PolynomialDiscreteSum<WrappedMint, 8> generic_summation;
    decltype(generic_summation)::CoefficientArray generic_coefficients{};
    generic_coefficients[0] = WrappedMint(3);
    generic_coefficients[1] = WrappedMint(2);
    const auto generic_antiderivative =
        generic_summation.discrete_antiderivative(generic_coefficients, 2);
    assert(generic_antiderivative[1] == WrappedMint(2));
    assert(generic_antiderivative[2] == WrappedMint(1));

    bool thrown = false;
    try{
        (void)power_summation.power_sum(-1, 2);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)fast_summation.power_sum(1, 9);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)fast_summation.discrete_antiderivative(fast_coefficients, 10);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)fast_summation.polynomial_prefix_sum(
            fast_coefficients, 2, -1LL
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    self_check();
    int query_count;
    if(!(std::cin >> query_count)) return 0;
    for(int query = 0; query < query_count; ++query){
        char operation;
        std::cin >> operation;
        if(operation == 'P'){
            long long n;
            std::size_t exponent;
            std::cin >> n >> exponent;
            std::cout << power_summation.power_sum(n, exponent) << '\n';
        }else if(operation == 'F'){
            long long n;
            std::size_t exponent;
            std::cin >> n >> exponent;
            std::cout << power_summation.faulhaber(mint(n), exponent) << '\n';
        }else if(operation == 'D'){
            std::size_t coefficient_count;
            std::cin >> coefficient_count;
            for(std::size_t index = 0; index < coefficient_count; ++index){
                std::cin >> coefficients[index];
            }
            const auto result = polynomial_summation.discrete_antiderivative(
                coefficients, coefficient_count
            );
            for(std::size_t index = 0; index <= coefficient_count; ++index){
                if(index != 0) std::cout << ' ';
                std::cout << result[index];
            }
            std::cout << '\n';
        }else if(operation == 'S' || operation == 'M'){
            std::size_t coefficient_count;
            long long n;
            std::cin >> coefficient_count >> n;
            for(std::size_t index = 0; index < coefficient_count; ++index){
                std::cin >> coefficients[index];
            }
            if(operation == 'S'){
                std::cout << polynomial_summation.polynomial_prefix_sum(
                    coefficients, coefficient_count, n
                ) << '\n';
            }else{
                std::cout << polynomial_summation.polynomial_prefix_sum(
                    coefficients, coefficient_count, mint(n)
                ) << '\n';
            }
        }else{
            throw std::runtime_error("unknown polynomial discrete sum operation");
        }
    }
}
