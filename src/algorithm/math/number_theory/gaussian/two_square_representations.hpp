#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_GAUSSIAN_TWO_SQUARE_REPRESENTATIONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_GAUSSIAN_TWO_SQUARE_REPRESENTATIONS_HPP_INCLUDED

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

#include "gaussian_integer_factorization.hpp"

namespace math{

// All ordered nonnegative (a,b) with a*a+b*b=value. Output is sorted.
inline std::vector<std::pair<std::uint64_t, std::uint64_t>>
two_square_representations(std::uint64_t value){
    if(value == 0) return {{0,0}};
    using point = std::pair<__int128_t, __int128_t>;
    auto multiply = [](point x, point y) -> point{
        return {x.first*y.first-x.second*y.second,
                x.first*y.second+x.second*y.first};
    };
    std::vector<point> representations{{1,0}};
    for(const auto [prime,exponent]: factorize_pollard_rho(value)){
        if(prime % 4 == 3){
            if(exponent % 2 != 0) return {};
            std::uint64_t scale = 1;
            for(int i=0;i<exponent/2;++i) scale *= prime;
            for(auto& [a,b]: representations){ a *= scale; b *= scale; }
        }else{
            const auto split = prime == 2 ? GaussianInteger{1,1}
                : gaussian_integer_factorization_internal::split_rational_prime(prime);
            std::vector<point> positive(exponent+1, {1,0});
            std::vector<point> negative(exponent+1, {1,0});
            for(int i=1;i<=exponent;++i){
                positive[i] = multiply(positive[i-1], {split.real,split.imag});
                negative[i] = multiply(negative[i-1], {split.real,-split.imag});
            }
            std::vector<point> next;
            for(point previous: representations){
                const int last = prime == 2 ? 0 : exponent;
                for(int i=0;i<=last;++i){
                    next.push_back(multiply(previous,
                        multiply(positive[i],negative[exponent-i])));
                }
            }
            representations = std::move(next);
        }
    }
    std::vector<std::pair<std::uint64_t,std::uint64_t>> result;
    for(auto [a,b]: representations){
        if(a<0) a=-a;
        if(b<0) b=-b;
        result.emplace_back(static_cast<std::uint64_t>(a),static_cast<std::uint64_t>(b));
        result.emplace_back(static_cast<std::uint64_t>(b),static_cast<std::uint64_t>(a));
    }
    std::sort(result.begin(),result.end());
    result.erase(std::unique(result.begin(),result.end()),result.end());
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_GAUSSIAN_TWO_SQUARE_REPRESENTATIONS_HPP_INCLUDED
