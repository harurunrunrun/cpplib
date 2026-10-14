#ifndef CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_MULTIVARIATE_CONVOLUTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_MULTIVARIATE_CONVOLUTION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>
#include "convolution.hpp"
#include "../../math/number_theory/prime/prime_factorization.hpp"

namespace multivariate_convolution_internal{
inline std::size_t shape_size(const std::vector<int>& shape){
    std::size_t result=1;
    for(int size:shape){
        if(size<2 || result>std::numeric_limits<std::size_t>::max()/size)
            throw std::invalid_argument("multivariate convolution: invalid shape");
        result*=size;
    }
    return result;
}
inline std::vector<std::uint32_t> convolution_mod(
    const std::vector<std::uint32_t>& a,const std::vector<std::uint32_t>& b,std::uint32_t modulus
){
    if(a.empty() || b.empty())return {};
    if(std::min(a.size(),b.size())<=32){
        std::vector<std::uint32_t> c(a.size()+b.size()-1);
        for(std::size_t i=0;i<a.size();++i)for(std::size_t j=0;j<b.size();++j)
            c[i+j]=(c[i+j]+std::uint64_t(a[i])*b[j])%modulus;
        return c;
    }
    constexpr long long p1=754974721,p2=167772161,p3=469762049;
    std::vector<Modint<p1>>a1(a.size()),b1(b.size());
    std::vector<Modint<p2>>a2(a.size()),b2(b.size());
    std::vector<Modint<p3>>a3(a.size()),b3(b.size());
    for(std::size_t i=0;i<a.size();++i){a1[i]=a[i];a2[i]=a[i];a3[i]=a[i];}
    for(std::size_t i=0;i<b.size();++i){b1[i]=b[i];b2[i]=b[i];b3[i]=b[i];}
    const auto c1=convolution_ntt(std::move(a1),std::move(b1));
    const auto c2=convolution_ntt(std::move(a2),std::move(b2));
    const auto c3=convolution_ntt(std::move(a3),std::move(b3));
    const long long inv1=Modint<p2>(p1).inv().val();
    const long long inv12=(Modint<p3>(p1)*Modint<p3>(p2)).inv().val();
    const std::uint64_t p12=std::uint64_t(p1)*p2%modulus;
    std::vector<std::uint32_t> c(c1.size());
    for(std::size_t i=0;i<c.size();++i){
        const long long x=c1[i].val();
        const long long y=((c2[i].val()-x)%p2+p2)*inv1%p2;
        const long long residue=(x+(p1%p3)*y)%p3;
        const long long z=((c3[i].val()-residue)%p3+p3)*inv12%p3;
        c[i]=(std::uint64_t(x)+std::uint64_t(p1%modulus)*y+p12*z)%modulus;
    }
    return c;
}
inline std::uint32_t power(std::uint32_t a,std::uint64_t e,std::uint32_t p){
    std::uint64_t result=1;
    for(;e;e>>=1,a=std::uint64_t(a)*a%p)if(e&1)result=result*a%p;
    return static_cast<std::uint32_t>(result);
}
inline void axis_dft(std::vector<std::uint32_t>& data,std::size_t stride,int length,
    std::uint32_t modulus,std::uint32_t root,bool inverse){
    if(inverse)root=power(root,modulus-2,modulus);
    std::vector<std::uint32_t> line(length),out(length),twiddle;
    std::vector<std::uint32_t> chirp,inverse_chirp,kernel;
    if(length<=128){
        twiddle.resize(std::size_t(length)*length);
        std::uint32_t row_root=1;
        for(int i=0;i<length;++i){
            std::uint64_t value=1;
            for(int j=0;j<length;++j){twiddle[std::size_t(i)*length+j]=value;value=value*row_root%modulus;}
            row_root=std::uint64_t(row_root)*root%modulus;
        }
    }else{
        chirp.resize(2*length-1);inverse_chirp.resize(length);
        std::uint64_t value=1,step=1;
        for(int i=0;i<2*length-1;++i){chirp[i]=value;value=value*step%modulus;step=step*root%modulus;}
        const auto inv_root=power(root,modulus-2,modulus);
        value=1;step=1;
        for(int i=0;i<length;++i){inverse_chirp[i]=value;value=value*step%modulus;step=step*inv_root%modulus;}
        kernel=chirp;
    }
    const std::uint32_t scale=inverse?power(length,modulus-2,modulus):1;
    for(std::size_t block=0;block<data.size();block+=stride*length)for(std::size_t offset=0;offset<stride;++offset){
        for(int j=0;j<length;++j)line[j]=data[block+offset+stride*j];
        if(length<=128){
            for(int i=0;i<length;++i){
                __uint128_t sum=0;
                for(int j=0;j<length;++j)sum+=std::uint64_t(line[j])*twiddle[std::size_t(i)*length+j];
                out[i]=sum%modulus;
            }
        }else{
            std::vector<std::uint32_t> left(length);
            for(int j=0;j<length;++j)left[length-1-j]=std::uint64_t(line[j])*inverse_chirp[j]%modulus;
            const auto product=convolution_mod(left,kernel,modulus);
            for(int i=0;i<length;++i)out[i]=std::uint64_t(product[length-1+i])*inverse_chirp[i]%modulus;
        }
        for(int j=0;j<length;++j)data[block+offset+stride*j]=std::uint64_t(out[j])*scale%modulus;
    }
}
}  // namespace multivariate_convolution_internal

template<int MOD=998244353>
std::vector<Modint<MOD>> multivariate_truncated_convolution(
    const std::vector<int>& shape,const std::vector<Modint<MOD>>& a,const std::vector<Modint<MOD>>& b
){
    using Mint=Modint<MOD>;using namespace multivariate_convolution_internal;
    const std::size_t size=shape_size(shape),dimensions=shape.size();
    if(a.size()!=size || b.size()!=size)throw std::invalid_argument("multivariate_truncated_convolution: shape mismatch");
    if(dimensions==0)return {a[0]*b[0]};
    std::vector<int> color(size);
    for(std::size_t i=0;i<size;++i){std::size_t rank=0,prefix=1;for(int n:shape){prefix*=n;rank+=i/prefix;}color[i]=rank%dimensions;}
    std::size_t length=1;while(length<2*size-1)length*=2;
    std::vector<std::vector<Mint>> left(dimensions,std::vector<Mint>(length)),right=left;
    for(std::size_t i=0;i<size;++i){left[color[i]][i]=a[i];right[color[i]][i]=b[i];}
    for(std::size_t c=0;c<dimensions;++c){number_theoretic_transform(left[c]);number_theoretic_transform(right[c]);}
    std::vector<Mint> temporary(dimensions);
    for(std::size_t i=0;i<length;++i){
        std::fill(temporary.begin(),temporary.end(),Mint{});
        for(std::size_t x=0;x<dimensions;++x)for(std::size_t y=0;y<dimensions;++y)
            temporary[(x+y)%dimensions]+=left[x][i]*right[y][i];
        for(std::size_t c=0;c<dimensions;++c)left[c][i]=temporary[c];
    }
    for(auto& row:left)number_theoretic_transform(row,true);
    std::vector<Mint> result(size);for(std::size_t i=0;i<size;++i)result[i]=left[color[i]][i];
    return result;
}

inline std::vector<std::uint32_t> multivariate_cyclic_convolution(
    const std::vector<int>& shape,const std::vector<std::uint32_t>& a,
    const std::vector<std::uint32_t>& b,std::uint32_t prime
){
    using namespace multivariate_convolution_internal;
    if(prime>1000000000 || !math::is_prime_miller_rabin(prime))
        throw std::invalid_argument("multivariate_cyclic_convolution: invalid prime");
    const std::size_t size=shape_size(shape);
    if(a.size()!=size || b.size()!=size)throw std::invalid_argument("multivariate_cyclic_convolution: shape mismatch");
    for(int n:shape)if((prime-1)%n)throw std::invalid_argument("multivariate_cyclic_convolution: dimension must divide prime minus one");
    auto left=a,right=b;for(auto& v:left)v%=prime;for(auto& v:right)v%=prime;
    const auto primitive=convolution_internal::primitive_root(prime);
    std::size_t stride=1;
    for(int n:shape){
        const auto root=power(primitive,(prime-1)/n,prime);
        axis_dft(left,stride,n,prime,root,false);axis_dft(right,stride,n,prime,root,false);stride*=n;
    }
    for(std::size_t i=0;i<size;++i)left[i]=std::uint64_t(left[i])*right[i]%prime;
    stride=1;
    for(int n:shape){axis_dft(left,stride,n,prime,power(primitive,(prime-1)/n,prime),true);stride*=n;}
    return left;
}

inline std::vector<std::uint32_t> multivariate_twisted_cyclic_convolution(
    const std::vector<int>& shape,const std::vector<std::uint32_t>& a,
    const std::vector<std::uint32_t>& b,std::uint32_t prime,
    const std::vector<std::uint32_t>& twists
){
    using namespace multivariate_convolution_internal;
    if(twists.size()!=shape.size() || a.size()!=shape_size(shape) || b.size()!=a.size())
        throw std::invalid_argument("multivariate_twisted_cyclic_convolution: shape mismatch");
    if(prime>1000000000 || !math::is_prime_miller_rabin(prime))
        throw std::invalid_argument("multivariate_twisted_cyclic_convolution: invalid prime");
    std::vector<std::uint32_t> multiplier(a.size(),1),inverse(a.size(),1);
    std::size_t stride=1;
    for(std::size_t axis=0;axis<shape.size();++axis){
        const auto twist=twists[axis]%prime;
        if(!twist)throw std::invalid_argument("multivariate_twisted_cyclic_convolution: zero twist");
        const auto inverse_twist=power(twist,prime-2,prime);
        std::vector<std::uint32_t> powers(shape[axis],1),inverse_powers(shape[axis],1);
        for(int i=1;i<shape[axis];++i){powers[i]=std::uint64_t(powers[i-1])*twist%prime;inverse_powers[i]=std::uint64_t(inverse_powers[i-1])*inverse_twist%prime;}
        for(std::size_t i=0;i<a.size();++i){
            const std::size_t digit=i/stride%shape[axis];
            multiplier[i]=std::uint64_t(multiplier[i])*powers[digit]%prime;
            inverse[i]=std::uint64_t(inverse[i])*inverse_powers[digit]%prime;
        }
        stride*=shape[axis];
    }
    auto left=a,right=b;
    for(std::size_t i=0;i<a.size();++i){left[i]=std::uint64_t(a[i]%prime)*multiplier[i]%prime;right[i]=std::uint64_t(b[i]%prime)*multiplier[i]%prime;}
    auto result=multivariate_cyclic_convolution(shape,left,right,prime);
    for(std::size_t i=0;i<result.size();++i)result[i]=std::uint64_t(result[i])*inverse[i]%prime;
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_MULTIVARIATE_CONVOLUTION_HPP_INCLUDED
