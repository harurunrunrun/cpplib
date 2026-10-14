#ifndef CPPLIB_SRC_ALGORITHM_MATH_SEQUENCE_LINEAR_RECURRENCE_SUM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_SEQUENCE_LINEAR_RECURRENCE_SUM_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "linear_recurrence.hpp"

namespace math{

// S_n=sum_{i<n}a_i has generating-function denominator (1-x)Q(x).
template<int MOD>
Modint<MOD> linear_recurrence_prefix_sum(
    const std::vector<Modint<MOD>>& initial,
    const std::vector<Modint<MOD>>& coefficient,std::uint64_t count
){
    using mint=Modint<MOD>;
    if(initial.size()!=coefficient.size()) throw std::invalid_argument("linear recurrence size mismatch");
    const std::size_t degree=initial.size();
    if(degree==0) return mint(0);
    std::vector<mint> prefix(degree+1),summed_coefficient(degree+1);
    for(std::size_t i=0;i<degree;++i) prefix[i+1]=prefix[i]+initial[i];
    summed_coefficient[0]=coefficient[0]+mint(1);
    for(std::size_t i=1;i<degree;++i) summed_coefficient[i]=coefficient[i]-coefficient[i-1];
    summed_coefficient[degree]=-coefficient.back();
    return bostan_mori<MOD>(prefix,summed_coefficient,count);
}

template<int MOD>
Modint<MOD> linear_recurrence_interval_sum(
    const std::vector<Modint<MOD>>& initial,
    const std::vector<Modint<MOD>>& coefficient,std::uint64_t left,std::uint64_t right
){
    if(left>right) throw std::invalid_argument("reversed recurrence sum interval");
    return linear_recurrence_prefix_sum<MOD>(initial,coefficient,right)
        -linear_recurrence_prefix_sum<MOD>(initial,coefficient,left);
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_SEQUENCE_LINEAR_RECURRENCE_SUM_HPP_INCLUDED
