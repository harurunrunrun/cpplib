#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_COMPOSITION_FAST_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_COMPOSITION_FAST_HPP_INCLUDED

#include <algorithm>
#include <bit>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../fft/transform/montgomery_ntt.hpp"
#include "../../fft/formal_power_series/formal_power_series.hpp"
#include "polynomial_common.hpp"
#include "polynomial_taylor_shift.hpp"

namespace math{
namespace polynomial_composition_fast_internal{

template<int MOD>
struct Bivariate{
    std::size_t rows,columns;
    Polynomial<MOD> coefficients;
    Bivariate(std::size_t height,std::size_t width):
        rows(height),columns(width),coefficients(height*width){}
    Modint<MOD>& operator()(std::size_t i,std::size_t j){return coefficients[i*columns+j];}
    const Modint<MOD>& operator()(std::size_t i,std::size_t j) const{return coefficients[i*columns+j];}
};

template<int MOD>
Bivariate<MOD> multiply(const Bivariate<MOD>& a,const Bivariate<MOD>& b){
    const std::size_t stride=a.columns+b.columns-1;
    Polynomial<MOD> left((a.rows-1)*stride+a.columns),right((b.rows-1)*stride+b.columns);
    for(std::size_t i=0;i<a.rows;++i)
        std::copy_n(a.coefficients.begin()+i*a.columns,a.columns,left.begin()+i*stride);
    for(std::size_t i=0;i<b.rows;++i)
        std::copy_n(b.coefficients.begin()+i*b.columns,b.columns,right.begin()+i*stride);
    Bivariate<MOD> result(a.rows+b.rows-1,stride);
    result.coefficients=fast_convolution_ntt<MOD>(left,right);
    return result;
}

template<int MOD>
Bivariate<MOD> negative_variable(Bivariate<MOD> value){
    for(std::size_t i=1;i<value.rows;i+=2)
        for(std::size_t j=0;j<value.columns;++j) value(i,j)=-value(i,j);
    return value;
}

template<int MOD>
Polynomial<MOD> middle_product(const Polynomial<MOD>& values,const Polynomial<MOD>& kernel){
    using residue=montgomery_ntt_internal::Residue<MOD>;
    const std::size_t size=std::bit_ceil(values.size());
    if((MOD-1)%size!=0) throw std::length_error("middle product exceeds NTT root order");
    std::vector<residue> left(size),right(size);
    for(std::size_t i=0;i<values.size();++i) left[i]=residue(values[i].val());
    for(std::size_t i=0;i<kernel.size();++i) right[kernel.size()-1-i]=residue(kernel[i].val());
    montgomery_ntt_internal::transform<MOD>(left);
    montgomery_ntt_internal::transform<MOD>(right);
    for(std::size_t i=0;i<size;++i) left[i]*=right[i];
    montgomery_ntt_internal::transform<MOD>(left,true);
    Polynomial<MOD> result(values.size()-kernel.size()+1);
    for(std::size_t i=0;i<result.size();++i)
        result[i]=Modint<MOD>::raw(left[kernel.size()-1+i].value());
    return result;
}

template<int MOD>
Bivariate<MOD> transpose_projection(Bivariate<MOD> denominator,const Polynomial<MOD>& outer){
    const std::size_t degree=denominator.rows-1;
    const std::size_t width=denominator.columns;
    if(degree==0){
        Bivariate<MOD> result(1,width);
        std::copy(outer.begin(),outer.end(),result.coefficients.begin());
        return result;
    }
    auto signed_denominator=negative_variable<MOD>(denominator);
    auto product=multiply<MOD>(denominator,signed_denominator);
    Bivariate<MOD> compressed(degree/2+1,product.columns);
    for(std::size_t i=0;i<compressed.rows;++i)
        for(std::size_t j=0;j<compressed.columns;++j) compressed(i,j)=product(2*i,j);
    product.coefficients.clear();product.coefficients.shrink_to_fit();
    auto child=transpose_projection<MOD>(std::move(compressed),outer);
    const std::size_t stride=2*width-1,flat_size=denominator.rows*stride;
    Polynomial<MOD> adjoint(flat_size*2-1),kernel(flat_size);
    for(std::size_t i=0;i<child.rows;++i)
        for(std::size_t j=0;j<child.columns;++j) adjoint[(2*i+degree%2)*stride+j]=child(i,j);
    for(std::size_t i=0;i<denominator.rows;++i)
        for(std::size_t j=0;j<width;++j) kernel[i*stride+j]=signed_denominator(i,j);
    const auto projected=middle_product<MOD>(adjoint,kernel);
    Bivariate<MOD> result(denominator.rows,width);
    for(std::size_t i=0;i<result.rows;++i)
        for(std::size_t j=0;j<width;++j) result(i,j)=projected[i*stride+j];
    return result;
}

} // namespace polynomial_composition_fast_internal

// Transposed bivariate Bostan--Mori composition, O(M(n) log n).
template<int MOD>
Polynomial<MOD> polynomial_compose_fast(
    Polynomial<MOD> outer,Polynomial<MOD> inner,std::size_t size
){
    using mint=Modint<MOD>;
    using namespace polynomial_composition_fast_internal;
    if(size==0) return {};
    const mint constant=inner.empty()?mint(0):inner[0];
    if(constant!=mint(0)) outer=polynomial_taylor_shift<MOD>(outer,constant);
    outer.resize(size);inner.resize(size);inner[0]=mint(0);
    Bivariate<MOD> denominator(size,2);
    denominator(0,0)=mint(1);
    for(std::size_t i=0;i<size;++i) denominator(i,1)=-inner[i];
    const auto transposed=transpose_projection<MOD>(std::move(denominator),outer);
    Polynomial<MOD> result(size);
    for(std::size_t i=0;i<size;++i) result[size-1-i]=transposed(i,0);
    return result;
}

// Returns [x^degree] weight(x)*function(x)^i, 0 <= i < count.
template<int MOD>
Polynomial<MOD> polynomial_power_projection(
    const Polynomial<MOD>& function,const Polynomial<MOD>& weight,
    std::size_t degree,std::size_t count
){
    using mint=Modint<MOD>;
    using namespace polynomial_composition_fast_internal;
    if(count==0) return {};
    Bivariate<MOD> numerator(degree+1,2),denominator(degree+1,2);
    denominator(0,0)=mint(1);
    for(std::size_t i=0;i<=degree;++i){
        if(i<function.size()) denominator(i,1)=-function[i];
        if(i<weight.size()) numerator(i,0)=weight[i];
    }
    while(degree>0){
        const auto signed_denominator=negative_variable<MOD>(denominator);
        const auto product_p=multiply<MOD>(numerator,signed_denominator);
        const auto product_q=multiply<MOD>(denominator,signed_denominator);
        const std::size_t width=std::min(count,product_p.columns);
        Bivariate<MOD> next_p(degree/2+1,width),next_q(degree/2+1,width);
        for(std::size_t i=0;i<next_p.rows;++i){
            for(std::size_t j=0;j<width;++j){
                next_p(i,j)=product_p(2*i+degree%2,j);
                next_q(i,j)=product_q(2*i,j);
            }
        }
        numerator=std::move(next_p);denominator=std::move(next_q);degree/=2;
    }
    numerator.coefficients.resize(count);denominator.coefficients.resize(count);
    if(function.empty() || function[0]==mint(0)) return numerator.coefficients;
    auto result=fast_convolution_ntt<MOD>(numerator.coefficients,fps_inv<MOD>(denominator.coefficients,count));
    result.resize(count);return result;
}

template<int MOD>
Polynomial<MOD> polynomial_reversion_fast(Polynomial<MOD> function,std::size_t size){
    using mint=Modint<MOD>;
    if(size==0) return {};
    if(size>=static_cast<std::size_t>(MOD)) throw std::invalid_argument("reversion degree reaches field characteristic");
    if(function.empty() || function[0]!=mint(0)) throw std::invalid_argument("reversion requires zero constant coefficient");
    if(size==1) return {mint(0)};
    if(function.size()<2 || function[1]==mint(0)) throw std::invalid_argument("reversion requires invertible linear coefficient");
    function.resize(size);
    const mint inverse_linear=function[1].inv();
    for(auto& x:function) x*=inverse_linear;
    const std::size_t degree=size-1;
    const auto projections=polynomial_power_projection<MOD>(function,{mint(1)},degree,size);
    Polynomial<MOD> inverse_integer(size,mint(1)),unit(degree);
    for(std::size_t i=2;i<size;++i) inverse_integer[i]=-mint(MOD/i)*inverse_integer[MOD%i];
    for(std::size_t i=1;i<=degree;++i) unit[degree-i]=mint(degree)*projections[i]*inverse_integer[i];
    auto logarithm=fps_log<MOD>(unit,degree);
    const mint exponent=-mint(degree).inv();
    for(auto& x:logarithm) x*=exponent;
    const auto powered=fps_exp<MOD>(logarithm,degree);
    Polynomial<MOD> result(size);
    mint scale=inverse_linear;
    for(std::size_t i=1;i<size;++i){result[i]=powered[i-1]*scale;scale*=inverse_linear;}
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_COMPOSITION_FAST_HPP_INCLUDED
