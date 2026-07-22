#ifndef CPPLIB_SRC_ALGORITHM_MATH_PRIME_COUNTING_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_PRIME_COUNTING_HPP_INCLUDED

#include <array>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace math{

// Counts the primes not greater than n without enumerating all integers up to n.
// The implementation stores values only for the distinct quotients floor(n / i).
inline long long prime_count(long long n){
    if(n < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (prime_count).");
    }
    if(n < 2) return 0;

    long long root = static_cast<long long>(std::sqrt(static_cast<long double>(n)));
    while((__int128)(root + 1) * (root + 1) <= n) ++root;
    while((__int128)root * root > n) --root;

    std::vector<long long> values;
    std::vector<long long> counts;
    values.reserve(static_cast<std::size_t>(root) * 2);
    counts.reserve(static_cast<std::size_t>(root) * 2);

    for(long long left = 1; left <= n;){
        const long long value = n / left;
        values.push_back(value);
        counts.push_back(value - 1);
        const long long right = n / value;
        if(right == n) break;
        left = right + 1;
    }

    const auto index_of = [&](long long value) -> std::size_t{
        if(value <= root) return values.size() - static_cast<std::size_t>(value);
        return static_cast<std::size_t>(n / value - 1);
    };

    for(long long prime = 2; prime <= root; ++prime){
        if(counts[index_of(prime)] == counts[index_of(prime - 1)]) continue;
        const long long primes_before = counts[index_of(prime - 1)];
        const long long square = prime * prime;
        for(std::size_t index = 0; index < values.size() && values[index] >= square; ++index){
            counts[index] -= counts[index_of(values[index] / prime)] - primes_before;
        }
    }
    return counts[0];
}

inline long long count_primes(long long n){
    return prime_count(n);
}

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

#endif  // CPPLIB_SRC_ALGORITHM_MATH_PRIME_COUNTING_HPP_INCLUDED
