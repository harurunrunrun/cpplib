#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_SUBSET_SUM_COUNTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_SUBSET_SUM_COUNTS_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>
#include "../../fft/formal_power_series/formal_power_series.hpp"

template<int MOD=998244353>
std::vector<Modint<MOD>> subset_sum_counts(const std::vector<int>& weights,int maximum){
    if(maximum<0 || maximum>=MOD)throw std::invalid_argument("subset_sum_counts: invalid maximum");
    using Mint=Modint<MOD>;
    std::vector<Mint> frequency(maximum+1),inverse(maximum+1),logarithm(maximum+1);
    Mint scale(1);
    for(int weight:weights){
        if(weight<0)throw std::invalid_argument("subset_sum_counts: negative weight");
        if(weight==0)scale+=scale;else if(weight<=maximum)frequency[weight]+=Mint(1);
    }
    if(maximum)inverse[1]=Mint(1);
    for(int k=2;k<=maximum;++k)inverse[k]=-Mint(MOD/k)*inverse[MOD%k];
    for(int weight=1;weight<=maximum;++weight)if(frequency[weight]!=Mint{}){
        for(int k=1;k<=maximum/weight;++k){
            const Mint term=frequency[weight]*inverse[k];
            if(k&1)logarithm[k*weight]+=term;else logarithm[k*weight]-=term;
        }
    }
    auto result=fps_exp<MOD>(logarithm,maximum+1);
    for(auto& value:result)value*=scale;
    return result;
}

template<int MOD=998244353>
std::vector<Modint<MOD>> bounded_partition_counts(
    const std::vector<std::pair<int,std::uint64_t>>& types,int maximum
){
    if(maximum<0 || maximum>=MOD)throw std::invalid_argument("bounded_partition_counts: invalid maximum");
    using Mint=Modint<MOD>;
    std::vector<Mint> multiplicity(maximum+1),inverse(maximum+1),logarithm(maximum+1);
    for(const auto& [weight,count]:types){
        if(weight<=0)throw std::invalid_argument("bounded_partition_counts: nonpositive weight");
        if(weight>maximum || count==0)continue;
        multiplicity[weight]+=Mint(1);
        const __uint128_t cutoff=(static_cast<__uint128_t>(count)+1)*weight;
        if(cutoff<=static_cast<unsigned int>(maximum))multiplicity[static_cast<std::size_t>(cutoff)]-=Mint(1);
    }
    if(maximum)inverse[1]=Mint(1);
    for(int k=2;k<=maximum;++k)inverse[k]=-Mint(MOD/k)*inverse[MOD%k];
    for(int weight=1;weight<=maximum;++weight)if(multiplicity[weight]!=Mint{})
        for(int k=1;k<=maximum/weight;++k)logarithm[k*weight]+=multiplicity[weight]*inverse[k];
    return fps_exp<MOD>(logarithm,maximum+1);
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_SUBSET_SUM_COUNTS_HPP_INCLUDED
