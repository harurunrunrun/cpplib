// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/fft/sparse_formal_power_series.hpp"

using mint = Modint998244353;
using term = SparseFormalPowerSeriesTerm<998244353>;

std::vector<mint> naive_multiply(const std::vector<mint>& left,
                                 const std::vector<mint>& right,
                                 std::size_t size){
    std::vector<mint> result(size);
    for(std::size_t i = 0; i < left.size() && i < size; ++i){
        for(std::size_t j = 0; j < right.size() && i + j < size; ++j){
            result[i + j] += left[i] * right[j];
        }
    }
    return result;
}

std::vector<mint> naive_inverse(const std::vector<mint>& values, std::size_t size){
    std::vector<mint> result(size);
    if(size == 0) return result;
    result[0] = values[0].inv();
    for(std::size_t degree = 1; degree < size; ++degree){
        mint sum = 0;
        for(std::size_t index = 1; index <= degree && index < values.size(); ++index){
            sum += values[index] * result[degree - index];
        }
        result[degree] = -sum * result[0];
    }
    return result;
}

std::vector<mint> naive_logarithm(const std::vector<mint>& values, std::size_t size){
    std::vector<mint> derivative(size == 0 ? 0 : size - 1);
    for(std::size_t degree = 1; degree < values.size() && degree < size; ++degree){
        derivative[degree - 1] = mint(static_cast<long long>(degree)) * values[degree];
    }
    const auto inverse = naive_inverse(values, size);
    const auto quotient = naive_multiply(derivative, inverse, derivative.size());
    std::vector<mint> result(size);
    for(std::size_t degree = 1; degree < size; ++degree){
        result[degree] = quotient[degree - 1] / mint(static_cast<long long>(degree));
    }
    return result;
}

std::vector<mint> naive_exponential(const std::vector<mint>& values, std::size_t size){
    std::vector<mint> result(size);
    if(size == 0) return result;
    result[0] = 1;
    for(std::size_t degree = 1; degree < size; ++degree){
        for(std::size_t index = 1; index <= degree && index < values.size(); ++index){
            result[degree] += mint(static_cast<long long>(index)) * values[index] *
                              result[degree - index];
        }
        result[degree] /= mint(static_cast<long long>(degree));
    }
    return result;
}

std::vector<mint> naive_power(const std::vector<mint>& values,
                              int exponent,
                              std::size_t size){
    std::vector<mint> result(size);
    if(size != 0) result[0] = 1;
    for(int iteration = 0; iteration < exponent; ++iteration){
        result = naive_multiply(result, values, size);
    }
    return result;
}

std::vector<term> duplicated_terms(const std::vector<mint>& values,
                                   std::mt19937_64& rng){
    std::vector<term> result;
    for(std::size_t degree = 0; degree < values.size(); ++degree){
        if(values[degree] == mint(0)) continue;
        const mint split(static_cast<long long>(rng() % 998244353ULL));
        result.emplace_back(degree, split);
        result.emplace_back(degree, values[degree] - split);
        if((rng() & 1ULL) != 0) result.emplace_back(degree, mint(0));
    }
    result.emplace_back(values.size() + 3, mint(17));
    result.emplace_back(values.size() + 3, mint(-17));
    std::shuffle(result.begin(), result.end(), rng);
    return result;
}

std::vector<mint> random_sparse_dense(std::size_t size,
                                      std::size_t maximum_terms,
                                      std::mt19937_64& rng){
    std::vector<mint> result(size);
    if(size == 0) return result;
    const std::size_t count = std::min(maximum_terms, size);
    for(std::size_t iteration = 0; iteration < count; ++iteration){
        const std::size_t degree = static_cast<std::size_t>(rng() % size);
        result[degree] += mint(static_cast<long long>(rng() % 100000ULL + 1));
    }
    return result;
}

void test_normalization(){
    const std::vector<term> input{
        {7, mint(0)}, {3, mint(5)}, {1, mint(8)}, {3, mint(-2)},
        {1, mint(-8)}, {3, mint(-3)}, {2, mint(11)}, {2, mint(4)}
    };
    assert(sparse_fps_normalize<998244353>(input) ==
           std::vector<term>({{2, mint(15)}}));
}

void test_random(){
    std::mt19937_64 rng(2026071317);
    for(int iteration = 0; iteration < 500; ++iteration){
        const std::size_t size = static_cast<std::size_t>(rng() % 30ULL + 1);

        auto inverse_input = random_sparse_dense(size, 7, rng);
        inverse_input[0] = mint(static_cast<long long>(rng() % 998244352ULL + 1));
        assert(sparse_fps_inv(duplicated_terms(inverse_input, rng), size) ==
               naive_inverse(inverse_input, size));

        auto logarithm_input = random_sparse_dense(size, 7, rng);
        logarithm_input[0] = 1;
        assert(sparse_fps_log(duplicated_terms(logarithm_input, rng), size) ==
               naive_logarithm(logarithm_input, size));

        auto exponential_input = random_sparse_dense(size, 7, rng);
        exponential_input[0] = 0;
        assert(sparse_fps_exp(duplicated_terms(exponential_input, rng), size) ==
               naive_exponential(exponential_input, size));

        auto power_input = random_sparse_dense(size, 7, rng);
        if(iteration % 3 == 0){
            power_input[0] = 0;
            if(size > 1) power_input[1] = 0;
        }
        const int exponent = static_cast<int>(rng() % 7ULL);
        assert(sparse_fps_pow(duplicated_terms(power_input, rng), exponent, size) ==
               naive_power(power_input, exponent, size));

        auto sparse_root = random_sparse_dense(size, 5, rng);
        if(iteration % 4 == 0){
            sparse_root[0] = 0;
            if(size > 1) sparse_root[1] = 0;
        }
        const auto square = naive_multiply(sparse_root, sparse_root, size);
        const auto recovered = sparse_fps_sqrt(duplicated_terms(square, rng), size);
        assert(recovered.has_value());
        assert(naive_multiply(*recovered, *recovered, size) == square);
    }
}

void test_boundaries(){
    const std::vector<term> empty;
    const std::vector<term> invalid_constant{{0, mint(2)}};
    assert(sparse_fps_inv(invalid_constant, 0).empty());
    assert(sparse_fps_log(invalid_constant, 0).empty());
    assert(sparse_fps_exp(invalid_constant, 0).empty());
    assert(sparse_fps_pow(invalid_constant, 1, 0).empty());
    assert(sparse_fps_sqrt(invalid_constant, 0) ==
           std::optional(std::vector<mint>{}));
    assert(sparse_fps_inv(std::vector<term>{{0, mint(3)}}, 1) ==
           std::vector<mint>{mint(3).inv()});
    assert(sparse_fps_log(std::vector<term>{{0, mint(1)}}, 1) ==
           std::vector<mint>{mint(0)});
    assert(sparse_fps_exp(empty, 1) == std::vector<mint>{mint(1)});
    assert(sparse_fps_pow(empty, 0, 6) ==
           std::vector<mint>({mint(1), mint(0), mint(0), mint(0), mint(0), mint(0)}));
    assert(sparse_fps_pow(std::vector<term>{{2, mint(3)}}, 3, 8) ==
           std::vector<mint>({0, 0, 0, 0, 0, 0, 27, 0}));
    assert(sparse_fps_pow(std::vector<term>{{3, mint(7)}}, 4, 10) ==
           std::vector<mint>(10));
    assert(sparse_fps_sqrt(empty, 7) ==
           std::optional(std::vector<mint>(7)));
    assert(!sparse_fps_sqrt(std::vector<term>{{3, mint(1)}}, 8));
    assert(!sparse_fps_sqrt(std::vector<term>{{0, mint(3)}}, 5));

    bool thrown = false;
    try{ (void)sparse_fps_inv(empty, 2); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{
        (void)sparse_fps_log(std::vector<term>{{0, mint(2)}}, 2);
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{
        (void)sparse_fps_exp(std::vector<term>{{0, mint(1)}}, 2);
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{
        (void)sparse_fps_pow(std::vector<term>{{0, mint(1)}}, -1, 2);
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    using small_mint = Modint<17>;
    using small_term = SparseFormalPowerSeriesTerm<17>;
    const std::vector<small_term> geometric{{0, small_mint(1)}, {1, small_mint(-1)}};
    const auto long_inverse = sparse_fps_inv(geometric, 24);
    assert(long_inverse == std::vector<small_mint>(24, small_mint(1)));
    thrown = false;
    try{ (void)sparse_fps_log(geometric, 18); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{
        (void)sparse_fps_exp(std::vector<small_term>{{1, small_mint(1)}}, 18);
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)sparse_fps_pow(geometric, 3, 18); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)sparse_fps_sqrt(geometric, 18); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    using binary_mint = Modint<2>;
    using binary_term = SparseFormalPowerSeriesTerm<2>;
    assert(sparse_fps_sqrt(std::vector<binary_term>{}, 2) ==
           std::optional(std::vector<binary_mint>(2)));
    thrown = false;
    try{
        (void)sparse_fps_sqrt(std::vector<binary_term>{{0, binary_mint(1)}}, 2);
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
}

void test_large_sparse(){
    constexpr std::size_t size = 100000;
    const std::vector<term> geometric{{0, mint(1)}, {1, mint(-1)}};
    const auto inverse = sparse_fps_inv(geometric, size);
    assert(inverse == std::vector<mint>(size, mint(1)));

    const auto logarithm = sparse_fps_log(geometric, size);
    assert(logarithm[0] == mint(0));
    for(std::size_t degree = 1; degree < 200; ++degree){
        assert(logarithm[degree] == -mint(1) / mint(static_cast<long long>(degree)));
    }

    const auto exponential = sparse_fps_exp(std::vector<term>{{1, mint(1)}}, size);
    mint factorial = 1;
    for(std::size_t degree = 0; degree < 200; ++degree){
        if(degree != 0) factorial *= mint(static_cast<long long>(degree));
        assert(exponential[degree] * factorial == mint(1));
    }

    const long long exponent = 123456;
    const auto powered = sparse_fps_pow(
        std::vector<term>{{0, mint(1)}, {1, mint(1)}}, exponent, size
    );
    mint binomial = 1;
    for(std::size_t degree = 0; degree < 200; ++degree){
        if(degree != 0){
            binomial *= mint(exponent - static_cast<long long>(degree) + 1);
            binomial /= mint(static_cast<long long>(degree));
        }
        assert(powered[degree] == binomial);
    }

    const auto root = sparse_fps_sqrt(
        std::vector<term>{{0, mint(1)}, {2, mint(2)}, {4, mint(1)}}, size
    );
    assert(root.has_value());
    for(std::size_t degree = 0; degree < size; ++degree){
        const mint expected = degree == 0 || degree == 2 ? mint(1) : mint(0);
        assert((*root)[degree] == expected);
    }
}

void print(const std::vector<mint>& values){
    for(std::size_t index = 0; index < values.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << values[index];
    }
    std::cout << '\n';
}

int main(){
    std::string operation;
    std::size_t size;
    std::size_t count;
    if(std::cin >> operation >> size >> count){
        long long exponent = 0;
        if(operation == "POW") std::cin >> exponent;
        std::vector<term> terms(count);
        for(auto& [degree, coefficient]: terms) std::cin >> degree >> coefficient;
        if(operation == "INV") print(sparse_fps_inv(terms, size));
        else if(operation == "LOG") print(sparse_fps_log(terms, size));
        else if(operation == "EXP") print(sparse_fps_exp(terms, size));
        else if(operation == "POW") print(sparse_fps_pow(terms, exponent, size));
        else if(operation == "SQRT"){
            const auto answer = sparse_fps_sqrt(terms, size);
            if(answer) print(*answer);
            else std::cout << "-1\n";
        }else{
            throw std::runtime_error("unknown sparse formal power series operation");
        }
        return 0;
    }
    test_normalization();
    test_random();
    test_boundaries();
    test_large_sparse();
}
