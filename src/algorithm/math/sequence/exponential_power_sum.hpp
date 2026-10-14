#ifndef CPPLIB_SRC_ALGORITHM_MATH_SEQUENCE_EXPONENTIAL_POWER_SUM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_SEQUENCE_EXPONENTIAL_POWER_SUM_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "../../../structure/modint/modint.hpp"

namespace math{
namespace exponential_power_sum_internal{

template<int MOD>
struct Workspace{
    using mint=Modint<MOD>;
    std::vector<mint> values,inverse;

    static std::size_t coefficient_count(std::size_t exponent){
        static_assert(MOD>=3);
        if(exponent>=static_cast<std::size_t>(MOD-2)){
            throw std::invalid_argument("power-sum degree reaches field characteristic");
        }
        return exponent+1;
    }
    explicit Workspace(std::size_t exponent):values(coefficient_count(exponent)),inverse(exponent+3){
        inverse[1]=mint(1);
        for(std::size_t i=2;i<inverse.size();++i){
            inverse[i]=-mint(MOD/static_cast<int>(i))*inverse[MOD%i];
        }
        values[0]=exponent==0?mint(1):mint(0);
        if(exponent==0){
            for(auto& x:values) x=mint(1);
            return;
        }
        if(exponent>=1) values[1]=mint(1);
        std::vector<std::uint32_t> primes;
        for(std::size_t i=2;i<=exponent;++i){
            if(values[i]==mint(0)){
                primes.push_back(static_cast<std::uint32_t>(i));
                values[i]=mint(i).pow(exponent);
            }
            for(const std::uint32_t prime:primes){
                if(prime>exponent/i) break;
                values[i*prime]=values[i]*values[prime];
                if(i%prime==0) break;
            }
        }
    }

    mint limit(mint ratio) const{
        if(ratio==mint(1)) throw std::invalid_argument("power sum has a pole at ratio 1");
        const std::size_t degree=values.size()-1;
        if(ratio==mint(0)) return degree==0?mint(1):mint(0);
        mint binomial=1,prefix=1,ratio_power=ratio.pow(degree),numerator=0;
        const mint inverse_ratio=ratio.inv();
        for(std::size_t j=0;j<=degree;++j){
            if(j!=0){
                binomial*=-ratio*mint(degree+2-j)*inverse[j];
                prefix+=binomial;
            }
            numerator+=values[degree-j]*ratio_power*prefix;
            ratio_power*=inverse_ratio;
        }
        return numerator/(mint(1)-ratio).pow(degree+1);
    }

    mint interpolate(mint point) const{
        const std::size_t degree=values.size()-1;
        if(static_cast<std::size_t>(point.val())<=degree) return values[point.val()];
        std::vector<mint> suffix(degree+2,mint(1));
        for(std::size_t i=degree+1;i-- >0;) suffix[i]=suffix[i+1]*(point-mint(i));
        mint weight=1;
        for(std::size_t i=1;i<=degree;++i) weight*=inverse[i];
        if(degree%2) weight=-weight;
        mint prefix=1,result=0;
        for(std::size_t i=0;i<=degree;++i){
            result+=values[i]*prefix*suffix[i+1]*weight;
            prefix*=point-mint(i);
            if(i<degree) weight*=-mint(degree-i)*inverse[i+1];
        }
        return result;
    }
};

} // namespace exponential_power_sum_internal

// Formal rational value sum_{i>=0} ratio^i*i^exponent, ratio != 1.
template<int MOD>
Modint<MOD> exponential_power_sum_limit(Modint<MOD> ratio,std::size_t exponent){
    if(ratio==Modint<MOD>(1)) throw std::invalid_argument("power sum has a pole at ratio 1");
    if(ratio==Modint<MOD>(0)) return exponent==0?Modint<MOD>(1):Modint<MOD>(0);
    return exponential_power_sum_internal::Workspace<MOD>(exponent).limit(ratio);
}

// sum_{0<=i<count} ratio^i*i^exponent, with 0^0=1.
template<int MOD>
Modint<MOD> exponential_power_sum(
    Modint<MOD> ratio,std::size_t exponent,std::uint64_t count
){
    using mint=Modint<MOD>;
    if(count==0) return mint(0);
    if(ratio==mint(0)) return exponent==0?mint(1):mint(0);
    exponential_power_sum_internal::Workspace<MOD> workspace(exponent);
    if(ratio==mint(1)){
        mint sum=0;
        for(std::size_t i=0;i<workspace.values.size();++i){
            const mint term=workspace.values[i];
            workspace.values[i]=sum;
            sum+=term;
        }
        workspace.values.push_back(sum);
        return workspace.interpolate(mint(count%MOD));
    }
    const mint limit=workspace.limit(ratio);
    mint sum=0,power=1,inverse_power=1;
    const mint inverse_ratio=ratio.inv();
    for(auto& value:workspace.values){
        const mint term=value;
        value=(sum-limit)*inverse_power;
        sum+=power*term;
        power*=ratio;
        inverse_power*=inverse_ratio;
    }
    return ratio.pow(static_cast<long long>(count%(MOD-1)))*workspace.interpolate(mint(count%MOD))+limit;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_SEQUENCE_EXPONENTIAL_POWER_SUM_HPP_INCLUDED
