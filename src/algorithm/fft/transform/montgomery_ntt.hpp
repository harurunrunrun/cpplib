#ifndef CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_MONTGOMERY_NTT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_MONTGOMERY_NTT_HPP_INCLUDED

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "convolution.hpp"

namespace montgomery_ntt_internal{

template<int MOD>
class Residue{
    static_assert(MOD>=3 && MOD<(1<<30) && MOD%2==1);
    static consteval std::uint32_t negative_inverse(){
        std::uint32_t inverse=MOD;
        for(int i=0;i<5;++i) inverse*=2U-MOD*inverse;
        return 0U-inverse;
    }
    static constexpr std::uint32_t inverse_=negative_inverse();
    static constexpr std::uint32_t square_=static_cast<std::uint32_t>(
        (std::uint64_t{0}-MOD)%MOD);
    std::uint32_t value_=0;
    static std::uint32_t reduce(std::uint64_t value){
        return static_cast<std::uint32_t>((value+static_cast<std::uint64_t>(
            static_cast<std::uint32_t>(value)*inverse_)*MOD)>>32);
    }
public:
    Residue()=default;
    explicit Residue(std::uint64_t value):value_(reduce(value%MOD*square_)){}
    std::uint32_t value() const{
        const auto value=reduce(value_);
        return value>=MOD?value-MOD:value;
    }
    Residue& operator+=(Residue other){
        value_+=other.value_;
        if(value_>=2U*MOD) value_-=2U*MOD;
        return *this;
    }
    Residue& operator-=(Residue other){
        if(value_<other.value_) value_+=2U*MOD;
        value_-=other.value_;return *this;
    }
    Residue& operator*=(Residue other){
        value_=reduce(static_cast<std::uint64_t>(value_)*other.value_);return *this;
    }
    friend Residue operator+(Residue a,Residue b){return a+=b;}
    friend Residue operator-(Residue a,Residue b){return a-=b;}
    friend Residue operator*(Residue a,Residue b){return a*=b;}
    Residue pow(std::uint64_t exponent) const{
        Residue result(1),base=*this;
        while(exponent){if(exponent%2) result*=base;base*=base;exponent/=2;}
        return result;
    }
};

template<int MOD>
void transform(std::vector<Residue<MOD>>& values,bool inverse=false){
    using residue=Residue<MOD>;
    const std::size_t size=values.size();
    if(size==0 || !std::has_single_bit(size) || (MOD-1)%size!=0){
        throw std::invalid_argument("invalid Montgomery NTT length");
    }
    if(size==1) return;
    convolution_internal::bit_reverse_permute(values);
    const unsigned levels=std::countr_zero(size);
    std::size_t half=2;
    if(levels%2){
        for(std::size_t block=0;block<size;block+=2){
            const residue second=values[block+1];
            values[block+1]=values[block]-second;values[block]+=second;
        }
        half=4;
    }
    const residue primitive(convolution_internal::primitive_root<MOD>());
    for(;half<size;half*=4){
        const std::size_t length=half*2,quarter=half/2;
        residue root=primitive.pow((MOD-1)/length);
        if(inverse) root=root.pow(MOD-2);
        const residue imaginary=root.pow(quarter);
        for(std::size_t block=0;block<size;block+=length){
            residue factor(1);
            for(std::size_t j=0;j<quarter;++j){
                const residue square=factor*factor;
                const residue a=values[block+j];
                const residue b=values[block+quarter+j]*square;
                const residue c=values[block+half+j];
                const residue d=values[block+half+quarter+j]*square;
                const residue low_sum=a+b,low_difference=a-b;
                const residue high_sum=(c+d)*factor;
                const residue high_difference=(c-d)*imaginary*factor;
                values[block+j]=low_sum+high_sum;
                values[block+half+j]=low_sum-high_sum;
                values[block+quarter+j]=low_difference+high_difference;
                values[block+half+quarter+j]=low_difference-high_difference;
                factor*=root;
            }
        }
    }
    if(inverse){
        const residue scale=residue(size).pow(MOD-2);
        for(auto& value:values) value*=scale;
    }
}

} // namespace montgomery_ntt_internal

template<int MOD>
std::vector<Modint<MOD>> fast_convolution_ntt(
    const std::vector<Modint<MOD>>& a,const std::vector<Modint<MOD>>& b
){
    if(a.empty() || b.empty()) return {};
    const std::size_t length=a.size()+b.size()-1;
    if(std::min(a.size(),b.size())<=32){
        std::vector<Modint<MOD>> result(length);
        for(std::size_t i=0;i<a.size();++i)
            for(std::size_t j=0;j<b.size();++j) result[i+j]+=a[i]*b[j];
        return result;
    }
    const auto size=std::bit_ceil(length);
    if((MOD-1)%size!=0) throw std::length_error("convolution exceeds NTT root order");
    using residue=montgomery_ntt_internal::Residue<MOD>;
    std::vector<residue> left(size),right(size);
    for(std::size_t i=0;i<a.size();++i) left[i]=residue(a[i].val());
    for(std::size_t i=0;i<b.size();++i) right[i]=residue(b[i].val());
    montgomery_ntt_internal::transform<MOD>(left);
    montgomery_ntt_internal::transform<MOD>(right);
    for(std::size_t i=0;i<size;++i) left[i]*=right[i];
    montgomery_ntt_internal::transform<MOD>(left,true);
    std::vector<Modint<MOD>> result(length);
    for(std::size_t i=0;i<length;++i) result[i]=Modint<MOD>::raw(left[i].value());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_MONTGOMERY_NTT_HPP_INCLUDED
