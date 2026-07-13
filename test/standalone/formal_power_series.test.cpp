// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/fft/formal_power_series.hpp"

using mint = Modint998244353;

std::vector<mint> naive_multiply(const std::vector<mint>& left,
                                 const std::vector<mint>& right,
                                 std::size_t size){
    std::vector<mint> result(size);
    for(std::size_t i = 0; i < left.size(); ++i){
        for(std::size_t j = 0; j < right.size() && i + j < size; ++j){
            result[i + j] += left[i] * right[j];
        }
    }
    return result;
}

std::vector<mint> naive_power(std::vector<mint> values, int exponent, std::size_t size){
    values.resize(size);
    std::vector<mint> result(size);
    if(size != 0) result[0] = 1;
    while(exponent-- > 0) result = naive_multiply(result, values, size);
    return result;
}

std::vector<mint> naive_exponential(const std::vector<mint>& values, std::size_t size){
    std::vector<mint> result(size);
    if(size == 0) return result;
    result[0] = 1;
    for(std::size_t degree = 1; degree < size; ++degree){
        for(std::size_t index = 1; index <= degree && index < values.size(); ++index){
            result[degree] += mint(static_cast<long long>(index)) * values[index] * result[degree - index];
        }
        result[degree] /= mint(static_cast<long long>(degree));
    }
    return result;
}

void test_random(){
    std::mt19937_64 rng(2026071305);
    for(int iteration = 0; iteration < 350; ++iteration){
        const std::size_t size = static_cast<std::size_t>(rng() % 28 + 1);
        std::vector<mint> values(size);
        for(mint& value: values) value = mint(static_cast<long long>(rng() % 1000));
        values[0] = mint(static_cast<long long>(rng() % 998244352 + 1));

        const auto inverse = fps_inv(values, size);
        const auto identity = naive_multiply(values, inverse, size);
        assert(identity[0] == mint(1));
        for(std::size_t i = 1; i < size; ++i) assert(identity[i] == mint(0));

        std::vector<mint> unit = values;
        unit[0] = 1;
        const auto logarithm = fps_log(unit, size);
        assert(fps_exp(logarithm, size) == unit);

        std::vector<mint> exponent_series = values;
        exponent_series[0] = 0;
        assert(fps_exp(exponent_series, size) == naive_exponential(exponent_series, size));

        const int exponent = static_cast<int>(rng() % 6);
        assert(fps_pow(values, exponent, size) == naive_power(values, exponent, size));

        std::vector<mint> root(size);
        for(mint& value: root) value = mint(static_cast<long long>(rng() % 1000));
        if(iteration % 4 == 0 && size > 2){ root[0] = 0; root[1] = 0; }
        const auto square = naive_multiply(root, root, size);
        const auto recovered = fps_sqrt(square, size);
        assert(recovered.has_value());
        assert(naive_multiply(*recovered, *recovered, size) == square);

        assert(fps_derivative(fps_integral(values)) == values);
    }
}

void test_boundaries_and_errors(){
    assert(fps_derivative(std::vector<mint>{}).empty());
    assert(fps_integral(std::vector<mint>{}) == std::vector<mint>{mint(0)});
    assert(fps_inv(std::vector<mint>{}, 0).empty());
    assert(fps_log(std::vector<mint>{}, 0).empty());
    assert(fps_exp(std::vector<mint>{mint(9)}, 0).empty());
    assert(fps_pow(std::vector<mint>{}, 0, 1) == std::vector<mint>{mint(1)});
    assert(fps_pow(std::vector<mint>{0, 0, 3}, 4, 7) == std::vector<mint>(7));
    assert(fps_sqrt(std::vector<mint>{}, 1) == std::optional(std::vector<mint>{mint(0)}));
    assert(!fps_sqrt(std::vector<mint>{0, 1}, 2));
    assert(!fps_sqrt(std::vector<mint>{mint(3)}, 1));

    bool thrown = false;
    try{ (void)fps_inv(std::vector<mint>{0, 1}, 2); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)fps_log(std::vector<mint>{2, 1}, 2); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)fps_exp(std::vector<mint>{1, 1}, 2); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)fps_pow(std::vector<mint>{1}, -1, 1); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    using small_mint = Modint<17>;
    thrown = false;
    try{ (void)fps_inv(std::vector<small_mint>{1, 1}, 10); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    if(sizeof(std::size_t) > sizeof(int)){
        thrown = false;
        try{
            (void)fps_inv(std::vector<mint>{1},
                static_cast<std::size_t>(std::numeric_limits<int>::max()) + 1);
        }catch(const std::length_error&){ thrown = true; }
        assert(thrown);
    }
}

void test_large(){
    constexpr std::size_t size = 1U << 14;
    std::vector<mint> geometric{1, -1};
    const auto inverse = fps_inv(geometric, size);
    assert(inverse.size() == size);
    for(const mint value: inverse) assert(value == mint(1));

    std::vector<mint> exponent_series(1U << 11);
    exponent_series[1] = 1;
    const auto exponential = fps_exp(exponent_series, exponent_series.size());
    mint factorial = 1;
    for(std::size_t i = 0; i < 100; ++i){
        if(i != 0) factorial *= mint(static_cast<long long>(i));
        assert(exponential[i] * factorial == mint(1));
    }
}

void print(const std::vector<mint>& values){
    for(std::size_t i = 0; i < values.size(); ++i){
        if(i != 0) std::cout << ' ';
        std::cout << values[i];
    }
    std::cout << '\n';
}

int main(){
    std::string operation;
    int n;
    if(std::cin >> operation >> n){
        long long exponent = 0;
        if(operation == "POW") std::cin >> exponent;
        std::vector<mint> values(static_cast<std::size_t>(n));
        for(mint& value: values) std::cin >> value;
        if(operation == "DERIVATIVE") print(fps_derivative(values));
        else if(operation == "INTEGRAL") print(fps_integral(values));
        else if(operation == "INV") print(fps_inv(values, values.size()));
        else if(operation == "LOG") print(fps_log(values, values.size()));
        else if(operation == "EXP") print(fps_exp(values, values.size()));
        else if(operation == "POW") print(fps_pow(values, exponent, values.size()));
        else{
            const auto root = fps_sqrt(values, values.size());
            if(root) print(*root);
            else std::cout << "-1\n";
        }
        return 0;
    }
    test_random();
    test_boundaries_and_errors();
    test_large();
}
