#pragma once

#include <array>
#include <stdexcept>

namespace math{

template<int MAX_N>
struct PrimeCounting{
    static_assert(MAX_N >= 0);

private:
    int _n = 0;
    std::array<bool, MAX_N + 1> is_prime_data{};
    std::array<int, MAX_N + 1> prefix_count{};

public:
    explicit PrimeCounting(int n = MAX_N){
        build(n);
    }

    void build(int n){
        if(n < 0 || MAX_N < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (build).");
        }
        _n = n;
        is_prime_data.fill(true);
        if(_n >= 0) is_prime_data[0] = false;
        if(_n >= 1) is_prime_data[1] = false;
        for(int p = 2; (long long)p * p <= _n; p++){
            if(!is_prime_data[p]) continue;
            for(long long q = (long long)p * p; q <= _n; q += p){
                is_prime_data[static_cast<int>(q)] = false;
            }
        }
        prefix_count[0] = 0;
        for(int i = 1; i <= _n; i++){
            prefix_count[i] = prefix_count[i - 1] + (is_prime_data[i] ? 1 : 0);
        }
    }

    int size() const{ return _n; }

    bool is_prime(int n) const{
        if(n < 0 || _n < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (is_prime).");
        }
        return is_prime_data[n];
    }

    int count_primes(int n) const{
        if(n < 0 || _n < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (count_primes).");
        }
        return prefix_count[n];
    }

    int count_primes_less(int n) const{
        if(n <= 0) return 0;
        if(_n < n - 1)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (count_primes_less).");
        }
        return prefix_count[n - 1];
    }
};

} // namespace math
