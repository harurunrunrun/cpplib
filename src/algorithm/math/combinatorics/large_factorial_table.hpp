#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_LARGE_FACTORIAL_TABLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_LARGE_FACTORIAL_TABLE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include "../../fft/transform/convolution.hpp"

template<int MOD, int BLOCK = 512>
class LargeFactorialTable{
    static_assert(MOD >= 2 && BLOCK > 0);
    using Mint = Modint<MOD>;
    std::uint32_t maximum_;
    std::vector<Mint> block_factorial_;
public:
    explicit LargeFactorialTable(std::uint32_t maximum = MOD-1)
        : maximum_(std::min(maximum, std::uint32_t(MOD-1))){
        const std::size_t blocks = maximum_/BLOCK;
        block_factorial_.assign(blocks+1,Mint(1));
        if(blocks == 0) return;
        const std::size_t samples = std::min(blocks,std::size_t(BLOCK)+1);
        std::vector<Mint> values(blocks);
        for(std::size_t k=0;k<samples;++k){
            Mint product(1);
            for(int j=1;j<=BLOCK;++j) product*=Mint(static_cast<long long>(k)*BLOCK+j);
            values[k]=product;
        }
        if(blocks>samples){
            std::vector<Mint> factorial(blocks,Mint(1)), inverse_factorial(blocks,Mint(1));
            for(std::size_t i=1;i<blocks;++i) factorial[i]=factorial[i-1]*Mint(static_cast<long long>(i));
            inverse_factorial.back()=factorial.back().inv();
            for(std::size_t i=blocks-1;i>0;--i)inverse_factorial[i-1]=inverse_factorial[i]*Mint(static_cast<long long>(i));
            std::vector<Mint> left(BLOCK+1),right(blocks);
            for(int i=0;i<=BLOCK;++i){
                left[i]=values[i]*inverse_factorial[i]*inverse_factorial[BLOCK-i];
                if((BLOCK-i)&1)left[i]=-left[i];
            }
            for(std::size_t i=1;i<blocks;++i)right[i]=factorial[i-1]*inverse_factorial[i];
            std::size_t transform_size=1;
            while(transform_size<left.size()+right.size()-1)transform_size<<=1;
            std::vector<Mint> convolution;
            if((MOD-1)%transform_size==0)convolution=convolution_ntt(std::move(left),std::move(right));
            else convolution=arbitrary_mod_convolution(left,right);
            for(std::size_t i=BLOCK+1;i<blocks;++i)
                values[i]=convolution[i]*factorial[i]*inverse_factorial[i-BLOCK-1];
        }
        for(std::size_t i=0;i<blocks;++i)block_factorial_[i+1]=block_factorial_[i]*values[i];
    }
    Mint factorial(std::uint64_t n) const{
        if(n>=static_cast<std::uint64_t>(MOD))return Mint(0);
        if(n>maximum_)throw std::out_of_range("LargeFactorialTable: query exceeds table maximum");
        Mint result=block_factorial_[n/BLOCK];
        for(std::uint64_t value=n/BLOCK*BLOCK+1;value<=n;++value)
            result*=Mint(static_cast<long long>(value));
        return result;
    }
    Mint range_product(std::uint64_t left,std::uint64_t right) const{
        if(left>right)return Mint(1);
        if(left==0 || right/MOD!=(left-1)/MOD)return Mint(0);
        return factorial(right%MOD)/factorial((left-1)%MOD);
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_LARGE_FACTORIAL_TABLE_HPP_INCLUDED
