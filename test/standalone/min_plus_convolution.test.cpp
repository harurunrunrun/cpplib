// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/fft/min_plus_convolution.hpp"

std::vector<long long> naive_min_plus(const std::vector<long long>& left,
                                      const std::vector<long long>& right){
    if(left.empty() || right.empty()) return {};
    std::vector<long long> result(left.size() + right.size() - 1,
                                  std::numeric_limits<long long>::max());
    for(std::size_t i = 0; i < left.size(); ++i){
        for(std::size_t j = 0; j < right.size(); ++j){
            result[i + j] = std::min(result[i + j], left[i] + right[j]);
        }
    }
    return result;
}

std::vector<long long> random_convex(std::mt19937_64& rng, int size){
    std::vector<long long> result(static_cast<std::size_t>(size));
    if(size == 0) return result;
    result[0] = static_cast<long long>(rng() % 101) - 50;
    long long slope = static_cast<long long>(rng() % 21) - 10;
    for(int i = 1; i < size; ++i){
        slope += static_cast<long long>(rng() % 5);
        result[static_cast<std::size_t>(i)] = result[static_cast<std::size_t>(i - 1)] + slope;
    }
    return result;
}

void test_random(){
    std::mt19937_64 rng(2026071304);
    for(int iteration = 0; iteration < 1200; ++iteration){
        const int n = static_cast<int>(rng() % 28);
        const int m = static_cast<int>(rng() % 28);
        const auto convex = random_convex(rng, n);
        const auto other_convex = random_convex(rng, m);
        std::vector<long long> arbitrary(static_cast<std::size_t>(m));
        for(long long& value: arbitrary) value = static_cast<long long>(rng() % 401) - 200;
        assert(is_discrete_convex(convex));
        assert(is_discrete_convex(other_convex));
        assert(min_plus_convolution_convex_arbitrary(convex, arbitrary) ==
               naive_min_plus(convex, arbitrary));
        assert(min_plus_convolution_arbitrary_convex(arbitrary, convex) ==
               naive_min_plus(arbitrary, convex));
        assert(min_plus_convolution_convex_convex(convex, other_convex) ==
               naive_min_plus(convex, other_convex));
    }
}

void test_boundaries(){
    assert(min_plus_convolution_convex_arbitrary(
        std::vector<long long>{}, std::vector<long long>{1, 2}
    ).empty());
    assert(min_plus_convolution_arbitrary_convex(
        std::vector<long long>{1, 2}, std::vector<long long>{}
    ).empty());
    const std::vector<long long> affine{-7, -4, -1, 2, 5};
    const std::vector<long long> equal{9, 9, 9, 9};
    assert(is_discrete_convex(affine));
    assert(is_discrete_convex(equal));
    assert(!is_discrete_convex(std::vector<long long>{0, 2, 3}));
    assert(min_plus_convolution_convex_arbitrary(affine, equal) == naive_min_plus(affine, equal));
    assert(min_plus_convolution_convex_convex(affine, equal) ==
           naive_min_plus(affine, equal));

    bool thrown = false;
    try{
        (void)min_plus_convolution_convex_arbitrary(
            std::vector<long long>{0, 2, 3}, std::vector<long long>{1, 2}
        );
    }catch(const std::invalid_argument&){ thrown = true; }
    assert(thrown);

    constexpr long long maximum = std::numeric_limits<long long>::max();
    constexpr long long minimum = std::numeric_limits<long long>::min();
    assert(min_plus_convolution_convex_arbitrary(
        std::vector<long long>{maximum - 3}, std::vector<long long>{3}
    ) == std::vector<long long>{maximum});
    assert(min_plus_convolution_arbitrary_convex(
        std::vector<long long>{-3}, std::vector<long long>{minimum + 3}
    ) == std::vector<long long>{minimum});
    thrown = false;
    try{
        (void)min_plus_convolution_convex_arbitrary(
            std::vector<long long>{maximum - 3}, std::vector<long long>{4}
        );
    }catch(const std::overflow_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{
        (void)min_plus_convolution_arbitrary_convex(
            std::vector<long long>{-4}, std::vector<long long>{minimum + 3}
        );
    }catch(const std::overflow_error&){ thrown = true; }
    assert(thrown);
}

void test_large(){
    constexpr int n = 50000;
    constexpr int m = 60000;
    std::vector<long long> convex(static_cast<std::size_t>(n));
    std::vector<long long> arbitrary(static_cast<std::size_t>(m), 0);
    for(long long i = 0; i < n; ++i) convex[static_cast<std::size_t>(i)] = i * i;
    const auto result = min_plus_convolution_convex_arbitrary(convex, arbitrary);
    assert(min_plus_convolution_convex_convex(convex, arbitrary) == result);
    assert(result.size() == static_cast<std::size_t>(n + m - 1));
    for(int index: {0, m - 1, m, n + m - 2}){
        const long long smallest_convex_index = std::max(0, index - (m - 1));
        assert(result[static_cast<std::size_t>(index)] ==
               smallest_convex_index * smallest_convex_index);
    }
}

int main(){
    char mode;
    int n, m;
    if(std::cin >> mode >> n >> m){
        std::vector<long long> left(static_cast<std::size_t>(n));
        std::vector<long long> right(static_cast<std::size_t>(m));
        for(long long& value: left) std::cin >> value;
        for(long long& value: right) std::cin >> value;
        std::vector<long long> result;
        if(mode == 'C')
            result = min_plus_convolution_convex_arbitrary(left, right);
        else if(mode == 'A')
            result = min_plus_convolution_arbitrary_convex(left, right);
        else
            result = min_plus_convolution_convex_convex(left, right);
        for(std::size_t i = 0; i < result.size(); ++i){
            if(i != 0) std::cout << ' ';
            std::cout << result[i];
        }
        std::cout << '\n';
        return 0;
    }
    test_random();
    test_boundaries();
    test_large();
}
