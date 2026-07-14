// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/math/polynomial_interpolation_fast.hpp"

namespace{

constexpr int MOD = 998244353;
using mint = Modint<MOD>;
using polynomial = math::Polynomial<MOD>;

mint evaluate(const polynomial& coefficients, mint point){
    mint result = 0;
    for(std::size_t index = coefficients.size(); index > 0; --index){
        result = result * point + coefficients[index - 1];
    }
    return result;
}

void check_case(
    const polynomial& expected_coefficients,
    const std::vector<mint>& points
){
    std::vector<mint> values;
    values.reserve(points.size());
    for(const mint point: points){
        values.push_back(evaluate(expected_coefficients, point));
    }
    polynomial actual = math::polynomial_interpolation<MOD>(points, values);
    actual.resize(points.size());
    assert(actual == expected_coefficients);
}

void self_test(){
    check_case({}, {});
    check_case({mint(7)}, {mint(123)});
    check_case({mint(2), mint(3), mint(4)}, {mint(0), mint(1), mint(5)});

    std::mt19937_64 engine(0xbe5466cf34e90c6cULL);
    for(int iteration = 0; iteration < 400; ++iteration){
        const std::size_t size = 1 + engine() % 55;
        polynomial coefficients(size);
        std::vector<mint> points(size);
        for(mint& value: coefficients) value = mint(static_cast<long long>(engine() % MOD));
        const mint offset(static_cast<long long>(engine() % MOD));
        const mint step(1 + static_cast<long long>(engine() % (MOD - 1)));
        for(std::size_t index = 0; index < size; ++index){
            points[index] = offset + step * mint(static_cast<long long>(index));
        }
        check_case(coefficients, points);
    }

    polynomial large_coefficients(257);
    std::vector<mint> large_points(257);
    for(mint& value: large_coefficients) value = mint(static_cast<long long>(engine() % MOD));
    for(std::size_t index = 0; index < large_points.size(); ++index){
        large_points[index] = mint(static_cast<long long>(index * 17 + 5));
    }
    check_case(large_coefficients, large_points);

    bool thrown = false;
    try{
        (void)math::polynomial_interpolation<MOD>(
            {mint(1)}, {mint(2), mint(3)}
        );
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)math::polynomial_interpolation<MOD>(
            {mint(4), mint(4)}, {mint(2), mint(3)}
        );
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}

void print_coefficients(const polynomial& coefficients){
    for(std::size_t index = 0; index < coefficients.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << coefficients[index];
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
        std::size_t size;
        std::cin >> size;
        std::vector<mint> points(size), values(size);
        for(mint& point: points) std::cin >> point;
        for(mint& value: values) std::cin >> value;
        polynomial result = math::polynomial_interpolation<MOD>(points, values);
        result.resize(size);
        print_coefficients(result);
    }
}
