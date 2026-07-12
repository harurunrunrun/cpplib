#pragma once

#include <array>
#include <stdexcept>
#include <utility>
#include <vector>

namespace math{

bool is_prime_sqrt(long long n){
    if(n <= 1){
        return false;
    }
    if(n % 2 == 0){
        return n == 2;
    }
    for(long long d = 3; d * d <= n; d += 2){
        if(n % d == 0){
            return false;
        }
    }
    return true;
}

template<int MAX_N>
struct PrimeSieve{
    static_assert(MAX_N >= 1);

    int _n;
    int _prime_count;
    std::array<int, MAX_N + 1> min_factor_data;
    std::array<int, MAX_N + 1> prime_data;

    explicit PrimeSieve(int n = MAX_N){
        build(n);
    }

    void build(int n){
        if(n < 0 || MAX_N < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (build).");
        }
        _n = n;
        _prime_count = 0;
        min_factor_data.fill(0);
        if(_n >= 1){
            min_factor_data[1] = 1;
        }
        for(int i = 2; i <= _n; i++){
            if(min_factor_data[i] == 0){
                min_factor_data[i] = i;
                prime_data[_prime_count++] = i;
            }
            for(int j = 0; j < _prime_count; j++){
                const int p = prime_data[j];
                if(p > min_factor_data[i] || (long long)i * p > _n){
                    break;
                }
                min_factor_data[i * p] = p;
            }
        }
    }

    int size()const{
        return _n;
    }

    int prime_count()const{
        return _prime_count;
    }

    int prime(int k)const{
        if(k < 0 || _prime_count <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (prime).");
        }
        return prime_data[k];
    }

    std::vector<int> primes()const{
        return std::vector<int>(prime_data.begin(), prime_data.begin() + _prime_count);
    }

    int min_factor(int n)const{
        if(n < 0 || _n < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (min_factor).");
        }
        return min_factor_data[n];
    }

    bool is_prime(int n)const{
        if(n < 0 || _n < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (is_prime).");
        }
        return n >= 2 && min_factor_data[n] == n;
    }

    std::vector<std::pair<int,int>> factorize(int n)const{
        if(n <= 0 || _n < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (factorize).");
        }
        std::vector<std::pair<int,int>> res;
        while(n > 1){
            const int p = min_factor_data[n];
            int e = 0;
            while(n % p == 0){
                n /= p;
                e++;
            }
            res.push_back({p, e});
        }
        return res;
    }
};

} // namespace math
