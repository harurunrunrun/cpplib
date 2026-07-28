#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_EULERIAN_NUMBERS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_EULERIAN_NUMBERS_HPP_INCLUDED

#include <cstddef>
#include <utility>
#include <vector>

#include "../../../structure/modint/modint.hpp"

namespace math{

template<int MOD>
std::vector<Modint<MOD>> eulerian_numbers(std::size_t order){
    if(order == 0) return {Modint<MOD>(1)};
    std::vector<Modint<MOD>> previous(1, Modint<MOD>(1));
    for(std::size_t size = 2; size <= order; ++size){
        std::vector<Modint<MOD>> current(size);
        for(std::size_t descents = 0; descents < size; ++descents){
            if(descents < previous.size()){
                current[descents] += previous[descents]
                    * Modint<MOD>(
                        static_cast<long long>(descents + 1)
                    );
            }
            if(descents != 0){
                current[descents] += previous[descents - 1]
                    * Modint<MOD>(
                        static_cast<long long>(size - descents)
                    );
            }
        }
        previous = std::move(current);
    }
    return previous;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_EULERIAN_NUMBERS_HPP_INCLUDED
