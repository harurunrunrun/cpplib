#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_ONE_KTH_ROOT_MOD_PRIME_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_ONE_KTH_ROOT_MOD_PRIME_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <unordered_map>

#include "modular_kth_root.hpp"
#include "../prime/prime_factorization.hpp"

namespace math{
namespace one_kth_root_internal{

inline u64 prime_power_root(u64 value,u64 prime_factor,int exponent,u64 prime){
    const u64 order=prime-1;
    u64 coprime=order,prime_power=1;
    int valuation=0;
    while(coprime%prime_factor==0){coprime/=prime_factor;++valuation;}
    for(int i=0;i<exponent;++i) prime_power*=prime_factor;
    const u64 inverse=coprime==1?0:
        modular_kth_root_internal::inverse_coprime(prime_power%coprime,coprime);
    u64 result=pow_mod_u64(value,inverse,prime);
    if(valuation==exponent) return result;
    u64 nonresidue=2;
    while(pow_mod_u64(nonresidue,order/prime_factor,prime)==1) ++nonresidue;
    const u64 generator=pow_mod_u64(nonresidue,coprime,prime);
    const u64 subgroup_order=order/coprime;
    const u64 digit_generator=pow_mod_u64(generator,subgroup_order/prime_factor,prime);
    u64 block=1;
    while(static_cast<u128>(block)*block<prime_factor) ++block;
    std::unordered_map<u64,u64> baby;
    baby.reserve(static_cast<std::size_t>(block*2));
    u64 current=1;
    for(u64 j=0;j<block;++j){baby.emplace(current,j);current=mul_mod_u64(current,digit_generator,prime);}
    const u64 inverse_giant=pow_mod_u64(current,prime-2,prime);
    u64 position=1,projection=subgroup_order/prime_power/prime_factor;
    for(int i=0;i<valuation-exponent;++i){
        const u64 residual=mul_mod_u64(value,
            pow_mod_u64(pow_mod_u64(result,prime_power,prime),prime-2,prime),prime);
        u64 target=pow_mod_u64(residual,projection,prime);
        u64 digit=prime_factor;
        for(u64 giant=0;giant<=block;++giant){
            const auto found=baby.find(target);
            if(found!=baby.end()){
                const u128 candidate=static_cast<u128>(giant)*block+found->second;
                if(candidate<prime_factor){digit=static_cast<u64>(candidate);break;}
            }
            target=mul_mod_u64(target,inverse_giant,prime);
        }
        if(digit==prime_factor) throw std::logic_error("prime-power root lifting failed");
        result=mul_mod_u64(result,pow_mod_u64(generator,digit*position,prime),prime);
        position*=prime_factor;
        projection/=prime_factor;
    }
    return result;
}

} // namespace one_kth_root_internal

// Returns any root, avoiding enumeration of gcd(exponent,p-1) roots and avoiding
// a discrete logarithm in the full multiplicative group. Exponent zero is valid.
inline std::optional<u64> one_kth_root_mod_prime(u64 value,u64 exponent,u64 prime){
    if(!is_prime_miller_rabin(prime)) throw std::invalid_argument("root modulus must be prime");
    value%=prime;
    if(exponent==0) return value==1?std::optional<u64>(1):std::nullopt;
    if(value==0) return u64(0);
    if(prime==2) return u64(1);
    const u64 order=prime-1;
    exponent%=order;
    const u64 divisor=std::gcd(exponent,order);
    if(pow_mod_u64(value,order/divisor,prime)!=1) return std::nullopt;
    if(exponent==0) return u64(1);
    const u64 reduced_order=order/divisor;
    const u64 inverse=reduced_order==1?0:
        modular_kth_root_internal::inverse_coprime(exponent/divisor,reduced_order);
    u64 result=pow_mod_u64(value,inverse,prime);
    for(const auto [factor,power]:factorize_pollard_rho(divisor)){
        result=one_kth_root_internal::prime_power_root(result,factor,power,prime);
    }
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_ONE_KTH_ROOT_MOD_PRIME_HPP_INCLUDED
