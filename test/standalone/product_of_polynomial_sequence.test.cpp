// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <utility>
#include <vector>

#include "../../src/algorithm/math/polynomial/polynomial_common.hpp"
#include "../../src/algorithm/math/polynomial/polynomial_product.hpp"

namespace{

constexpr int MOD = 998244353;
using mint = Modint<MOD>;
using polynomial = math::Polynomial<MOD>;

void trim(polynomial& values){
    while(!values.empty() && values.back() == mint(0)) values.pop_back();
}

polynomial naive_multiply(const polynomial& left, const polynomial& right){
    if(left.empty() || right.empty()) return {};
    polynomial result(left.size() + right.size() - 1);
    for(std::size_t i = 0; i < left.size(); ++i){
        for(std::size_t j = 0; j < right.size(); ++j){
            result[i + j] += left[i] * right[j];
        }
    }
    trim(result);
    return result;
}

polynomial naive_product(const std::vector<polynomial>& polynomials){
    polynomial result{mint(1)};
    for(polynomial factor: polynomials){
        trim(factor);
        result = naive_multiply(result, factor);
    }
    return result;
}

void check_case(const std::vector<polynomial>& polynomials){
    assert(
        math::product_of_polynomial_sequence<MOD>(polynomials) ==
        naive_product(polynomials)
    );
}

void self_test(){
    {
        polynomial values{mint(1), mint(2), mint(0), mint(0)};
        math::polynomial_internal::trim(values);
        assert((values == polynomial{mint(1), mint(2)}));
        assert((math::polynomial_internal::normalized<MOD>(
            polynomial{mint(3), mint(0)}
        ) == polynomial{mint(3)}));
        assert((math::polynomial_internal::naive_multiply<MOD>(
            polynomial{mint(1), mint(2)},
            polynomial{mint(3), mint(4)}
        ) == polynomial{mint(3), mint(10), mint(8)}));
        assert((math::polynomial_multiply<MOD>(
            polynomial{mint(1), mint(2)},
            polynomial{mint(3), mint(4)}
        ) == polynomial{mint(3), mint(10), mint(8)}));
        assert((math::polynomial_add<MOD>(
            polynomial{mint(1), mint(2)},
            polynomial{mint(3), mint(-2), mint(5)}
        ) == polynomial{mint(4), mint(0), mint(5)}));
        assert((math::polynomial_derivative<MOD>(
            polynomial{mint(7), mint(3), mint(4)}
        ) == polynomial{mint(3), mint(8)}));
    }

    check_case({});
    check_case({{}});
    check_case({{mint(2)}, {mint(1), mint(2)}, {mint(3), mint(2), mint(1)}});

    std::mt19937_64 engine(0xa4093822299f31d0ULL);
    for(int iteration = 0; iteration < 400; ++iteration){
        const std::size_t count = engine() % 18;
        std::vector<polynomial> polynomials(count);
        for(polynomial& factor: polynomials){
            const std::size_t size = engine() % 13;
            factor.resize(size);
            for(mint& value: factor) value = mint(static_cast<long long>(engine() % MOD));
        }
        check_case(polynomials);
    }

    std::vector<polynomial> large(64);
    for(polynomial& factor: large){
        factor.resize(6);
        for(mint& value: factor) value = mint(static_cast<long long>(engine() % MOD));
        factor.back() = mint(1 + static_cast<long long>(engine() % (MOD - 1)));
    }
    check_case(large);
}

void print_polynomial(const polynomial& values){
    std::cout << values.size() << '\n';
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
        std::size_t count;
        std::cin >> count;
        std::vector<polynomial> polynomials(count);
        for(polynomial& factor: polynomials){
            std::size_t size;
            std::cin >> size;
            factor.resize(size);
            for(mint& value: factor) std::cin >> value;
        }
        print_polynomial(
            math::product_of_polynomial_sequence<MOD>(polynomials)
        );
    }
}
