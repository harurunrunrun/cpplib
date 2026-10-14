#ifndef CPPLIB_SRC_ALGORITHM_MATH_SEQUENCE_LINEAR_RECURRENCE_RANGE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_SEQUENCE_LINEAR_RECURRENCE_RANGE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../polynomial/polynomial_division.hpp"

namespace math{

// Consecutive recurrence terms in O(M(d) log(start)+M(d+count)).
template<int MOD>
Polynomial<MOD> linear_recurrence_range(
    const Polynomial<MOD>& initial,const Polynomial<MOD>& coefficient,
    std::uint64_t start,std::size_t count
){
    using mint=Modint<MOD>;
    if(initial.size()!=coefficient.size()){
        throw std::invalid_argument("linear recurrence size mismatch");
    }
    Polynomial<MOD> result(count);
    if(count==0 || initial.empty()) return result;
    Polynomial<MOD> denominator(coefficient.size()+1);
    denominator[0]=mint(1);
    for(std::size_t i=0;i<coefficient.size();++i) denominator[i+1]=-coefficient[i];
    polynomial_internal::trim(denominator);
    auto numerator=polynomial_multiply<MOD>(initial,denominator);
    numerator.resize(initial.size());
    auto decomposition=polynomial_divmod<MOD>(numerator,denominator);
    for(std::size_t i=0;i<count && start<decomposition.quotient.size();++i){
        if(i>=decomposition.quotient.size()-static_cast<std::size_t>(start)) break;
        result[i]=decomposition.quotient[static_cast<std::size_t>(start)+i];
    }
    if(decomposition.remainder.empty()) return result;
    const std::size_t degree=denominator.size()-1;
    const Polynomial<MOD> reversed(denominator.rbegin(),denominator.rend());
    const auto reciprocal=fps_inv<MOD>(reversed,degree+1);
    auto reduce=[&](Polynomial<MOD> value){
        polynomial_internal::trim(value);
        if(value.size()<=degree) return value;
        const std::size_t quotient_size=value.size()-degree;
        Polynomial<MOD> high(quotient_size);
        for(std::size_t i=0;i<quotient_size;++i) high[i]=value[value.size()-1-i];
        auto quotient=polynomial_multiply<MOD>(high,reciprocal);
        quotient.resize(quotient_size);
        std::reverse(quotient.begin(),quotient.end());
        const auto product=polynomial_multiply<MOD>(quotient,denominator);
        value.resize(degree);
        for(std::size_t i=0;i<degree && i<product.size();++i) value[i]-=product[i];
        polynomial_internal::trim(value);
        return value;
    };
    Polynomial<MOD> inverse_x(degree);
    for(std::size_t i=0;i<degree;++i) inverse_x[i]=-denominator[i+1];
    // x^(-k) = x^(k mod 2) * (x^(-ceil(k/2)))^2 modulo Q.
    auto power=[&](auto&& self,std::uint64_t exponent)->Polynomial<MOD>{
        if(exponent==0) return {mint(1)};
        if(exponent==1) return inverse_x;
        auto half=self(self,exponent/2+exponent%2);
        auto square=polynomial_multiply<MOD>(half,half);
        if(exponent%2) square.insert(square.begin(),mint(0));
        return reduce(std::move(square));
    };
    auto shifted=reduce(polynomial_multiply<MOD>(power(power,start),decomposition.remainder));
    auto series=polynomial_multiply<MOD>(shifted,fps_inv<MOD>(denominator,count));
    series.resize(count);
    for(std::size_t i=0;i<count;++i) result[i]+=series[i];
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_SEQUENCE_LINEAR_RECURRENCE_RANGE_HPP_INCLUDED
