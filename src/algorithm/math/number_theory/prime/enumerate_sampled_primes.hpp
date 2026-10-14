#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_PRIME_ENUMERATE_SAMPLED_PRIMES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_PRIME_ENUMERATE_SAMPLED_PRIMES_HPP_INCLUDED

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace math{

struct SampledPrimeEnumeration{
    std::uint64_t count=0;
    std::vector<int> primes;
};

// Counts primes <= limit and stores zero-based prime indices offset mod stride.
// Segmented odd-only Eratosthenes; the sieve memory is O(sqrt(limit)).
inline SampledPrimeEnumeration enumerate_sampled_primes(
    int limit,int stride=1,int offset=0
){
    if(limit<0 || stride<=0 || offset<0 || offset>=stride){
        throw std::invalid_argument("invalid enumerate_sampled_primes arguments");
    }
    SampledPrimeEnumeration result;
    if(limit<2) return result;
    auto append=[&](int prime){
        if(result.count%static_cast<unsigned>(stride)==static_cast<unsigned>(offset)){
            result.primes.push_back(prime);
        }
        ++result.count;
    };
    append(2);
    int root=0;
    while(static_cast<std::int64_t>(root+1)*(root+1)<=limit) ++root;
    std::vector<bool> composite(root+1);
    std::vector<int> base_primes;
    std::vector<std::int64_t> next;
    for(int p=3;p<=root;p+=2){
        if(composite[p]) continue;
        base_primes.push_back(p);
        next.push_back(static_cast<std::int64_t>(p)*p);
        if(p<=root/p){
            for(int multiple=p*p;multiple<=root;multiple+=2*p) composite[multiple]=true;
        }
    }
    constexpr int block_size=32768;
    std::vector<unsigned char> block(block_size);
    for(std::int64_t low=3;low<=limit;low+=2*block_size){
        const int size=static_cast<int>(std::min<std::int64_t>(block_size,(limit-low)/2+1));
        std::fill_n(block.begin(),size,0);
        const std::int64_t high=low+2*size;
        for(std::size_t i=0;i<base_primes.size();++i){
            const int p=base_primes[i];
            if(static_cast<std::int64_t>(p)*p>=high) break;
            auto position=next[i];
            for(;position<high;position+=2*p) block[(position-low)/2]=1;
            next[i]=position;
        }
        for(int i=0;i<size;++i){
            if(!block[i]) append(static_cast<int>(low+2*i));
        }
    }
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_PRIME_ENUMERATE_SAMPLED_PRIMES_HPP_INCLUDED
