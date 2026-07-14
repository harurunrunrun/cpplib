#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace math{

class VisibleLatticePointsTable{
    std::vector<std::int64_t> mertens;

public:
    explicit VisibleLatticePointsTable(int limit):
        mertens(static_cast<std::size_t>(limit) + 1)
    {
        if(limit < 1)[[unlikely]]{
            throw std::invalid_argument("visible lattice limit must be positive");
        }
        std::vector<int> primes;
        std::vector<int> mobius(static_cast<std::size_t>(limit) + 1);
        std::vector<bool> composite(static_cast<std::size_t>(limit) + 1);
        mobius[1] = 1;
        for(int value = 2; value <= limit; ++value){
            if(!composite[static_cast<std::size_t>(value)]){
                primes.push_back(value);
                mobius[static_cast<std::size_t>(value)] = -1;
            }
            for(const int prime: primes){
                if(value > limit / prime) break;
                const int product = value * prime;
                composite[static_cast<std::size_t>(product)] = true;
                if(value % prime == 0){
                    mobius[static_cast<std::size_t>(product)] = 0;
                    break;
                }
                mobius[static_cast<std::size_t>(product)] =
                    -mobius[static_cast<std::size_t>(value)];
            }
        }
        for(int value = 1; value <= limit; ++value){
            mertens[static_cast<std::size_t>(value)] =
                mertens[static_cast<std::size_t>(value - 1)] +
                mobius[static_cast<std::size_t>(value)];
        }
    }

    int limit() const{ return static_cast<int>(mertens.size()) - 1; }

    std::uint64_t value(int n) const{
        if(n < 1 || limit() < n)[[unlikely]]{
            throw std::out_of_range("visible lattice query is outside the table");
        }
        __int128 coprime_pairs = 0;
        __int128 coprime_triples = 0;
        for(int left = 1; left <= n;){
            const int quotient = n / left;
            const int right = n / quotient;
            const std::int64_t mobius_sum =
                mertens[static_cast<std::size_t>(right)] -
                mertens[static_cast<std::size_t>(left - 1)];
            coprime_pairs += static_cast<__int128>(mobius_sum) * quotient * quotient;
            coprime_triples += static_cast<__int128>(mobius_sum) * quotient *
                quotient * quotient;
            left = right + 1;
        }
        const __int128 result = 3 + 3 * coprime_pairs + coprime_triples;
        return static_cast<std::uint64_t>(result);
    }
};

} // namespace math
