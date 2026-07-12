#pragma once

#include <array>
#include <stdexcept>

template<class Mint, int MAX_N>
struct BinomialPrefixSum{
    static_assert(MAX_N >= 0);

    std::array<std::array<Mint, MAX_N + 1>, MAX_N + 1> comb{};
    std::array<std::array<Mint, MAX_N + 1>, MAX_N + 1> prefix{};

    BinomialPrefixSum(){
        for(int n = 0; n <= MAX_N; n++){
            comb[n][0] = Mint(1);
            comb[n][n] = Mint(1);
            for(int k = 1; k < n; k++){
                comb[n][k] = comb[n - 1][k - 1] + comb[n - 1][k];
            }
            prefix[n][0] = comb[n][0];
            for(int k = 1; k <= MAX_N; k++){
                prefix[n][k] = prefix[n][k - 1] + (k <= n ? comb[n][k] : Mint(0));
            }
        }
    }

    Mint combination(int n, int k) const{
        if(n < 0 || MAX_N < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (combination).");
        }
        if(k < 0 || n < k) return Mint(0);
        return comb[n][k];
    }

    Mint prefix_sum(int n, int k) const{
        if(n < 0 || MAX_N < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (prefix_sum).");
        }
        if(k < 0) return Mint(0);
        if(k > MAX_N) k = MAX_N;
        return prefix[n][k];
    }
};
