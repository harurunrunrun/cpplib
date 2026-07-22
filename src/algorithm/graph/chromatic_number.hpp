#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CHROMATIC_NUMBER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CHROMATIC_NUMBER_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

int chromatic_number(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    if(n > 20)[[unlikely]]{
        throw std::runtime_error("library assertion fault: n is too large (chromatic_number).");
    }
    std::vector<std::uint32_t> adjacent(static_cast<std::size_t>(n), 0);
    for(int v = 0; v < n; v++){
        for(int to: graph[static_cast<std::size_t>(v)]){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (chromatic_number).");
            }
            if(to != v) adjacent[static_cast<std::size_t>(v)] |= std::uint32_t(1) << to;
        }
    }
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            bool a = adjacent[static_cast<std::size_t>(i)] >> j & 1U;
            bool b = adjacent[static_cast<std::size_t>(j)] >> i & 1U;
            if(a != b)[[unlikely]]{
                throw std::runtime_error("library assertion fault: graph must be undirected (chromatic_number).");
            }
        }
    }

    if(n == 0) return 0;
    const std::size_t subset_count = std::size_t{1} << n;
    std::vector<std::uint32_t> independent_count(subset_count);
    independent_count[0] = 1;
    for(std::size_t subset = 1; subset < subset_count; ++subset){
        const std::size_t vertex = static_cast<std::size_t>(
            std::countr_zero(subset)
        );
        const std::size_t remaining = subset & (subset - 1);
        const std::size_t without_neighbors = remaining &
            ~static_cast<std::size_t>(adjacent[vertex]);
        independent_count[subset] =
            independent_count[remaining] +
            independent_count[without_neighbors];
    }

    constexpr std::array<std::uint32_t, 3> moduli{
        1'000'000'007U,
        1'000'000'009U,
        998'244'353U,
    };
    std::array<std::vector<std::uint32_t>, moduli.size()> powers;
    for(auto& values: powers) values.assign(subset_count, 1);

    for(int color_count = 1; color_count <= n; ++color_count){
        for(std::size_t modulus_index = 0;
            modulus_index < moduli.size();
            ++modulus_index){
            const std::uint32_t modulus = moduli[modulus_index];
            std::uint32_t sum = 0;
            auto& values = powers[modulus_index];
            for(std::size_t subset = 0; subset < subset_count; ++subset){
                values[subset] = static_cast<std::uint32_t>(
                    static_cast<std::uint64_t>(values[subset]) *
                    independent_count[subset] % modulus
                );
                const bool negative =
                    ((n - static_cast<int>(std::popcount(subset))) & 1) != 0;
                if(negative){
                    sum = sum >= values[subset]
                        ? sum - values[subset]
                        : sum + modulus - values[subset];
                }else{
                    sum += values[subset];
                    if(sum >= modulus) sum -= modulus;
                }
            }
            if(sum != 0) return color_count;
        }
    }
    return n;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CHROMATIC_NUMBER_HPP_INCLUDED
