#ifndef CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_LARGE_CONVOLUTION_NTT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_LARGE_CONVOLUTION_NTT_HPP_INCLUDED

#include <algorithm>
#include <bit>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "montgomery_ntt.hpp"

// Block convolution exceeding the largest root-of-unity order in the field.
// maximum_transform_size may be reduced to trade transform size against blocks.
template<int MOD>
std::vector<Modint<MOD>> large_convolution_ntt(
    const std::vector<Modint<MOD>>& a,const std::vector<Modint<MOD>>& b,
    std::size_t maximum_transform_size=std::size_t{1}<<std::countr_zero(static_cast<unsigned>(MOD-1))
){
    if(!std::has_single_bit(maximum_transform_size) || (MOD-1)%maximum_transform_size!=0){
        throw std::invalid_argument("invalid large convolution transform limit");
    }
    if(a.empty() || b.empty()) return {};
    if(a.size()+b.size()-1<=maximum_transform_size) return fast_convolution_ntt<MOD>(a,b);
    using residue=montgomery_ntt_internal::Residue<MOD>;
    const std::size_t block_size=std::max<std::size_t>(1,maximum_transform_size/2);
    const std::size_t left_count=(a.size()+block_size-1)/block_size;
    const std::size_t right_count=(b.size()+block_size-1)/block_size;
    std::vector<std::vector<residue>> left(left_count),right(right_count);
    auto prepare=[&](const auto& source,auto& blocks){
        for(std::size_t block=0;block<blocks.size();++block){
            auto& transformed=blocks[block];transformed.resize(maximum_transform_size);
            const std::size_t offset=block*block_size;
            for(std::size_t i=0;i<std::min(block_size,source.size()-offset);++i)
                transformed[i]=residue(source[offset+i].val());
            montgomery_ntt_internal::transform<MOD>(transformed);
        }
    };
    prepare(a,left);prepare(b,right);
    std::vector<Modint<MOD>> result(a.size()+b.size()-1);
    std::vector<residue> diagonal(maximum_transform_size);
    for(std::size_t sum=0;sum<left_count+right_count-1;++sum){
        std::fill(diagonal.begin(),diagonal.end(),residue{});
        const std::size_t first=sum>=right_count?sum-right_count+1:0;
        const std::size_t last=std::min(sum+1,left_count);
        for(std::size_t i=first;i<last;++i){
            const auto& x=left[i];const auto& y=right[sum-i];
            for(std::size_t j=0;j<maximum_transform_size;++j) diagonal[j]+=x[j]*y[j];
        }
        montgomery_ntt_internal::transform<MOD>(diagonal,true);
        const std::size_t offset=sum*block_size;
        for(std::size_t j=0;j<std::min(maximum_transform_size,result.size()-offset);++j)
            result[offset+j]+=Modint<MOD>::raw(diagonal[j].value());
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_LARGE_CONVOLUTION_NTT_HPP_INCLUDED
