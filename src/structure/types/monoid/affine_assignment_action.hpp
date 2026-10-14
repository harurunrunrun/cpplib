#ifndef CPPLIB_SRC_STRUCTURE_TYPES_MONOID_AFFINE_ASSIGNMENT_ACTION_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TYPES_MONOID_AFFINE_ASSIGNMENT_ACTION_HPP_INCLUDED

#include <optional>
#include "common_monoids.hpp"

// Assign one affine function to every vertex in a path/sequence segment.
template<class Scalar>
struct AffineAssignmentAction{
    using S=MonoidAffine<Scalar>;
    using T=std::optional<S>;
    constexpr S op(const S& a,long long,const S& b,long long)const{
        return {b.multiplier*a.multiplier,b.multiplier*a.addend+b.addend};
    }
    constexpr S e()const{return {};}
    constexpr S mapping(const T& action,S value,long long length)const{
        if(!action)return value;
        S result{},power=*action;
        while(length){
            if(length&1)result=op(result,0,power,0);
            power=op(power,0,power,0);length>>=1;
        }
        return result;
    }
    constexpr T composition(const T& newer,const T& older)const{return newer?newer:older;}
    constexpr T id()const{return std::nullopt;}
};

#endif  // CPPLIB_SRC_STRUCTURE_TYPES_MONOID_AFFINE_ASSIGNMENT_ACTION_HPP_INCLUDED
