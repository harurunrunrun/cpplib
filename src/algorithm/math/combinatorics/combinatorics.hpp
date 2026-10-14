#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_COMBINATORICS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_COMBINATORICS_HPP_INCLUDED

#include <array>
#include <stdexcept>
#include <vector>

template<class Mint, int MAX_N = -1>
struct Combinatorics{
    static_assert(MAX_N >= 0);

    std::array<Mint, MAX_N + 1> fact;
    std::array<Mint, MAX_N + 1> inv_fact;
    std::array<Mint, MAX_N + 1> inv;

    Combinatorics(){
        fact[0] = Mint(1);
        for(int i = 1; i <= MAX_N; i++){
            fact[i] = fact[i - 1] * Mint(i);
        }
        inv_fact[MAX_N] = fact[MAX_N].inv();
        for(int i = MAX_N; i >= 1; i--){
            inv_fact[i - 1] = inv_fact[i] * Mint(i);
        }
        inv[0] = Mint(0);
        for(int i = 1; i <= MAX_N; i++){
            inv[i] = fact[i - 1] * inv_fact[i];
        }
    }

    Mint factorial(int n)const{
        if(n < 0 || MAX_N < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (factorial).");
        }
        return fact[n];
    }

    Mint inverse_factorial(int n)const{
        if(n < 0 || MAX_N < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (inverse_factorial).");
        }
        return inv_fact[n];
    }

    Mint inverse(int n)const{
        if(n <= 0 || MAX_N < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (inverse).");
        }
        return inv[n];
    }

    Mint permutation(int n, int k)const{
        if(n < 0 || MAX_N < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (permutation).");
        }
        if(k < 0 || n < k){
            return Mint(0);
        }
        return fact[n] * inv_fact[n - k];
    }

    Mint combination(int n, int k)const{
        if(n < 0 || MAX_N < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (combination).");
        }
        if(k < 0 || n < k){
            return Mint(0);
        }
        return fact[n] * inv_fact[k] * inv_fact[n - k];
    }

    Mint homogeneous(int n, int k)const{
        if(n < 0 || k < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (homogeneous).");
        }
        if(n == 0){
            return k == 0 ? Mint(1) : Mint(0);
        }
        if(MAX_N < n + k - 1)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (homogeneous).");
        }
        return combination(static_cast<int>(static_cast<long long>(n) + k - 1), k);
    }

    Mint bell_number(int n)const{
        if(n < 0 || MAX_N < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (bell_number).");
        }
        std::array<Mint, MAX_N + 1> dp{};
        dp[0] = Mint(1);
        for(int i = 0; i < n; i++){
            for(int k = i + 1; k >= 1; k--){
                dp[k] = dp[k - 1] + dp[k] * Mint(k);
            }
            dp[0] = Mint(0);
        }
        Mint res = 0;
        for(int k = 0; k <= n; k++){
            res += dp[k];
        }
        return res;
    }
};

template<class Mint>
struct Combinatorics<Mint, -1>{
    int maximum_;

    std::vector<Mint> fact;
    std::vector<Mint> inv_fact;
    std::vector<Mint> inv;

    explicit Combinatorics(int maximum = 0): maximum_(maximum){
        if(maximum < 0) throw std::invalid_argument("negative combinatorics size");
        fact.resize(static_cast<std::size_t>(maximum) + 1);
        inv_fact.resize(fact.size());
        inv.resize(fact.size());
        fact[0] = Mint(1);
        for(int i = 1; i <= maximum_; i++){
            fact[i] = fact[i - 1] * Mint(i);
        }
        inv_fact[maximum_] = fact[maximum_].inv();
        for(int i = maximum_; i >= 1; i--){
            inv_fact[i - 1] = inv_fact[i] * Mint(i);
        }
        inv[0] = Mint(0);
        for(int i = 1; i <= maximum_; i++){
            inv[i] = fact[i - 1] * inv_fact[i];
        }
    }

    Mint factorial(int n)const{
        if(n < 0 || maximum_ < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (factorial).");
        }
        return fact[n];
    }

    Mint inverse_factorial(int n)const{
        if(n < 0 || maximum_ < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (inverse_factorial).");
        }
        return inv_fact[n];
    }

    Mint inverse(int n)const{
        if(n <= 0 || maximum_ < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (inverse).");
        }
        return inv[n];
    }

    Mint permutation(int n, int k)const{
        if(n < 0 || maximum_ < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (permutation).");
        }
        if(k < 0 || n < k){
            return Mint(0);
        }
        return fact[n] * inv_fact[n - k];
    }

    Mint combination(int n, int k)const{
        if(n < 0 || maximum_ < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (combination).");
        }
        if(k < 0 || n < k){
            return Mint(0);
        }
        return fact[n] * inv_fact[k] * inv_fact[n - k];
    }

    Mint homogeneous(int n, int k)const{
        if(n < 0 || k < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (homogeneous).");
        }
        if(n == 0){
            return k == 0 ? Mint(1) : Mint(0);
        }
        if(maximum_ < static_cast<long long>(n) + k - 1)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (homogeneous).");
        }
        return combination(static_cast<int>(static_cast<long long>(n) + k - 1), k);
    }

    Mint bell_number(int n)const{
        if(n < 0 || maximum_ < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (bell_number).");
        }
        std::vector<Mint> dp(static_cast<std::size_t>(n) + 1);
        dp[0] = Mint(1);
        for(int i = 0; i < n; i++){
            for(int k = i + 1; k >= 1; k--){
                dp[k] = dp[k - 1] + dp[k] * Mint(k);
            }
            dp[0] = Mint(0);
        }
        Mint res = 0;
        for(int k = 0; k <= n; k++){
            res += dp[k];
        }
        return res;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_COMBINATORICS_HPP_INCLUDED
