#ifndef CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_MULTIPLICATIVE_CONVOLUTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_MULTIPLICATIVE_CONVOLUTION_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <stdexcept>
#include <vector>
#include "convolution.hpp"
#include "../../math/number_theory/prime/prime_factorization.hpp"

template<int MOD=998244353>
std::vector<Modint<MOD>> prime_multiplicative_convolution(
    const std::vector<Modint<MOD>>& a,const std::vector<Modint<MOD>>& b
){
    const int p=static_cast<int>(a.size());
    if(a.size()!=b.size() || !math::is_prime_miller_rabin(p))
        throw std::invalid_argument("prime_multiplicative_convolution: equal prime lengths required");
    using Mint=Modint<MOD>;
    const int generator=convolution_internal::primitive_root(p);
    std::vector<Mint> left(p-1),right(p-1),answer(p);
    Mint sum_a{},sum_b{};
    for(auto x:a)sum_a+=x;
    for(auto x:b)sum_b+=x;
    answer[0]=a[0]*sum_b+b[0]*(sum_a-a[0]);
    int x=1;
    for(int k=0;k<p-1;++k){left[k]=a[x];right[k]=b[x];x=static_cast<long long>(x)*generator%p;}
    const auto product=arbitrary_mod_convolution(left,right);
    x=1;
    for(int k=0;k<p-1;++k){
        answer[x]=product[k];
        if(k+p-1<static_cast<int>(product.size()))answer[x]+=product[k+p-1];
        x=static_cast<long long>(x)*generator%p;
    }
    return answer;
}

template<int MOD=998244353>
std::vector<Modint<MOD>> power_two_multiplicative_convolution(
    const std::vector<Modint<MOD>>& a,const std::vector<Modint<MOD>>& b
){
    if(a.size()!=b.size() || !std::has_single_bit(a.size()))
        throw std::invalid_argument("power_two_multiplicative_convolution: equal power-of-two lengths required");
    using Mint=Modint<MOD>;using Layer=std::array<std::vector<Mint>,2>;
    const int size=static_cast<int>(a.size()),bits=std::countr_zero(a.size()),mask=size-1;
    std::vector<int> powers(std::max(1,size/4),1);
    for(std::size_t i=1;i<powers.size();++i)powers[i]=static_cast<long long>(powers[i-1])*5&mask;
    std::vector<Layer> left(bits+1),right(bits+1),product(bits+1);
    for(int level=0;level<=bits;++level){
        const int length=1<<std::max(0,bits-level-2);
        for(int sign=0;sign<2;++sign){left[level][sign].resize(length);right[level][sign].resize(length);product[level][sign].resize(length);}
        if(level>=bits-1){
            const int index=level==bits?0:1<<level;
            left[level][0][0]=a[index];right[level][0][0]=b[index];
            continue;
        }
        for(int sign=0;sign<2;++sign)for(int k=0;k<length;++k){
            const int odd=sign?((-powers[k])&mask):powers[k];
            const int index=(static_cast<long long>(odd)<<level)&mask;
            left[level][sign][k]=a[index];right[level][sign][k]=b[index];
        }
        for(int sign=0;sign<2;++sign){number_theoretic_transform(left[level][sign]);number_theoretic_transform(right[level][sign]);}
        for(int k=0;k<length;++k){
            const Mint x=left[level][0][k],y=left[level][1][k];
            left[level][0][k]=x+y;left[level][1][k]=x-y;
            const Mint u=right[level][0][k],v=right[level][1][k];
            right[level][0][k]=u+v;right[level][1][k]=u-v;
        }
    }
    for(int i=0;i<=bits;++i)for(int j=0;j<=bits;++j){
        const int level=std::min(bits,i+j);
        const std::size_t length=product[level][0].size();
        const std::size_t left_step=left[i][0].size()/length,right_step=right[j][0].size()/length;
        const int signs=level>=bits-1?1:2;
        for(int sign=0;sign<signs;++sign)for(std::size_t k=0;k<length;++k)
            product[level][sign][k]+=left[i][sign][k*left_step]*right[j][sign][k*right_step];
    }
    std::vector<Mint> answer(size);const Mint half=Mint(2).inv();
    for(int level=0;level<bits-1;++level){
        auto& layer=product[level];
        for(std::size_t k=0;k<layer[0].size();++k){
            const Mint x=layer[0][k],y=layer[1][k];layer[0][k]=(x+y)*half;layer[1][k]=(x-y)*half;
        }
        for(int sign=0;sign<2;++sign){
            number_theoretic_transform(layer[sign],true);
            for(std::size_t k=0;k<layer[sign].size();++k){
                const int odd=sign?((-powers[k])&mask):powers[k];
                answer[(static_cast<long long>(odd)<<level)&mask]=layer[sign][k];
            }
        }
    }
    if(bits)answer[size/2]=product[bits-1][0][0];
    answer[0]=product[bits][0][0];return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_MULTIPLICATIVE_CONVOLUTION_HPP_INCLUDED
