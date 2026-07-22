// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <utility>
#include <vector>

#include "../../src/algorithm/math/polynomial/multipoint_evaluation.hpp"

namespace{

constexpr int MOD = 998244353;
using mint = Modint<MOD>;
using polynomial = math::Polynomial<MOD>;

mint naive_evaluate(const polynomial& coefficients, mint point){
    mint result = 0;
    for(std::size_t index = coefficients.size(); index > 0; --index){
        result = result * point + coefficients[index - 1];
    }
    return result;
}

std::vector<mint> naive_multipoint(
    const polynomial& coefficients,
    const std::vector<mint>& points
){
    std::vector<mint> result;
    result.reserve(points.size());
    for(const mint point: points){
        result.push_back(naive_evaluate(coefficients, point));
    }
    return result;
}

void check_case(
    const polynomial& coefficients,
    const std::vector<mint>& points
){
    assert(
        math::multipoint_evaluation<MOD>(coefficients, points) ==
        naive_multipoint(coefficients, points)
    );
}

void self_test(){
    check_case({}, {});
    check_case({}, {mint(0), mint(1)});
    check_case({mint(2), mint(3), mint(4)}, {mint(0), mint(1), mint(5), mint(5)});

    std::mt19937_64 engine(0x082efa98ec4e6c89ULL);
    for(int iteration = 0; iteration < 450; ++iteration){
        polynomial coefficients(engine() % 65);
        std::vector<mint> points(engine() % 65);
        for(mint& value: coefficients) value = mint(static_cast<long long>(engine() % MOD));
        for(mint& value: points) value = mint(static_cast<long long>(engine() % MOD));
        check_case(coefficients, points);
    }

    polynomial large_coefficients(257);
    std::vector<mint> large_points(256);
    for(mint& value: large_coefficients) value = mint(static_cast<long long>(engine() % MOD));
    for(mint& value: large_points) value = mint(static_cast<long long>(engine() % MOD));
    check_case(large_coefficients, large_points);
}

void print_values(const std::vector<mint>& values){
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
        std::size_t coefficient_count, point_count;
        std::cin >> coefficient_count >> point_count;
        polynomial coefficients(coefficient_count);
        std::vector<mint> points(point_count);
        for(mint& value: coefficients) std::cin >> value;
        for(mint& value: points) std::cin >> value;
        print_values(math::multipoint_evaluation<MOD>(
            std::move(coefficients), points
        ));
    }
}
