#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BINOMIAL_TABLES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BINOMIAL_TABLES_HPP_INCLUDED

#include <algorithm>
#include <cstdint>
#include <limits>
#include <span>
#include <stdexcept>
#include <vector>
#include "generalized_lucas_theorem.hpp"

namespace math{

class BinomialPrimeTable{
    std::uint32_t prime_;
    std::vector<std::uint32_t> factorial_, inverse_factorial_;
public:
    BinomialPrimeTable(std::uint32_t prime, std::uint32_t maximum_digit):prime_(prime){
        if(!is_prime_miller_rabin(prime))throw std::invalid_argument("BinomialPrimeTable: modulus is not prime");
        maximum_digit=std::min(maximum_digit,prime-1);
        factorial_.assign(std::size_t(maximum_digit)+1,1);
        inverse_factorial_.resize(factorial_.size());
        for(std::uint32_t i=1;i<=maximum_digit;++i)
            factorial_[i]=std::uint64_t(factorial_[i-1])*i%prime;
        inverse_factorial_.back()=generalized_lucas_internal::inverse_coprime(factorial_.back(),prime);
        for(std::uint32_t i=maximum_digit;i>0;--i)
            inverse_factorial_[i-1]=std::uint64_t(inverse_factorial_[i])*i%prime;
    }
    std::uint32_t binomial(std::uint64_t n,std::uint64_t k)const{
        if(k>n)return 0;
        std::uint64_t result=1;
        while(n || k){
            const std::uint32_t a=n%prime_,b=k%prime_;
            if(b>a)return 0;
            if(a>=factorial_.size())throw std::out_of_range("BinomialPrimeTable: digit exceeds table maximum");
            result=result*factorial_[a]%prime_*inverse_factorial_[b]%prime_*inverse_factorial_[a-b]%prime_;
            n/=prime_;k/=prime_;
        }
        return static_cast<std::uint32_t>(result);
    }
};

class BinomialCompositeTable{
    struct PrimePower{
        std::uint32_t prime,modulus;
        int exponent;
        std::uint64_t crt_weight;
        std::vector<std::uint32_t> prefix,inverse_prefix,powers;
        std::uint64_t unit_factorial(std::uint64_t n,bool inverse)const{
            const auto& table=inverse?inverse_prefix:prefix;
            std::uint64_t value=1;
            while(n){
                if(prefix.back()==modulus-1 && ((n/modulus)&1))value=modulus-value;
                value=value*table[n%modulus]%modulus;
                n/=prime;
            }
            return value;
        }
        std::uint64_t binomial(std::uint64_t n,std::uint64_t k)const{
            const auto valuation=generalized_lucas_internal::exponent_in_factorial(n,prime)
                -generalized_lucas_internal::exponent_in_factorial(k,prime)
                -generalized_lucas_internal::exponent_in_factorial(n-k,prime);
            if(valuation>=static_cast<std::uint64_t>(exponent))return 0;
            return unit_factorial(n,false)*unit_factorial(k,true)%modulus
                *unit_factorial(n-k,true)%modulus*powers[valuation]%modulus;
        }
    };
    std::uint32_t modulus_;
    std::vector<PrimePower> factors_;
public:
    explicit BinomialCompositeTable(std::uint32_t modulus):modulus_(modulus){
        if(modulus==0)throw std::invalid_argument("BinomialCompositeTable: zero modulus");
        if(modulus==1)return;
        for(const auto& [prime,exponent]:factorize_pollard_rho(modulus)){
            PrimePower part;
            part.prime=static_cast<std::uint32_t>(prime);part.exponent=exponent;
            part.modulus=static_cast<std::uint32_t>(generalized_lucas_internal::checked_prime_power(prime,exponent));
            part.prefix.assign(std::size_t(part.modulus)+1,1);
            for(std::uint64_t i=1;i<=part.modulus;++i)
                part.prefix[i]=std::uint64_t(part.prefix[i-1])*(i%prime==0?1:i)%part.modulus;
            part.inverse_prefix.resize(part.prefix.size());
            part.inverse_prefix.back()=generalized_lucas_internal::inverse_coprime(part.prefix.back(),part.modulus);
            for(std::uint64_t i=part.modulus;i>0;--i)
                part.inverse_prefix[i-1]=std::uint64_t(part.inverse_prefix[i])*(i%prime==0?1:i)%part.modulus;
            part.powers.assign(exponent+1,1);
            for(int e=1;e<=exponent;++e)part.powers[e]=std::uint64_t(part.powers[e-1])*prime;
            const std::uint64_t other=modulus/part.modulus;
            part.crt_weight=other*generalized_lucas_internal::inverse_coprime(other%part.modulus,part.modulus)%modulus;
            factors_.push_back(std::move(part));
        }
    }
    std::uint32_t binomial(std::uint64_t n,std::uint64_t k)const{
        if(k>n || modulus_==1)return 0;
        std::uint64_t answer=0;
        for(const auto& part:factors_)
            answer=(answer+part.binomial(n,k)*part.crt_weight)%modulus_;
        return static_cast<std::uint32_t>(answer);
    }
    std::uint32_t multinomial(std::span<const std::uint64_t> counts)const{
        std::uint64_t remaining=0;
        for(auto count:counts){
            if(count>std::numeric_limits<std::uint64_t>::max()-remaining)
                throw std::overflow_error("BinomialCompositeTable: sum of counts overflows");
            remaining+=count;
        }
        std::uint64_t answer=1%modulus_;
        for(auto count:counts){answer=answer*binomial(remaining,count)%modulus_;remaining-=count;}
        return static_cast<std::uint32_t>(answer);
    }
};

}  // namespace math
#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BINOMIAL_TABLES_HPP_INCLUDED
